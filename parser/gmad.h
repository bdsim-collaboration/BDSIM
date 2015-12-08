/*
 * GMAD interface 
 * I. Agapov 2005-2006
 *
 */
#ifndef _GMAD_H
#define _GMAD_H

namespace GMAD {
  /** Python interface **/ 
  extern "C" {   
    int    GmadParser_c(char *name);

    /* Interface to extern beamline_list */
    int          GetNElements();            // Length of list
    int          GetType(int);              // Type of element 
    const char*  GetName(int);              // Name of element
    double       GetLength(int);            // Length of element
    double       GetAngle(int);             // Angle of element
    double*      GetKs(int);                // All magnetic ks
    double       GetAper1(int);             // Aperture 1
    double       GetAper2(int);             // Aperture 2
    double       GetBeampipeThickness(int); // Beam Pipe Thickness

  }
}
#endif
