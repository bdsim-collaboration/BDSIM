/*
 *  parser.h
 *
 *    GMAD parser functions
 *    Ilya Agapov 2005-2006
 *    bdsim v.0.3
 */

#ifndef __PARSER_H
#define __PARSER_H

#include "sym_table.h"
#ifndef _WIN32
#include <unistd.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <list>
#include <string>
#include <vector>

#include <iostream>

#include "gmad.h"
#include "getEnv.h"

std::string sBDSIMPATH = getEnv("BDSIMPATH");

//double pow(double x, double y) {return exp( y * log(x));}

int yyerror(const char *);

extern FILE* yyin;
extern int yylex();


const int MAX_EXPAND_ITERATIONS = 50;
const int MAX_MULTIPOLE_ORDER = 5;

const int _undefined = 0;

std::list<double> _tmparray;  // for reading of arrays
std::list<char*> _tmpstring;

// representation of arrays used in tokens
struct Array {
  std::vector<char*> symbols;
  double *data;
  int size;
};

const char *typestr(int type) {
  switch(type){
  case _MARKER : 
    return "marker";
  case _DRIFT :
    return "drift";
  case _PCLDRIFT :
    return "pcldrift";
  case _RF :
   return "rf";
  case _SBEND : 
    return "sbend";
  case _RBEND :
    return "rbend";
  case _QUAD :
    return "quadrupole";
  case _SEXTUPOLE :
    return "sextupole";
  case _OCTUPOLE :
    return "octupole";
  case _MULT :
    return "multipole";
  case _SOLENOID : 
    return "solenoid";
  case _ECOL : 
    return "ecol";
  case _MUSPOILER : 
    return "muspoiler";
  case _VKICK :
    return "vkick";
  case _HKICK :
    return "hkick";
  case _RCOL : 
    return "rcol";
  case _LINE :
    return "line";
  case _REV_LINE :
    return "rev_line";
  case _SAMPLER :
    return "sampler";
  case _CSAMPLER:
    return "csampler";
  case _GAS:
    return "gas";
  case _TUNNEL:
    return "tunnel";
  case _MATERIAL:
    return "material";
  case _ATOM:
    return "atom";
  case _LASER:
    return "laser";
  case _ELEMENT :
    return "element";
  case _TRANSFORM3D :
    return "transform3d";
  case _SCREEN :
    return "screen";
  default:
    return "none";
  }
}
struct Parameters params;
struct Options options;
struct Element element;

void print(struct Parameters pars)
{
  printf("printing parameters:\n");
  std::list<double>::iterator it;
  for(it = pars.knl.begin();it!=pars.knl.end();++it)
    printf(" %f ", (*it));
  printf("\n");
}

void flush(struct Element& e )
{
  e.l = 0;
  e.B = 0;
  e.ks = 0;
  e.k0 = 0;
  e.k1 = 0;
  e.k2 = 0;
  e.k3 = 0;
  e.angle = 0;
  e.phiAngleIn = 0;
  e.phiAngleOut = 0;
  e.tilt = 0;
  e.phi = 0;
  e.psi = 0;
  e.theta = 0;

  e.gradient = 0;

  e.flatlength = 0;
  e.taperlength = 0;
  e.hgap = 0;
  e.beampipeThickness = 0;
  e.aper = 0;
  e.aperX = 0;
  e.aperY = 0;
  e.aperYUp = 0;
  e.aperYDown = 0;
  e.aperDy = 0;
  e.inR = 0;
  e.bpRad = 0;
  e.outR = 0;
  e.waveLength = 0;

  e.xdir = 0;
  e.ydir = 0;
  e.zdir = 0;

  e.name = NULL;
  e.type = _NONE;

  e.A = 0;
  e.Z = 0;
  e.density = 0;      //g*cm-3
  e.temper = 300;     //kelvin
  e.pressure = 0;     //atm
  e.state = "";  //allowed values: "solid", "liquid", "gas"

  /*  
      e.knl = std::list<double>(0);
      e.ksl = std::list<double>(0);
      
      geometryFile
      bmapFile
      material;
  */

  //e.material = "";
  e.spec = "";
  e.material="";
  e.tunnelMaterial="";
  e.tunnelCavityMaterial="Air";
  e.tunnelRadius=0;
  e.tunnelOffsetX=1e6;
}

void copy_properties(std::list<struct Element>::iterator dest, std::list<struct Element>::iterator src)
{

#ifdef DEBUG 
  printf("%s %s \n",typestr((*dest).type),typestr((*src).type));
#endif
  (*dest).type = (*src).type;  
 
  (*dest).l = (*src).l;

  (*dest).angle = (*src).angle; 
  (*dest).phiAngleIn = (*src).phiAngleIn; 
  (*dest).phiAngleOut = (*src).phiAngleOut; 
  (*dest).xsize = (*src).xsize; 
  (*dest).ysize = (*src).ysize; 

  (*dest).xdir = (*src).xdir; 
  (*dest).ydir = (*src).ydir; 
  (*dest).zdir = (*src).zdir; 
  (*dest).phi = (*src).phi; 
  (*dest).theta = (*src).theta; 
  (*dest).psi = (*src).psi; 
  (*dest).waveLength = (*src).waveLength; 
 
  (*dest).flatlength = (*src).flatlength;
  (*dest).taperlength = (*src).taperlength;
  (*dest).beampipeThickness = (*src).beampipeThickness;
  (*dest).aper = (*src).aper; 
  (*dest).aperX = (*src).aperX; 
  (*dest).aperY = (*src).aperY;
  (*dest).aperYUp = (*src).aperYUp;
  (*dest).aperYDown = (*src).aperYDown;
  (*dest).aperDy = (*src).aperDy; 
  (*dest).inR = (*src).inR; 
  (*dest).bpRad = (*src).bpRad ;
  (*dest).outR = (*src).outR; 
  (*dest).tilt = (*src).tilt; 
  (*dest).B = (*src).B; 
  (*dest).ks = (*src).ks;
  (*dest).k0 = (*src).k0;
  (*dest).k1 = (*src).k1;
  (*dest).k2 = (*src).k2;
  (*dest).k3 = (*src).k3;
  (*dest).knl = (*src).knl;
  (*dest).ksl = (*src).ksl;
  (*dest).blmLocZ = (*src).blmLocZ;
  (*dest).blmLocTheta = (*src).blmLocTheta;
  (*dest).hgap = (*src).hgap;

  (*dest).gradient = (*src).gradient; 

  (*dest).A = (*src).A;
  (*dest).Z = (*src).Z;
  (*dest).density = (*src).density;
  (*dest).temper = (*src).temper; 
  (*dest).pressure = (*src).pressure; 
  (*dest).state = (*src).state; 
  (*dest).components = (*src).components;
  (*dest).componentsWeights = (*src).componentsWeights;
  (*dest).componentsFractions = (*src).componentsFractions;
  (*dest).symbol = (*src).symbol;

  (*dest).geometryFile = (*src).geometryFile;

  (*dest).bmapFile = (*src).bmapFile;
  (*dest).precisionRegion = (*src).precisionRegion;

  (*dest).material = (*src).material;

  (*dest).tunnelMaterial = (*src).tunnelMaterial;
  (*dest).tunnelCavityMaterial = (*src).tunnelCavityMaterial;

  (*dest).tunnelRadius = (*src).tunnelRadius;
  (*dest).tunnelOffsetX = (*src).tunnelOffsetX;

  (*dest).spec = (*src).spec;

  return;
} 

