/*
 * GMAD interface 
 * I. Agapov 2005-2006
 * bdsim v.0.3
 *
 * modification history:
*/

#ifndef _GMAD_H
#define _GMAD_H

#include <iostream>
#include <cstdio>
#include <iterator>
#include <list>
#include <map>
#include <string>
#include <cstring>

// parse the input file and construct beamline_list and options 
int gmad_parser(FILE *f);

int gmad_parser(std::string name);

/** Python interface **/ 
extern "C" {   
  int    gmad_parser_c(char *name);

  /* Interface to extern ElementList beamline_list */
  int    get_nelements(); // Length of list
  short  get_type(int);   // Type of element 
  const char*  get_name(int);   // Name of element
  double get_length(int); // Length of element
  double get_angle(int);  // Angle of element

  /* Interface to extern Options options*/
  /*
  char*  get_physicsList();
  char*  get_particleName();
  char*  get_distribType();
  char*  get_distribFile();
  int    get_numberToGenerate();
  
  double get_sigmaX();
  double get_sigmaY();
  double get_sigmaXp();
  double get_sigmaYp();
  double get_sigmaE();
  
  double get_shellX();
  double get_shellY();
  double get_shellXp();
  double get_shellYp();
  
  double get_rMin();
  double get_rMax();
 
  double get_betx();
  double get_bety();
  double get_alfx();
  double get_alfy();
  double get_emitx();
  double get_emity();
   
  */
  
  
}
#endif
