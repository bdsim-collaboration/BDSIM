#include "element.h"
#include "enums.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <float.h>

extern const char* current_line;
// extern const int VERBOSE;

namespace {
  // helper method
  void print(std::list<struct Element> l, int ident=0)
  {
    //  if(VERBOSE) 
    if(ident == 0) printf("using line %s\n",current_line);
  
    for(std::list<struct Element>::iterator it=l.begin();it!=l.end();it++)
      {
	(*it).print(ident);
      }
  }
}

Element::Element():lst(NULL) {
  flush();
}

void Element::print(int & ident)const{
  for(int i=0;i<ident;i++)
    printf("--");

  printf("->%s : %s",name.c_str(),typestr(type));

  std::list<double>::const_iterator it;
  switch(type) {
  case _DRIFT:
  case _PCLDRIFT:
  case _SBEND:
  case _RBEND:
  case _QUAD:
  case _SEXTUPOLE:
  case _OCTUPOLE:
    printf(", l=%.10g, k0=%.10g, k1=%.10g, k2=%.10g, k3=%.10g, angle=%.10g,tilt=%.10g ",
	   l,k0,k1,k2,k3,angle,tilt);
    break;
    
  case _SOLENOID:
    printf(", l=%.10g, ks=%.10g ", l, ks);
    break;
    
  case _MULT:
    printf(" , knl={");
    for(it=knl.begin();it!=knl.end();++it)
      printf("%.10g, ",(*it));
    printf("},  ksl={");
    for(it=ksl.begin();it!=ksl.end();++it)
      printf("%.10g, ",(*it));
    printf("}");
    break;
    
  case _ELEMENT:
    printf("\ngeometry file : %s\n",geometryFile.c_str());
    printf("B map file : %s\n",bmapFile.c_str());
    //printf("E map driver : %s\n",geometryFile);
    //printf("E map file : %s\n",geometryFile);
    break;
    
  case _SCREEN:
    break;
    
  case _CSAMPLER:
    printf(" length=%.10g, radius=%.10g",l, r);
    break;
    
  case _TRANSFORM3D:
    printf(" xdir=%.10g, ydir=%.10g, zdir=%.10g,  phi=%.10g, theta=%.10g,psi=%.10g",
	   xdir, ydir, zdir, phi, theta, psi);
    break;
  case _MATERIAL:
    printf(" A=%.10g, Z=%.10g, density=%.10g,  temper=%.10g, pressure=%.10g",
	   A, Z, density, temper, pressure);
    break;
  default:
    break;
  }
  
  printf("\n");
  
  if(lst != NULL)
    {
      ::print(*lst,++ident);
      ident--;
    }
}

void Element::flush() {
  l = 0;
  bmapZOffset = 0;
  B = 0;
  ks = 0;
  k0 = 0;
  k1 = 0;
  k2 = 0;
  k3 = 0;
  angle = 0;
  xsize = 0;
  ysize = 0;
  r = 0;
  phiAngleIn = 0;
  phiAngleOut = 0;
  tscint = 0;
  twindow = 0;
  tilt = 0;
  phi = 0;
  psi = 0;
  theta = 0;

  gradient = 0;

  flatlength = 0;
  taperlength = 0;
  hgap = 0;
  beampipeThickness = 0;
  aper = 0;
  aperX = 0;
  aperY = 0;
  aperYUp = 0;
  aperYDown = 0;
  aperDy = 0;
  inR = 0;
  bpRad = 0;
  outR = 0;
  waveLength = 0;

  xdir = 0;
  ydir = 0;
  zdir = 0;

  name = "";
  type = _NONE;
  precisionRegion = 0;

  A = 0;
  Z = 0;
  density = 0;      //g*cm-3
  temper = 300;     //kelvin
  pressure = 0;     //atm
  state = "";  //allowed values: "solid", "liquid", "gas"

  /*  
      knl = std::list<double>(0);
      ksl = std::list<double>(0);
      
      geometryFile
      bmapFile
      material;
  */

  //material = "";
  spec = "";
  material="";
  scintmaterial="";
  windowmaterial="";
  vacuummaterial="";
  airmaterial="";
  tunnelMaterial="";
  tunnelCavityMaterial="Air";
  tunnelRadius=DBL_MAX; 
  tunnelOffsetX=DBL_MAX; 
  floorBeamlineHeight=DBL_MAX; 
  beamlineCeilingHeight=DBL_MAX; 
  tunnelThickness=DBL_MAX; 
  tunnelSoilThickness=DBL_MAX;
  tunnelType=-1; 
}