void inherit_properties(struct Element e)
{
  // copy parameters into temporary buffer params from element e
  // parameters already set in params have priority and are not overridden
  
  if(!params.lset) { params.l = e.l; params.lset = 1; }
  if(!params.Bset) { params.B = e.B; params.Bset = 1; }
  if(!params.ksset) { params.ks = e.ks; params.ksset = 1; }
  if(!params.k0set) { params.k0 = e.k0; params.k0set = 1; }
  if(!params.k1set) { params.k1 = e.k1; params.k1set = 1; }
  if(!params.k2set) { params.k2 = e.k2; params.k2set = 1; }
  if(!params.k3set) { params.k3 = e.k3; params.k3set = 1; }
  if(!params.angleset) { params.angle = e.angle; params.angleset = 1; }
  if(!params.phiAngleInset) { params.phiAngleIn = e.phiAngleIn; params.phiAngleInset = 1; }
  if(!params.phiAngleOutset) { params.phiAngleOut = e.phiAngleOut; params.phiAngleOutset = 1; }
  if(!params.xsizeset) { params.xsize = e.xsize; params.xsizeset = 1; }
  if(!params.ysizeset) { params.ysize = e.ysize; params.ysizeset = 1; }
 
  if(!params.xdirset) { params.xdir = e.xdir; params.xdirset = 1; }
  if(!params.ydirset) { params.ydir = e.ydir; params.ydirset = 1; }
  if(!params.zdirset) { params.zdir = e.zdir; params.zdirset = 1; }
  if(!params.waveLength) { params.waveLength = e.waveLength; params.waveLengthset = 1; }
  if(!params.phiset) { params.phi = e.phi; params.phiset = 1; }
  if(!params.psiset) { params.psi = e.psi; params.psiset = 1; }
  if(!params.thetaset) { params.theta = e.theta; params.thetaset = 1; }
  if(!params.hgapset) { params.hgap = e.hgap; params.hgapset = 1; }
  if(!params.flatlengthset) { params.flatlength = e.flatlength; params.flatlengthset = 1; }
  if(!params.taperlengthset) { params.taperlength = e.taperlength; params.taperlengthset = 1; }

  if(!params.tscintset) { params.tscint = e.tscint; params.tscintset = 1; }

  //materials
  if(!params.Aset) { params.A = e.A; params.Aset = 1; }
  if(!params.Zset) { params.Z = e.Z; params.Zset = 1; }
  if(!params.densityset) { params.density = e.density; params.densityset = 1; }
  if(!params.temperset) { params.temper = e.temper; params.temperset = 1; }
  if(!params.pressureset) { params.pressure = e.pressure; params.pressureset = 1; }
  if(!params.stateset) { strncpy(params.state, e.state.c_str(),64); params.stateset = 1; }
  if(!params.symbolset) { strncpy(params.symbol, e.symbol.c_str(),64); params.symbolset = 1; }
  if(!params.componentsset) 
    { params.components = e.components; params.componentsset = 1; }
  if(!params.componentsWeightsset) 
    { params.componentsWeights = e.componentsWeights; params.componentsWeightsset = 1; }
  if(!params.componentsFractionsset) 
    { params.componentsFractions = e.componentsFractions; params.componentsFractionsset = 1; }

  if(!params.beampipeThicknessset) { params.beampipeThickness = e.beampipeThickness; params.beampipeThicknessset = 1; }
  if(!params.aperset) { params.aper = e.aper; params.aperset = 1; }
  if(!params.aperXset) { params.aperX = e.aperX; params.aperXset = 1; }
  if(!params.aperYset) { params.aperY = e.aperY; params.aperYset = 1; }
  if(!params.aperYUpset) { params.aperYUp = e.aperYUp; params.aperYUpset = 1; }
  if(!params.aperYDownset) { params.aperYDown = e.aperYDown; params.aperYDownset = 1; }
  if(!params.aperDyset) { params.aperDy = e.aperDy; params.aperDyset = 1; }
  if(!params.inRset) { params.inR = e.inR; params.inRset = 1; }
  if(!params.bpRadset) { params.bpRad = e.bpRad; params.bpRadset = 1; }
  if(!params.outRset) { params.outR = e.outR; params.outRset = 1; }

  if(!params.gradientset) { params.gradient = e.gradient; params.gradientset = 1; }

  if(!params.tiltset) { params.tilt = e.tilt; params.tiltset = 1; }
  if(!params.knlset) { params.knl = e.knl; params.knlset = 1; }
  if(!params.kslset) { params.ksl = e.ksl; params.kslset = 1; }
  //beam loss monitor locations
  if(!params.blmLocZset) { params.blmLocZ = e.blmLocZ; params.blmLocZset = 1; }
  if(!params.blmLocThetaset) { params.blmLocTheta = e.blmLocTheta; params.blmLocThetaset = 1; }

  if(!params.specset) { strncpy(params.spec,e.spec.c_str(),1024); params.specset = 1; }
  if(!params.materialset) { strncpy(params.material,e.spec.c_str(),64); params.materialset = 1; }
  if(!params.tunnelmaterialset) { strncpy(params.tunnelMaterial,e.spec.c_str(),64); params.tunnelmaterialset = 1; }
  if(!params.tunnelcavitymaterialset) { strncpy(params.tunnelCavityMaterial,e.spec.c_str(),64); params.tunnelcavitymaterialset = 1; }
  if(!params.tunnelRadiusset) { params.tunnelRadius = e.tunnelRadius; params.tunnelRadiusset = 1; }
  if(!params.tunnelOffsetXset) { params.tunnelOffsetX = e.tunnelOffsetX; params.tunnelOffsetXset = 1; }
  if(!params.precisionRegionset) { params.precisionRegion = e.precisionRegion; params.precisionRegionset = 1; }



}

