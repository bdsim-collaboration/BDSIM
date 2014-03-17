#ifndef __ENUMS_H
#define __ENUMS_H

// types of elements

enum {
  _NONE = -1,
  _MARKER = 1,
  _DRIFT = 2,
  _PCLDRIFT = 63,
  _RF = 3,
  _SBEND = 4, 
  _QUAD  = 5,
  _SEXTUPOLE  = 6,
  _OCTUPOLE = 7,
  _MULT  = 8,
  _SOLENOID = 9,
  _ELEMENT = 10,
  _LINE = 11,
  _REV_LINE= -11, //for line inversion in sublines
  _COLLIMATOR = 12, // obsolete?
  _ECOL = 13,
  _MUSPOILER = 62,
  _RCOL = 14,
  _LASER=15,
  _MATERIAL=16,
  _RBEND=17,
  _ATOM = 18,
  _SEQUENCE = 19,

  _SCREEN = 21,
    
  _VKICK=31,
  _HKICK=32,
  
  _SAMPLER = 41,
  _CSAMPLER = 42,
  _DUMP = 43,
  _GAS = 51,
  _TUNNEL = 52,

  _TRANSFORM3D = 61
};

const char *typestr(int type);

// geometry and field formats
/* enum { */
/*   _GMAD = 0, */
/*   _MOKKA = 1 */
/* }; */

#endif
