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

enum class ElementType;

// parse the input file and construct beamline_list and options 
int gmad_parser(FILE *f);

int gmad_parser(std::string name);

/** Python interface **/ 
extern "C" {   
  int    GmadParser_c(char *name);

  /* Interface to extern ElementList beamline_list */
  int          GetNelements();            // Length of list
  int          GetType(int);              // Type of element 
  const char*  GetName(int);              // Name of element
  double       GetLength(int);            // Length of element
  double       GetAngle(int);             // Angle of element
  double*      GetKs(int);                // All magnetic ks
  double       GetAper1(int);             // Aperture 1
  double       GetAper2(int);             // Aperture 2
  double       GetBeampipeThickness(int); // Beam Pipe Thickness

}
#endif
