#ifndef FUNCS_H_
#define FUNCS_H_

int init_vars();
int velinit();
int rezero();
void get_specie_and_relative_atom_id(int abs_atom_id, int *specie_id, int *sample_atom_id);
int init_sf_hypergeo();
int init_sf_atom_explicit();
int init_tasos_grid();
int init_my_interp();
int init_npt_respa();
int init_nvt();
int init_hmc();
int calculate_ljlrc();
int fnValidateInput();

int frclong();

int fnSffrcSession();
int fnErfrcSession(int iStartMole, int iEndMole);
int fnRafrcSession(int iMole);
int vver();
int erfrc();
int rafrc();
int averages();
int calres();
int loadit();
int saveit();
int printit();
int snapshot();
int trajectory();
int echo();
int vver_nh_3();
int npt_respa();
int hmc();
int fnValidateInit();
void rezero_nvt_ts();
void rezero_npt_ts();
int fnMDmove(int nStepMD, void (*pfnRezero)(), int (*pfnAlgorithm)());
int fnVolumeChange();
int collect_aves();
int fnMetalClusterFF();
int fnInsDelMole();
int opening();
int ending();
int readins();
int siman();


#endif /*FUNCS_H_*/