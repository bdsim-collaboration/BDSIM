#include "element.h"
#include "enums.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

extern const char* current_line;

namespace {
  // helper method
  void print(std::list<struct Element> l, int ident=0)
  {
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
  offsetX = 0;
  offsetY = 0;
  phi = 0;
  psi = 0;
  theta = 0;

  gradient = 0;

  // new aperture model
  beampipeThickness = 0;
  aper1 = 0;
  aper2 = 0;
  aper3 = 0;
  aper4 = 0;
  apertureType = "";
  beampipeMaterial = "";

  // magnet geometry
  magnetGeometryType  = "";
  outerMaterial = "";
  outerDiameter = 0;
  
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
  scintmaterial = "";
  windowmaterial = "";
  spec = "";
  material="";
  scintmaterial="";
  windowmaterial="";
  airmaterial="";

  // tunnel
  tunnelMaterial="";
  //tunnelCavityMaterial="Air";
  tunnelRadius=0;
  tunnelOffsetX=1e6;
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
  if(!strcmp(property_name,"aper")) return aper1;
  if(!strcmp(property_name,"aper1")) return aper1;
  if(!strcmp(property_name,"aper2")) return aper2;
  if(!strcmp(property_name,"aper3")) return aper3;
  if(!strcmp(property_name,"aper4")) return aper4;
  if(!strcmp(property_name,"outerDiameter")) return outerDiameter;
  if(!strcmp(property_name,"boxSize")) return outerDiameter;
  if(!strcmp(property_name,"outR")) return 0.5*outerDiameter;
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
  if(!strcmp(property_name,"offsetX")) return offsetX;
  if(!strcmp(property_name,"offsetY")) return offsetY;
  if(!strcmp(property_name,"gradient")) return gradient;

  if(!strcmp(property_name,"A")) return A;
  if(!strcmp(property_name,"Z")) return Z;
  if(!strcmp(property_name,"density")) return density;
  if(!strcmp(property_name,"T")) return temper;
  if(!strcmp(property_name,"P")) return pressure;

  std::cerr << "parser.h> Error: unknown property \"" << property_name << "\". Returning 0." << std::endl; 
  exit(1);
  //what about property_lookup for attributes of type string, like material?
}