void set_vector(std::list<double>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back(src->data[i]);
#ifdef DEBUG 
    std::cout << src->data[i] << " ";
#endif
  }
#ifdef DEBUG 
  std::cout << std::endl;
#endif
  
}


void set_vector(std::list<char*>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back(src->symbols[i]);
#ifdef DEBUG 
    std::cout << src->symbols[i] << " ";
#endif
  }
#ifdef DEBUG 
  std::cout << std::endl;
#endif
}

void set_vector(std::list<const char*>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back(src->symbols[i]);
#ifdef DEBUG 
    std::cout << src->symbols[i] << " ";
#endif
  }
#ifdef DEBUG 
  std::cout << std::endl;
#endif
}

void set_vector(std::list<std::string>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back((std::string)src->symbols[i]);
#ifdef DEBUG 
    std::cout << (std::string)src->symbols[i] << " ";
#endif
  }
#ifdef DEBUG 
  std::cout << std::endl;
#endif
}


void set_vector(std::list<int>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back((int)(src->data[i]));
#ifdef DEBUG 
    std::cout << (int)(src->data[i]) << " ";
#endif
  }
#ifdef DEBUG 
  std::cout << std::endl;
#endif
}


// list of all encountered elements
std::list<struct Element> element_list;

// temporary list
std::list<struct Element> tmp_list;

std::list<struct Element> beamline_list;
std::list<struct Element> material_list;
std::list<struct Element> atom_list;

const char* current_line = "";
const char* current_start = "";
const char* current_end = "";

struct symtab *symtab; 

extern struct symtab * symlook(const char *s);

std::list<struct Element>::iterator element_lookup(const char *name);
std::list<struct Element>::iterator element_lookup(const char *name, std::list<struct Element>& el);
int write_table(struct Parameters pars,const char* name, int type, std::list<struct Element> *lst=NULL);
int expand_line(char *name, char *start, char *end);
void print(std::list<struct Element> l, int ident=0);

// *********************
// functions
// *********************


void help()
{
  printf("helping...\n");
}

void quit()
{
  printf("parsing complete...\n");
  exit(0);
}

