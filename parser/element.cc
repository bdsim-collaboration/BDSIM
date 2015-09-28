#include "element.h"
#include "elementtype.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace GMAD;

namespace GMAD {
  extern std::string current_line;
}

namespace {
  // helper method
  void print(std::list<struct Element> l, int ident=0)
  {
    if(ident == 0) std::cout << "using line " << current_line << std::endl;
  
    for(std::list<struct Element>::iterator it=l.begin();it!=l.end();it++)
      {
	(*it).print(ident);
      }
  }
}

Element::Element():lst(nullptr) {
  flush();
}

void Element::print(int & ident)const{
  for(int i=0;i<ident;i++)
    printf("--");

  std::cout << "->" << name << " : " << type << std::endl;

  std::list<double>::const_iterator it;
  switch(type) {
  case ElementType::_DRIFT:
  case ElementType::_SBEND:
  case ElementType::_RBEND:
  case ElementType::_QUAD:
  case ElementType::_SEXTUPOLE:
  case ElementType::_OCTUPOLE:
  case ElementType::_DECAPOLE:
    printf(", l=%.10g, k0=%.10g, k1=%.10g, k2=%.10g, k3=%.10g, k4=%.10g, angle=%.10g,tilt=%.10g ",
	   l,k0,k1,k2,k3,k4,angle,tilt);
    break;
    
  case ElementType::_SOLENOID:
    printf(", l=%.10g, ks=%.10g ", l, ks);
    break;
    
  case ElementType::_MULT:
    printf(" , knl={");
    for(it=knl.begin();it!=knl.end();++it)
      printf("%.10g, ",(*it));
    printf("},  ksl={");
    for(it=ksl.begin();it!=ksl.end();++it)
      printf("%.10g, ",(*it));
    printf("}");
    break;
    
  case ElementType::_ELEMENT:
    printf("\ngeometry file : %s\n",geometryFile.c_str());
    printf("B map file : %s\n",bmapFile.c_str());
    //printf("E map driver : %s\n",geometryFile);
    //printf("E map file : %s\n",geometryFile);
    break;
    
  case ElementType::_SCREEN:
    break;
    
  case ElementType::_CSAMPLER:
    printf(" length=%.10g, radius=%.10g",l, r);
    break;
    
  case ElementType::_TRANSFORM3D:
    printf(" xdir=%.10g, ydir=%.10g, zdir=%.10g,  phi=%.10g, theta=%.10g,psi=%.10g",
	   xdir, ydir, zdir, phi, theta, psi);
    break;
  case ElementType::_MATERIAL:
    printf(" A=%.10g, Z=%.10g, density=%.10g,  temper=%.10g, pressure=%.10g",
	   A, Z, density, temper, pressure);
    break;
  default:
    break;
  }
  
  printf("\n");
  
  if(lst != nullptr)
    {
      ::print(*lst,++ident);
      ident--;
    }
}

void Element::flush() {
  l = 0;
  bmapXOffset = 0;
  bmapYOffset = 0;
  bmapZOffset = 0;
  B = 0;
  ks = 0;
  k0 = 0;
  k1 = 0;
  k2 = 0;
  k3 = 0;
  k4 = 0;
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

  bias = "";
  
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
  type = ElementType::_NONE;
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

  scintmaterial = "";
  windowmaterial = "";
  spec = "";
  material="";
  airmaterial="";
}

double Element::property_lookup(std::string property_name)const{
  if(property_name == "l") return l;
  if(property_name == "bmapXOffset") return bmapXOffset;
  if(property_name == "bmapYOffset") return bmapYOffset;
  if(property_name == "bmapZOffset") return bmapZOffset;
  if(property_name == "B") return B;
  if(property_name == "ks") return ks;
  if(property_name == "k0") return k0;
  if(property_name == "k1") return k1;
  if(property_name == "k2") return k2;
  if(property_name == "k3") return k3;
  if(property_name == "k4") return k4;
  if(property_name == "angle") return angle;
  if(property_name == "phiAngleIn") return phiAngleIn;
  if(property_name == "phiAngleOut") return phiAngleOut;
  if(property_name == "beampipeThickness") return beampipeThickness;
  if(property_name == "aper") return aper1;
  if(property_name == "aper1") return aper1;
  if(property_name == "aper2") return aper2;
  if(property_name == "aper3") return aper3;
  if(property_name == "aper4") return aper4;
  if(property_name == "outerDiameter") return outerDiameter;
  if(property_name == "boxSize") return outerDiameter;
  if(property_name == "outR") return 0.5*outerDiameter;
  if(property_name == "xsize") return xsize;
  if(property_name == "ysize") return ysize;
  if(property_name == "xdir") return xdir;
  if(property_name == "ydir") return ydir;
  if(property_name == "zdir") return zdir;
  if(property_name == "phi") return phi;
  if(property_name == "psi") return psi;
  if(property_name == "theta") return theta;
  if(property_name == "waveLength") return waveLength;
  if(property_name == "tilt") return tilt;
  if(property_name == "offsetX") return offsetX;
  if(property_name == "offsetY") return offsetY;
  if(property_name == "gradient") return gradient;

  if(property_name == "A") return A;
  if(property_name == "Z") return Z;
  if(property_name == "density") return density;
  if(property_name == "T") return temper;
  if(property_name == "P") return pressure;

  std::cerr << "parser.h> Error: unknown property \"" << property_name << "\". Returning 0." << std::endl; 
  exit(1);
  //what about property_lookup for attributes of type string, like material?
}
