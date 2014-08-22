/*
 * GMAD interface 
 * I. Agapov 2005-2006
 * bdsim v.0.3
 *
 * modification history:
*/

#ifndef _GMAD_H
#define _GMAD_H

#include <cstdio>
#include <string>

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

}
#endif