int write_table(struct Parameters params,const char* name, int type, std::list<struct Element> *lst)
{
#ifdef DEBUG 
  printf("k1=%.10g, k2=%.10g, k3=%.10g, type=%d, lset = %d\n", params.k1, params.k2, params.k3, type, params.lset);
#endif
  struct Element e;
  flush(e);
  // common parameters for all elements
  e.name = name;
  e.lst = NULL;
  e.aper = params.aper;
  e.aperX = params.aperX;
  e.aperY = params.aperY;
  e.bpRad = params.bpRad;
  e.outR = params.outR;
  e.xsize = params.xsize;
  e.ysize = params.ysize;
  e.material = params.material;  
  e.tunnelMaterial = params.tunnelMaterial;  
  e.tunnelCavityMaterial = params.tunnelCavityMaterial;  
  e.tunnelRadius = params.tunnelRadius;
  e.tunnelOffsetX = params.tunnelOffsetX;
  e.precisionRegion = params.precisionRegion;
  
  //specific parameters
  switch(type) {

  case _MARKER :
    e.type= _MARKER;
    break;

  case _DRIFT:
    e.type = _DRIFT;
    e.l = params.l;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    if(params.phiAngleInset)
      e.phiAngleIn = params.phiAngleIn;
    if(params.phiAngleOutset)
      e.phiAngleOut = params.phiAngleOut;
    if(params.beampipeThicknessset)
      e.beampipeThickness = params.beampipeThickness;
    break;

  case _PCLDRIFT:
    e.type = _PCLDRIFT;
    e.l = params.l;
    if(params.blmLocZset) e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset) e.blmLocTheta = params.blmLocTheta;
    if(params.aperYUpset) e.aperYUp = params.aperYUp;	
    if(params.aperYDownset) e.aperYDown = params.aperYDown;
    if(params.aperDyset) e.aperDy = params.aperDy;
    if(params.beampipeThicknessset) e.beampipeThickness = params.beampipeThickness;
    break;

  case _RF:
    e.type = _RF;
    e.l = params.l;
    e.gradient = params.gradient;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _SBEND:
    e.type = _SBEND;
    e.l = params.l;
    e.B = params.B;
    e.angle = params.angle;
    e.hgap = params.hgap;
    e.k1 = params.k1;
    if(params.tiltset) e.tilt = params.tilt;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    
    break;

  case _RBEND:
    e.type = _RBEND;
    e.l = params.l;
    e.B = params.B;
    e.angle = params.angle;
    e.hgap = params.hgap;
    e.k1 = params.k1;
    if(params.tiltset) e.tilt = params.tilt;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    
    break;

  case _VKICK:
    e.type = _VKICK;
    e.l = params.l;
    e.B = params.B;
    e.angle = params.angle;
    if(params.tiltset) e.tilt = params.tilt;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    
    break;

  case _HKICK:
    e.type = _HKICK;
    e.l = params.l;
    e.B = params.B;
    e.angle = params.angle;
    if(params.tiltset) e.tilt = params.tilt;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    
    break;

  case _QUAD:
    e.type = _QUAD;      
    e.l = params.l;
    
    if(params.k0set) {
      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type QUADRUPOLE\n",name);
    }
    if(params.k1set) {
      e.k1 = params.k1;
    }
    if(params.k2set) {
      if(VERBOSE)
	printf("Warning: k2 will not be set for element %s of type QUADRUPOLE\n",name);
    }
    if(params.k3set) {
      if(VERBOSE)
	printf("Warning: k3 will not be set for element %s of type QUADRUPOLE\n",name);
    }
    if(params.tiltset) {
      e.tilt = params.tilt;
    }
    e.spec = std::string(params.spec); 
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _SEXTUPOLE:
    e.type = _SEXTUPOLE;
    e.l = params.l;
    
    if(params.k0set) {
      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type SEXTUPOLE\n",name);
    }
    if(params.k1set) {
      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type SEXTUPOLE\n",name);
    }
    if(params.k2set) {
      e.k2 = params.k2;
    }
    if(params.k3set) {
      if(VERBOSE)
	printf("Warning: k3 will not be set for element %s of type SEXTUPOLE\n",name);
    }
    if(params.tiltset) {
      e.tilt = params.tilt;
    }
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _OCTUPOLE:
    e.type = _OCTUPOLE;
    e.l = params.l;
    
    if(params.k0set) {
      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type OCTUPOLE\n",name);
    }
    if(params.k1set) {
      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type OCTUPOLE\n",name);
    }
    if(params.k2set) {
      if(VERBOSE)
	printf("Warning: k2 will not be set for element %s of type OCTUPOLE\n",name);
    }
    if(params.k3set) {
      e.k3 = params.k3;
    }
    if(params.tiltset) {
      e.tilt = params.tilt;
    }
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _MULT:
    e.type = _MULT;
    e.l = params.l;
    
    if(params.knlset)
      e.knl = params.knl;
    if(params.kslset)
      e.ksl = params.ksl;
    if(params.k0set) {
      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k1set) {
      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k2set) {
      if(VERBOSE)
	printf("Warning: k2 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k3set) {
      if(VERBOSE)
	printf("Warning: k3 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.tiltset) {
      e.tilt = params.tilt;
    }
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _SOLENOID:
    e.type = _SOLENOID;
    e.l = params.l;
    e.ks = params.ks;
    e.B = params.B;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _ECOL:
    e.type = _ECOL;
    e.l = params.l;
    e.material = std::string(params.material);
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    
    break;

  case _MUSPOILER:
    e.type = _MUSPOILER;
    e.l = params.l;
    e.B = params.B;
    e.outR = params.outR;
    e.inR = params.inR;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _RCOL:
    e.type = _RCOL;
    e.l = params.l;
    e.material = std::string(params.material);
    e.flatlength = params.flatlength;
    e.taperlength = params.taperlength;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _LASER:
    e.type = _LASER;
    e.l = params.l;
    e.xdir = params.xdir;
    e.ydir = params.ydir;
    e.zdir = params.zdir;
    e.waveLength = params.waveLength;
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _ELEMENT:
    e.type = _ELEMENT;
    e.l = params.l;
    e.geometryFile = std::string(params.geometry);
    e.bmapFile = std::string(params.bmap);
    if(params.blmLocZset)
      e.blmLocZ = params.blmLocZ;
    if(params.blmLocThetaset)
      e.blmLocTheta = params.blmLocTheta;
    break;

  case _LINE:
    e.lst = lst;
    e.type = _LINE;
    break;

  case _REV_LINE:
    e.lst = lst;
    e.type = _REV_LINE;
    break;

  case _SAMPLER:
    e.type = _SAMPLER;
    break;
    
  case _TRANSFORM3D:
    e.type = _TRANSFORM3D;
    e.xdir = params.xdir;
    e.ydir = params.ydir;
    e.zdir = params.zdir;
    e.theta = params.theta;
    e.phi = params.phi;
    e.psi = params.psi;
    break;

  case _MATERIAL:
    e.type = _MATERIAL;
    e.A = params.A;
    e.Z = params.Z;
    e.density = params.density;
    e.temper = params.temper;
    e.pressure = params.pressure;
    e.state = params.state;
    e.components = params.components;
    e.componentsWeights = params.componentsWeights;
    e.componentsFractions = params.componentsFractions;
    material_list.push_back(e);
    return 0;

  case _ATOM:
    e.type = _ATOM;
    e.A = params.A;
    e.Z = params.Z;
    e.symbol = params.symbol;
    atom_list.push_back(e);
    return 0;

  case _TUNNEL:
    e.type = _TUNNEL;
    e.l = -1;
    e.geometryFile = std::string(params.geometry);
    break;

  case _SCREEN:
    e.type = _SCREEN;
    e.l = params.l;
    e.tscint = params.tscint;
    break;


  default:
    break;  


  }

  element_list.push_back(e);

  return 0;

}

int expand_line(const char *name, const char *start, const char* end)
{
  std::list<struct Element>::const_iterator iterNULL = element_list.end(); //bugfix for gcc 4.1.2 - cannot compare iterator to int(NULL). SPM
  std::list<struct Element>::iterator it;
  
  struct Element e;
  
  it = element_lookup(name);
  
//  if( (it!=NULL) && ((*it).type == _LINE || (*it).type == _REV_LINE) ) 
  if((*it).type == _LINE || (*it).type == _REV_LINE ) 

    {
      
      // delete the previous beamline
      
      beamline_list.clear();
      
      // expand the desired beamline
      
      e.type = (*it).type;
      e.name = name;
      e.l = 0;
      e.lst = NULL;
      
      beamline_list.push_back(e);
      
      if(VERBOSE) printf("expanding line %s, range = %s/%s\n",name,start,end);
      
      if(!(*it).lst) return 0; //list empty

      
      // first expand the whole range 
      std::list<struct Element>::iterator sit = (*it).lst->begin();
      std::list<struct Element>::iterator eit = (*it).lst->end();

      // copy the list into the resulting list
      switch((*it).type){
	case _LINE:
          beamline_list.insert(beamline_list.end(),(*it).lst->begin(),(*it).lst->end());
          break;
	case _REV_LINE:
          beamline_list.insert(beamline_list.end(),(*it).lst->rbegin(),(*it).lst->rend());
	  break;
	default:
          beamline_list.insert(beamline_list.end(),(*it).lst->begin(),(*it).lst->end());
	}
      bool is_expanded = false;
      
      // insert material entries.
      // TODO:::


      // parse starting from the second element until the list is expanded
      int iteration = 0;
      while(!is_expanded)
        {
	  is_expanded = true;
	  for(it = ++beamline_list.begin();it!=beamline_list.end();it++ )
	    {
#ifdef DEBUG 
              printf("%s , %s \n",(*it).name,typestr((*it).type));
#endif
	      if((*it).type == _LINE || (*it).type == _REV_LINE)  // list - expand further	  
		{
		  is_expanded = false;
		  // lookup the line in main list
		  std::list<struct Element>::iterator tmpit = element_lookup((*it).name);

		  if( (tmpit != iterNULL) && ( (*tmpit).lst != NULL) ) { // sublist found and not empty
		    
#ifdef DEBUG
                    printf("inserting sequence for %s - %s ...",(*it).name,(*tmpit).name);
#endif
if((*it).type == _LINE)
		      beamline_list.insert(it,(*tmpit).lst->begin(),(*tmpit).lst->end());
		    else if((*it).type == _REV_LINE){
		      //iterate over list and invert any sublines contained within. SPM
		      std::list<struct Element> tmpList;
		      tmpList.insert(tmpList.end(),(*tmpit).lst->begin(),(*tmpit).lst->end());
		      for(std::list<struct Element>::iterator 
			itLineInverter = tmpList.begin();
			itLineInverter != tmpList.end(); itLineInverter++){
			  if((*itLineInverter).type == _LINE ||
			    (*itLineInverter).type == _REV_LINE)
			      (*itLineInverter).type *= -1;}
		      beamline_list.insert(it,tmpList.rbegin(),tmpList.rend());
		    }
#ifdef DEBUG 
 printf("inserted\n");
#endif
		    
		    // delete the list pointer
		    beamline_list.erase(it--);
		    
		  } else if ( tmpit != iterNULL ) // entry points to a scalar element type -
		    //transfer properties from the main list
		    { 
#ifdef DEBUG 
                      printf("keeping element...%s\n",(*it).name);
#endif
		      copy_properties(it,tmpit);
#ifdef DEBUG 
                      printf("done\n");
#endif

		    } else  // element of undefined type - neglecting
		      {
			if(VERBOSE)
			  printf("Warning : Expanding line %s : element %s has not been \
                               defined , skipping \n",name,(*it).name);
			beamline_list.erase(it--);
		      }
		  
		} else  // element - keep as it is 
		  {
		    // do nothing
		  }
	      
	    }
	  iteration++;
	  if( iteration > MAX_EXPAND_ITERATIONS )
	    {
	      printf("Error : Line expansion of '%s' seems to loop, \
                     \n possible recursive line definition,quitting \n",name);
	      exit(0);
	    }
	  
	}// while
      
      
      // leave only the desired range
      //
      // rule - from first occurence of 'start' till first 'end' coming after 'start'


      if( (start!=NULL)) // determine the start element
	{
	  sit = element_lookup(start,beamline_list);
	  
	  if(sit==iterNULL)
	    {
	      sit = beamline_list.begin();
	    }
	  
	  if(!strcmp(start,"#s")) sit = beamline_list.begin(); 
	  
	  beamline_list.erase(beamline_list.begin(),sit);

	}

      if( (end!=NULL)) // determine the end element
	{
	  eit = element_lookup(end,beamline_list);
	  
	  if(eit==iterNULL)
	    {
	      eit = beamline_list.end();
	    }
	  
	  if(!strcmp(end,"#e")) eit = beamline_list.end();	  


	  beamline_list.erase(++eit,beamline_list.end());
	}


      // insert the tunnel if present

      it = element_lookup("tunnel");
      if(it!=iterNULL)
	beamline_list.push_back(*it);
      
      return 0;
    }
  
  
  printf("line '%s' not found",name);
  return 1;
  
}

std::list<struct Element>::iterator element_lookup(const char *name)
{
   std::list<struct Element>::iterator it;

   for(it=element_list.begin();it!=element_list.end();it++)
     {
       if(!strcmp((*it).name,name) )
	 return it;
     }
   return element_list.end();
}

std::list<struct Element>::iterator element_lookup(const char *name,std::list<struct Element>& el)
{
   std::list<struct Element>::iterator it;

   for(it=el.begin();it!=el.end();it++)
     {
       if(!strcmp((*it).name,name) )
	 return it;
     }
   return element_list.end();
}


// insert a sampler into beamline_list
void add_sampler(char *name, char *before, int before_count)
{
#ifdef DEBUG 
  std::cout<<"inserting sampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif
  std::list<struct Element>::iterator it;

  int element_count = 1;  // count from 1 like in goddam FORTRAN -- for range parsing
  struct Element e;
  e.type = _SAMPLER;
  e.name = name;
  e.lst = NULL;

  for(it = beamline_list.begin();it != beamline_list.end(); ++it)
    {
#ifdef DEBUG 
      std::cout<<"-->"<<(*it).name<<std::endl;
#endif

      if( !strcmp((*it).name, before)) 
	{

	  if( before_count == element_count)
	    {
	      beamline_list.insert(it,e);
	      return;
	    }

	  element_count++;
	}

    }

  std::cout<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;

}

// insert a cylindrical sampler into beamline_list
void add_csampler(char *name, char *before, int before_count, double length, double rad)
{
#ifdef DEBUG 
  std::cout<<"inserting sampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  std::list<struct Element>::iterator it;

  int element_count = 1;  // count from 1 like in goddam FORTRAN -- for range parsing
  struct Element e;
  e.type = _CSAMPLER;
  e.l = length;
  e.r = rad;
  e.name = name;
  e.lst = NULL;

  for(it = beamline_list.begin();it != beamline_list.end(); ++it)
    {
#ifdef DEBUG 
      std::cout<<"-->"<<(*it).name<<std::endl;
#endif

      if( !strcmp((*it).name, before)) 
	{

	  if( before_count == element_count)
	    {
	      beamline_list.insert(it,e);
	      return;
	    }

	  element_count++;
	}

    }

  std::cout<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;

}

// insert a beam dumper into beamline_list
void add_dump(char *name, char *before, int before_count)
{
#ifdef DEBUG 
  std::cout<<"inserting dump before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  std::list<struct Element>::iterator it;

  int element_count = 1;  // count from 1 like in goddam FORTRAN -- for range parsing
  struct Element e;
  e.type = _DUMP;
  e.name = name;
  e.lst = NULL;

  for(it = beamline_list.begin();it != beamline_list.end(); ++it)
    {
#ifdef DEBUG 
      std::cout<<"-->"<<(*it).name<<std::endl;
#endif

      if( !strcmp((*it).name, before))
        {

          if( before_count == element_count)
            {
              beamline_list.insert(it,e);
              return;
            }


          element_count++;
        }

    }

  std::cout<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;

}

// insert beam gas                                             
void add_gas(const char *name, const char *before, int before_count,  const char* material)
{
  printf("gas %s will be inserted into %s number %d\n",material,before,before_count);
  struct Element e;
  e.type = _GAS;
  e.name = name;
  e.lst = NULL;
  element_list.insert(beamline_list.end(),e);
 
}


void print(std::list<struct Element> l, int ident)
{

  if(VERBOSE) if(ident == 0) printf("using line %s\n",current_line);

  std::list<struct Element>::iterator it;
  std::list<double>::iterator it2;

  for(it=l.begin();it!=l.end();it++)
    {
      for(int i=0;i<ident;i++)
	printf("--");

      printf("->%s : %s",(*it).name,typestr((*it).type));

      switch((*it).type) {
      case _DRIFT:
      case _PCLDRIFT:
      case _SBEND:
      case _RBEND:
      case _QUAD:
      case _SEXTUPOLE:
      case _OCTUPOLE:
	printf(", l=%.10g, k0=%.10g, k1=%.10g, k2=%.10g, k3=%.10g, angle=%.10g,tilt=%.10g ",
	       (*it).l,(*it).k0,(*it).k1,(*it).k2,(*it).k3,(*it).angle,(*it).tilt);
	break;

      case _SOLENOID:
	printf(", l=%.10g, ks=%.10g ", (*it).l, (*it).ks);
	break;

      case _MULT:
	printf(" , knl={");
	for(it2=(*it).knl.begin();it2!=(*it).knl.end();it2++)
	  printf("%.10g, ",(*it2));
	printf("},  ksl={");
	for(it2=(*it).ksl.begin();it2!=(*it).ksl.end();it2++)
	  printf("%.10g, ",(*it2));
	printf("}");
	break;

      case _ELEMENT:
	printf("\ngeometry file : %s\n",(*it).geometryFile.c_str());
	printf("B map file : %s\n",(*it).bmapFile.c_str());
	//printf("E map driver : %s\n",(*it).geometryFile);
	//printf("E map file : %s\n",(*it).geometryFile);
	break;

      case _SCREEN:
	break;

      case _CSAMPLER:
	printf(" length=%.10g, radius=%.10g",(*it).l, (*it).r);
	break;

      case _TRANSFORM3D:
	printf(" xdir=%.10g, ydir=%.10g, zdir=%.10g,  phi=%.10g, theta=%.10g,psi=%.10g",
	       (*it).xdir, (*it).ydir, (*it).zdir, (*it).phi, (*it).theta, (*it).psi);
	break;
      case _MATERIAL:
	printf(" A=%.10g, Z=%.10g, density=%.10g,  temper=%.10g, pressure=%.10g",
	       (*it).A, (*it).Z, (*it).density, (*it).temper, (*it).pressure);
	break;
      default:
	break;
      }

      printf("\n");

      if((*it).lst != NULL)
	{
	  print(*(*it).lst,++ident);
	  ident--;
	}
    }
}

void print(struct Options opt)
{
  std::cout<<"Options : "<<std::endl;
  std::cout<<"particle : "<<opt.particleName<<std::endl;
  std::cout<<"energy : "<<opt.beamEnergy<<std::endl;
  std::cout<<"n macroparticles : "<<opt.numberToGenerate<<std::endl;
  std::cout<<"sigmaX           : "<<opt.sigmaX<<std::endl;
  std::cout<<"Cerenkov on               : "<<opt.turnOnCerenkov<<std::endl;
}


void set_value(std::string name, double value )
{
#ifdef DEBUG
  std::cout << "parser.h> Setting value " << name << ", " << value << std::endl; 
#endif
  //
  // numeric options for the "beam" command
  //
  if(name == "elossHistoBinWidth") {options.elossHistoBinWidth = value; return;}
  if(name == "elossHistotransBinWidth") {options.elossHistoTransBinWidth = value; return;}
  if(name == "defaultRangeCut") {options.defaultRangeCut = value; return;}
  if(name == "ffact") {options.ffact = value; return;}
  if(name == "energy" ) {options.beamEnergy = value; return;}
  if(name == "X0" ) { options.X0 = value; return; }
  if(name == "Y0" ) { options.Y0 = value; return; }
  if(name == "Z0" ) { options.Z0 = value; return; }
  if(name == "T0" ) { options.T0 = value; return; }
  if(name == "Xp0" ) { options.Xp0 = value; return; }
  if(name == "Yp0" ) { options.Yp0 = value; return; }
  if(name == "Zp0" ) { options.Zp0 = value; return; }

  if(name == "sigmaT" ) { options.sigmaT = value; return; }
  if(name == "sigmaE" ) { options.sigmaE = value; return; }

  // options for beam distrType="gauss"
  if(name == "sigmaX" ) { options.sigmaX = value; return; }
  if(name == "sigmaY" ) { options.sigmaY = value; return; }
  if(name == "sigmaXp" ) { options.sigmaXp = value; return; }
  if(name == "sigmaYp" ) { options.sigmaYp = value; return; }

  // options for beam distrType="square" or distrType="circle"
  if(name == "envelopeX" ) { options.envelopeX = value; return; }
  if(name == "envelopeY" ) { options.envelopeY = value; return; }
  if(name == "envelopeXp" ) { options.envelopeXp = value; return; }
  if(name == "envelopeYp" ) { options.envelopeYp = value; return; }
  if(name == "envelopeT" ) { options.envelopeT = value; return; }
  if(name == "envelopeE" ) { options.envelopeE = value; return; }

  // options for beam distrType="gaussmatrix"
  if(name == "sigma11" ) { options.sigma11 = value; return; }
  if(name == "sigma12" ) { options.sigma12 = value; return; }
  if(name == "sigma13" ) { options.sigma13 = value; return; }
  if(name == "sigma14" ) { options.sigma14 = value; return; }
  if(name == "sigma15" ) { options.sigma15 = value; return; }
  if(name == "sigma16" ) { options.sigma16 = value; return; }

  if(name == "sigma22" ) { options.sigma22 = value; return; }
  if(name == "sigma23" ) { options.sigma23 = value; return; }
  if(name == "sigma24" ) { options.sigma24 = value; return; }
  if(name == "sigma25" ) { options.sigma25 = value; return; }
  if(name == "sigma26" ) { options.sigma26 = value; return; }

  if(name == "sigma33" ) { options.sigma33 = value; return; }
  if(name == "sigma34" ) { options.sigma34 = value; return; }
  if(name == "sigma35" ) { options.sigma35 = value; return; }
  if(name == "sigma36" ) { options.sigma36 = value; return; }

  if(name == "sigma44" ) { options.sigma44 = value; return; }
  if(name == "sigma45" ) { options.sigma45 = value; return; }
  if(name == "sigma46" ) { options.sigma46 = value; return; }

  if(name == "sigma55" ) { options.sigma55 = value; return; }
  if(name == "sigma56" ) { options.sigma56 = value; return; }

  if(name == "sigma66" ) { options.sigma66 = value; return; }
    
  // options for beam distrType="eshell"
  if(name == "shellX" ) { options.shellX = value; return; }
  if(name == "shellY" ) { options.shellY = value; return; }
  if(name == "shellXp" ) { options.shellXp = value; return; }
  if(name == "shellYp" ) { options.shellYp = value; return; }

  // options for beam distrType="ring"
  if(name == "Rmin" ) { options.Rmin = value; return; }
  if(name == "Rmax" ) { options.Rmax = value; return; }

  //
  // numeric options for the"option" command
  //

  // options for beam loss monitor geometry
  if(name == "blmRad" ) { options.blmRad = value; return; }
  if(name == "blmLength" ) { options.blmLength = value; return; }

  // options which influence the geometry
  if(name == "boxSize" ) {options.componentBoxSize = value; return; }
  if(name == "tunnelRadius" ) { options.tunnelRadius = value; return; }
  if(name == "beampipeThickness" ) { options.beampipeThickness = value; return; }
  if(name == "beampipeRadius" ) { options.beampipeRadius = value; return; }

  if(name == "includeIronMagFields") {
    options.includeIronMagFields = (int)value; return;
  } 

  if(name == "buildTunnel") {
      options.buildTunnel = (int)value; return;
  }

  if(name == "buildTunnelFloor") {
    options.buildTunnelFloor = (int)value; return;
  }
  
  if(name == "showTunnel") {
    options.showTunnel = (int)value; return;
  }

  if(name == "synchRadOn") { 
    options.synchRadOn=(int)value;
    return; 
  }

  if(name == "decayOn") { 
    options.decayOn = (int)value; return; 
  }
  
  if(name == "tunnelOffsetX" ) { options.tunnelOffsetX = value; return; }
  if(name == "tunnelOffsetY" ) { options.tunnelOffsetY = value; return; }
  if(name == "samplerDiameter" ) { options.samplerDiameter = value; return; }
  if(name == "tunnelThickness" ) { options.tunnelThickness = value; return; }
  if(name == "tunnelSoilThickness" ) { options.tunnelSoilThickness = value; return; }
  if(name == "tunnelFloorOffset" ) { options.tunnelFloorOffset = value; return; }

  //geometry biasing
  if(name == "geometryBias") {
    options.geometryBias = (int)value; return;
  }


  // options which influence tracking 
  if(name == "maximumTrackingTime") {options.maximumTrackingTime = value; return;}
  if(name == "deltaChord") { options.deltaChord = value; return; }
  if(name == "deltaIntersection") { options.deltaIntersection = value; return; }
  if(name == "chordStepMinimum") { options.chordStepMinimum = value; return; }
  if(name == "lengthSafety") { options.lengthSafety = value; return; }
  if(name == "minimumEpsilonStep" ) { options.minimumEpsilonStep = value; return; }
  if(name == "maximumEpsilonStep" ) { options.maximumEpsilonStep = value; return; }
  if(name == "deltaOneStep" ) { options.deltaOneStep = value; return; }

  // physics processes
  if(name == "turnOnCerenkov") {
      options.turnOnCerenkov = (int)value; return;
  }

  if(name == "srRescale") {
      options.synchRescale = (int)value; return;
  }

  if(name == "srTrackPhotons") {
    options.synchTrackPhotons = (int)value; return;
  }

  if(name == "useEMLPB") { options.useEMLPB = value; return; }
  if(name == "useHadLPB") { options.useHadLPB = value; return; }
  if(name == "sensitiveBeamlineComponents") { options.sensitiveBeamlineComponents = value; return; }
  if(name == "sensitiveBeamPipe") { options.sensitiveBeamPipe = value; return; }
  if(name == "sensitiveBLMs") { options.sensitiveBLMs = value; return; }
  if(name == "LPBFraction") { options.LPBFraction = value; return; }
  if(name == "annihiToMuFe") { options.annihiToMuFe = value; return; }
  if(name == "gammaToMuFe") { options.gammaToMuFe = value; return; }
  if(name == "eeToHadronsFe") { options.eeToHadronsFe = value; return; }
  if(name == "thresholdCutCharged" ) { options.thresholdCutCharged = (double)value; return; }
  if(name == "thresholdCutPhotons" ) { options.thresholdCutPhotons = (double)value; return; }
  if(name == "vacuumPressure") { options.vacuumPressure = (double)value; return; }
  if(name == "planckScatterFe") { options.planckScatterFe = (double)value; return; }
  if(name == "stopTracks") { options.stopTracks = (int) value; return; } 
  if(name == "srLowX") { options.synchLowX = value; return; }
  if(name == "srLowGamE") { options.synchLowGamE = value; return; }
  if(name == "srMultiplicity") { options.synchPhotonMultiplicity = (int) value; return; }
  if(name == "srMeamFreeFactor") { options.synchMeanFreeFactor = (int) value; return; }

  if(name == "prodCutPhotons" ) { options.prodCutPhotons = value; return; }
  if(name == "prodCutPhotonsP" ) { options.prodCutPhotonsP = value; return; }
  if(name == "prodCutPhotonsA" ) { options.prodCutPhotonsA = value; return; }
  if(name == "prodCutElectrons" ) { options.prodCutElectrons = value; return; }
  if(name == "prodCutElectronsP" ) { options.prodCutElectronsP = value; return; }
  if(name == "prodCutElectronsA" ) { options.prodCutElectronsA = value; return; }
  if(name == "prodCutPositrons" ) { options.prodCutPositrons = value; return; }
  if(name == "prodCutPositronsP" ) { options.prodCutPositronsP = value; return; }
  if(name == "prodCutPositronsA" ) { options.prodCutPositronsA = value; return; }

  // twiss parameters
  if(name == "betx" ) { options.betx = value; return; }
  if(name == "bety" ) { options.bety = value; return; }
  if(name == "alfx" ) { options.alfx = value; return; }
  if(name == "alfy" ) { options.alfy = value; return; }
  if(name == "emitx" ) { options.emitx = value; return; }
  if(name == "emity" ) { options.emity = value; return; }
  if(name == "doTwiss" ) { options.doTwiss = (int) value; return; }
  if(name == "doPlanckScattering" ) { options.doPlanckScattering = (int) value; return; }
  if(name == "checkOverlaps" ) { options.checkOverlaps = (int) value; return; }

  if(name == "storeTrajectory") { options.storeTrajectory = (int) value; return; } 
  if(name == "storeTrajectories") { options.storeTrajectory = (int) value; return; } 
  if(name == "storeMuonTrajectory") { options.storeMuonTrajectories = (int) value; return; } 
  if(name == "storeMuonTrajectories") { options.storeMuonTrajectories = (int) value; return; } 
  if(name == "trajCutGTZ") { options.trajCutGTZ = (double) value; return; } 
  if(name == "trajCutLTR") { options.trajCutLTR = (double) value; return; } 

  if(name == "storeNeutronTrajectory") { options.storeNeutronTrajectories = (int) value; return; } 
  if(name == "storeNeutronTrajectories") { options.storeNeutronTrajectories = (int) value; return; } 


  // options for generation and storage
  if(name == "randomSeed") { options.randomSeed = (int) value; return; }
  if(name == "ngenerate" ) { options.numberToGenerate = (int)value; return; }
  if(name == "nperfile" ) { options.numberOfEventsPerNtuple = (int)value; return; }
  if(name == "eventNumberOffset" ) { options.eventNumberOffset = (int)value; return; }
  if(name == "nlinesIgnore") { options.nlinesIgnore = (int) value; return; }

  // options for neutrons
  if(name=="refcopyno") { options.refcopyno = (int) value; return; }
  std::cerr << "Error: parser.h> unkown option \"" << name << "\"" << std::endl; 
  exit(1);
}


void set_value(std::string name, std::string value )
{
#ifdef DEBUG
  std::cout << "parser.h> Setting value " << name << ", " << value << std::endl; 
#endif
  // 
  // string options for the "beam" command
  //
  if(name == "particle") { options.particleName = value; return; }
  if(name == "distrType" ) { options.distribType = value; return; }
  if(name == "distrFile" ) { options.distribFile = sBDSIMPATH+value; return; }  

  //
  // string options for the "option" command
  //

  // options which influence the geometry
  if(name == "beampipeMaterial" ) { options.pipeMaterial = value; return; }
  if(name == "vacMaterial" ) { options.vacMaterial = value; return; }
  if(name == "tunnelMaterial" ) { options.tunnelMaterial = value; return; }
  if(name == "tunnelCavityMaterial" ) { options.tunnelCavityMaterial = value; return; }
  if(name == "soilMaterial" ) { options.soilMaterial = value; return; }
  
  // options which influence the tracking
  if(name == "physicsList" ) { options.physicsList = value; return; } 

  // options for external code interfaces
  if(name == "fifo") { options.fifo = value; return; }
  if(name == "refvolume") { options.refvolume = value; return; }
  std::cerr << "Error: parser.h> unkown option \"" << name << "\"" << std::endl; 
  exit(1);
}

double property_lookup(char *element_name, char *property_name)
{
   std::list<struct Element>::iterator it = element_lookup(element_name);
   std::list<struct Element>::const_iterator iterNULL = element_list.end();

   if(it == iterNULL) return 0;
   
   if(!strcmp(property_name,"l")) return (*it).l;
   if(!strcmp(property_name,"B")) return (*it).B;
   if(!strcmp(property_name,"ks")) return (*it).ks;
   if(!strcmp(property_name,"k0")) return (*it).k0;
   if(!strcmp(property_name,"k1")) return (*it).k1;
   if(!strcmp(property_name,"k2")) return (*it).k2;
   if(!strcmp(property_name,"k3")) return (*it).k3;
   if(!strcmp(property_name,"angle")) return (*it).angle;
   if(!strcmp(property_name,"phiAngleIn")) return (*it).phiAngleIn;
   if(!strcmp(property_name,"phiAngleOut")) return (*it).phiAngleOut;
   if(!strcmp(property_name,"beampipeThickness")) return (*it).beampipeThickness;
   if(!strcmp(property_name,"aper")) return (*it).aper;
   if(!strcmp(property_name,"aperX")) return (*it).aperX;
   if(!strcmp(property_name,"aperY")) return (*it).aperY;
   if(!strcmp(property_name,"aperYUp")) return (*it).aperYUp;
   if(!strcmp(property_name,"aperYDown")) return (*it).aperYDown;
   if(!strcmp(property_name,"aperDy")) return (*it).aperDy;
   if(!strcmp(property_name,"outR")) return (*it).outR;
   if(!strcmp(property_name,"inR")) return (*it).inR;
   if(!strcmp(property_name,"bpRad")) return (*it).bpRad;
   if(!strcmp(property_name,"xsize")) return (*it).xsize;
   if(!strcmp(property_name,"ysize")) return (*it).ysize;
   if(!strcmp(property_name,"xdir")) return (*it).xdir;
   if(!strcmp(property_name,"ydir")) return (*it).ydir;
   if(!strcmp(property_name,"zdir")) return (*it).zdir;
   if(!strcmp(property_name,"phi")) return (*it).phi;
   if(!strcmp(property_name,"psi")) return (*it).psi;
   if(!strcmp(property_name,"theta")) return (*it).theta;
   if(!strcmp(property_name,"waveLength")) return (*it).waveLength;
   if(!strcmp(property_name,"tilt")) return (*it).tilt;
   if(!strcmp(property_name,"gradient")) return (*it).gradient;
   if(!strcmp(property_name,"hgap")) return (*it).hgap;
   if(!strcmp(property_name,"flatlength")) return (*it).flatlength;
   if(!strcmp(property_name,"taperlength")) return (*it).taperlength;

   if(!strcmp(property_name,"A")) return (*it).A;
   if(!strcmp(property_name,"Z")) return (*it).Z;
   if(!strcmp(property_name,"density")) return (*it).density;
   if(!strcmp(property_name,"T")) return (*it).temper;
   if(!strcmp(property_name,"P")) return (*it).pressure;

   std::cerr << "parser.h> Error: unkown property \"" << property_name << "\". Returning 0." << std::endl; 
   exit(1);
   //what about property_lookup for attributes of type string, like material?
}

// ******************************************************
// parser functions
// ******************************************************


int add_func(const char *name, double (*func)(double))
{
  struct symtab *sp=symlook(name);
  sp->funcptr=func;
  return 0;
}

int add_var(const char *name, double value, int is_reserved = 0)
{
  struct symtab *sp=symlook(name);
  sp->value=value;
  sp->is_reserved = is_reserved;
  return 0;
}


#endif