double Element::property_lookup(char* property_name)const{
  if(!strcmp(property_name,"l")) return l;
  if(!strcmp(property_name,"bmapZOffset")) return bmapZOffset;
  if(!strcmp(property_name,"B")) return B;
  if(!strcmp(property_name,"ks")) return ks;
  if(!strcmp(property_name,"k0")) return k0;
  if(!strcmp(property_name,"k1")) return k1;
  if(!strcmp(property_name,"k2")) return k2;
  if(!strcmp(property_name,"k3")) return k3;
  if(!strcmp(property_name,"angle")) return angle;
  if(!strcmp(property_name,"phiAngleIn")) return phiAngleIn;
  if(!strcmp(property_name,"phiAngleOut")) return phiAngleOut;
  if(!strcmp(property_name,"beampipeThickness")) return beampipeThickness;
  if(!strcmp(property_name,"aper")) return aper;
  if(!strcmp(property_name,"aperX")) return aperX;
  if(!strcmp(property_name,"aperY")) return aperY;
  if(!strcmp(property_name,"aperYUp")) return aperYUp;
  if(!strcmp(property_name,"aperYDown")) return aperYDown;
  if(!strcmp(property_name,"aperDy")) return aperDy;
  if(!strcmp(property_name,"outR")) return outR;
  if(!strcmp(property_name,"inR")) return inR;
  if(!strcmp(property_name,"bpRad")) return bpRad;
  if(!strcmp(property_name,"xsize")) return xsize;
  if(!strcmp(property_name,"ysize")) return ysize;
  if(!strcmp(property_name,"xdir")) return xdir;
  if(!strcmp(property_name,"ydir")) return ydir;
  if(!strcmp(property_name,"zdir")) return zdir;
  if(!strcmp(property_name,"phi")) return phi;
  if(!strcmp(property_name,"psi")) return psi;
  if(!strcmp(property_name,"theta")) return theta;
  if(!strcmp(property_name,"waveLength")) return waveLength;
  if(!strcmp(property_name,"tilt")) return tilt;
  if(!strcmp(property_name,"gradient")) return gradient;
  if(!strcmp(property_name,"hgap")) return hgap;
  if(!strcmp(property_name,"flatlength")) return flatlength;
  if(!strcmp(property_name,"taperlength")) return taperlength;

  if(!strcmp(property_name,"A")) return A;
  if(!strcmp(property_name,"Z")) return Z;
  if(!strcmp(property_name,"density")) return density;
  if(!strcmp(property_name,"T")) return temper;
  if(!strcmp(property_name,"P")) return pressure;

  //Tunnel properties
  if(!strcmp(property_name,"tunnelRadius")) return tunnelRadius;
  if(!strcmp(property_name,"tunnelOffsetX")) return tunnelOffsetX;
  if(!strcmp(property_name,"floorBeamlineHeight")) return floorBeamlineHeight;
  if(!strcmp(property_name,"beamlineCeilingHeight")) return beamlineCeilingHeight;
  if(!strcmp(property_name,"tunnelThickness")) return tunnelThickness;
  if(!strcmp(property_name,"tunnelSoilThickness")) return tunnelSoilThickness;
  if(!strcmp(property_name,"tunnelType")) return tunnelType;

  std::cerr << "element.cc> Error: unkown property \"" << property_name << "\". Returning 0." << std::endl; 
  exit(1);
  //what about property_lookup for attributes of type string, like material?
}
