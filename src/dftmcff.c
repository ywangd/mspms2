/**
 * \brief Calculate the energy of metal clusters (DFT fit)
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>
#include "mspms2.h"

extern void ffield_(int* nm, double* x, double* y, double* z, double* ff, double* boxlx, double* boxly, double* boxlz);
extern void dfield_(int* nm, double* x, double* y, double* z, double* dx, double* dy, double* dz, double* boxlx, double* boxly, double* boxlz);
extern void ffieldag_(int* nm, double* x, double* y, double* z, double* ff, double* dx, double* dy, double* dz, double* boxlx, double* boxly, double* boxlz);

/**
 * \ This is just a wrapper function for the actually fortran subroutine.
 * This function is kept just for compatability with the old code and keep
 * the modification to the minimal.
 */
void ffieldcu_(int* nm, int* ndata, double* x, double* y, double* z, double* ff)
{
	ffield_(nm, x, y, z, ff, &boxlx, &boxly, &boxlz);
}

double deriv_ffieldcu(double pos, void* params)
{
	int ndata, indexF;
	double posold;
	double energy;

	DFTMCFFPARAM *p = (DFTMCFFPARAM *)params;

	ndata = p->index;
	indexF = ndata + 1; // Fortran index

	if (p->iWhichAxis == X_AXIS)
	{
		posold = xx[ndata];
		xx[ndata] = pos;
		ffieldcu_(&natom, &indexF, xx, yy, zz, &energy);
		xx[ndata] = posold;
	}
	else if (p->iWhichAxis == Y_AXIS)
	{
		posold = yy[ndata];
		yy[ndata] = pos;
		ffieldcu_(&natom, &indexF, xx, yy, zz, &energy);
		yy[ndata] = posold;
	}
	else // Z_AXIS
	{
		posold = zz[ndata];
		zz[ndata] = pos;
		ffieldcu_(&natom, &indexF, xx, yy, zz, &energy);
		zz[ndata] = posold;
	}

	return energy*EV_TO_K*natom/epsilon_base;
}

int fnMetalClusterFF_Cu()
{
	int ii;
	int ndata; // no use at all, just for consistency with the FORTRAN code
	double energy;

	// sigma_base must be set to 1.0 since the following code is commented out.
	// Convert to real units for Fortran subroutines
	// The reduced unit of length has to be 1 Angstrom to ensure the following code
	// to be commented out correctly
	/*
	 for (ii=0;ii<natom;ii++)
	 {
	 xx[ii] *= sigma_base;
	 yy[ii] *= sigma_base;
	 zz[ii] *= sigma_base;
	 }
	 */

	// energy calculation
	ffieldcu_(&natom, &ndata, xx, yy, zz, &energy);

	udftmcff = energy*EV_TO_K/epsilon_base;
	// convert to reduced unit
	udftmcff *= natom;
	// convert to total energy for the system, keep consistence with other energies

	if (natom > 25)
	{
		dfield_(&natom, xx, yy, zz, fxl, fyl, fzl, &boxlx, &boxly, &boxlz);
		for (ii=0;ii<natom;ii++)
		{
			fxl[ii] = -fxl[ii]*natom*EV_TO_K/epsilon_base*sigma_base;
			fyl[ii] = -fyl[ii]*natom*EV_TO_K/epsilon_base*sigma_base;
			fzl[ii] = -fzl[ii]*natom*EV_TO_K/epsilon_base*sigma_base;
		}
	}
	else
	{
		// numerical forces
		DFTMCFFPARAM param;
		gsl_function FF;
		FF.function = &deriv_ffieldcu;
		FF.params = &param;
		double value, abserr;

		for (ii=0; ii<natom; ii++)
		{
			param.index = ii;

			param.iWhichAxis = X_AXIS;
			gsl_deriv_central(&FF, xx[ii], STEP_SIZE, &value, &abserr);
			fxl[ii] -= value*sigma_base;
			// printf("dx = %lf (%lf)  ", value, abserr);

			param.iWhichAxis = Y_AXIS;
			gsl_deriv_central(&FF, yy[ii], STEP_SIZE, &value, &abserr);
			fyl[ii] -= value*sigma_base;
			// printf("dy = %lf (%lf)  ", value, abserr);

			param.iWhichAxis = Z_AXIS;
			gsl_deriv_central(&FF, zz[ii], STEP_SIZE, &value, &abserr);
			fzl[ii] -= value*sigma_base;
			// printf("dz = %lf (%lf)  \n", value, abserr);
		}
	}

	// Convert back to reduced units 
	/*
	 for (ii=0;ii<natom;ii++)
	 {
	 xx[ii] /= sigma_base;
	 yy[ii] /= sigma_base;
	 zz[ii] /= sigma_base;
	 }
	 */

	return 0;
}

int fnMetalClusterFF_Ag()
{
	int ii;
	double energy;
	ffieldag_(&natom, xx, yy, zz, &energy, fxl, fyl, fzl, &boxlx, &boxly, &boxlz);
	udftmcff = energy*EV_TO_K/epsilon_base;
	udftmcff *= natom;
	for (ii=0;ii<natom;ii++)
	{
		fxl[ii] = -fxl[ii]*natom*EV_TO_K/epsilon_base*sigma_base; 
		fyl[ii] = -fyl[ii]*natom*EV_TO_K/epsilon_base*sigma_base; 
		fzl[ii] = -fzl[ii]*natom*EV_TO_K/epsilon_base*sigma_base;
	}
	
	return 0;
}

