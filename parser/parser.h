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
#include <map>
#include <string>
#include <vector>

#include <iostream>

#include "element.h"
#include "elementlist.h"
#include "enums.h"
#include "gmad.h"
#include "options.h"
#include "parameters.h"

int yyerror(const char *);

extern FILE* yyin;
extern int yylex();


const int MAX_EXPAND_ITERATIONS = 50;
//const int MAX_MULTIPOLE_ORDER = 5;

//const int _undefined = 0;

std::list<double> _tmparray;  // for reading of arrays
std::list<char*> _tmpstring;

struct Parameters params;
struct Options options;
struct Element element;


// list of all encountered elements
ElementList element_list;

// temporary list
std::list<struct Element> tmp_list;

ElementList beamline_list;
std::list<struct Element> material_list;
std::list<struct Element> atom_list;

const char* current_line = "";
const char* current_start = "";
const char* current_end = "";

//struct symtab *symtab; 
std::map<std::string, struct symtab*> symtab_map;

extern struct symtab * symlook(const char *s);

// representation of arrays used in tokens
struct Array {
  std::vector<char*> symbols;
  double *data;
  int size;
};

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

int write_table(struct Parameters pars,const char* name, int type, std::list<struct Element> *lst=NULL);
int expand_line(const char *name, const char *start, const char *end);

// *********************
// functions
// *********************

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
  // common parameters for all elements
  e.name = std::string(name);
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
      //      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type QUADRUPOLE\n",name);
    }
    if(params.k1set) {
      e.k1 = params.k1;
    }
    if(params.k2set) {
      //      if(VERBOSE)
	printf("Warning: k2 will not be set for element %s of type QUADRUPOLE\n",name);
    }
    if(params.k3set) {
      //      if(VERBOSE)
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
      //      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type SEXTUPOLE\n",name);
    }
    if(params.k1set) {
      //      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type SEXTUPOLE\n",name);
    }
    if(params.k2set) {
      e.k2 = params.k2;
    }
    if(params.k3set) {
      //      if(VERBOSE)
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
      //      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type OCTUPOLE\n",name);
    }
    if(params.k1set) {
      //      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type OCTUPOLE\n",name);
    }
    if(params.k2set) {
      //      if(VERBOSE)
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
      //      if(VERBOSE)
	printf("Warning: k0 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k1set) {
      //      if(VERBOSE)
	printf("Warning: k1 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k2set) {
      //      if(VERBOSE)
	printf("Warning: k2 will not be set for element %s of type MULTIPOLE\n",name);
    }
    if(params.k3set) {
      //      if(VERBOSE)
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

int expand_line(const char *charname, const char *start, const char* end)
{
  std::list<struct Element>::const_iterator iterEnd = element_list.end();
  std::list<struct Element>::iterator it;
  
  struct Element e;
  std::string name = std::string(charname);
  it = element_list.find(name);

  if (it==element_list.end()) {
    printf("line '%s' not found\n",charname);
    return 1;
  }
  if((*it).type != _LINE && (*it).type != _REV_LINE ) {
    printf("'%s' is not a line\n",charname);
  }

  // delete the previous beamline
  
  beamline_list.clear();
  
  // expand the desired beamline
  
  e.type = (*it).type;
  e.name = name;
  e.l = 0;
  e.lst = NULL;
  
  beamline_list.push_back(e);
  
  if(VERBOSE) printf("expanding line %s, range = %s/%s\n",charname,start,end);
  
  if(!(*it).lst) return 0; //list empty
  
  
  // first expand the whole range 
  std::list<struct Element>::iterator sit = (*it).lst->begin();
  std::list<struct Element>::iterator eit = (*it).lst->end();
  
  // copy the list into the resulting list
  switch((*it).type){
  case _LINE:
    beamline_list.insert(beamline_list.end(),sit,eit);
    break;
  case _REV_LINE:
    beamline_list.insert(beamline_list.end(),(*it).lst->rbegin(),(*it).lst->rend());
    break;
  default:
    beamline_list.insert(beamline_list.end(),sit,eit);
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
	  printf("%s , %s \n",(*it).name.c_str(),typestr((*it).type));
#endif
	  if((*it).type == _LINE || (*it).type == _REV_LINE)  // list - expand further	  
	    {
	      is_expanded = false;
	      // lookup the line in main list
	      std::list<struct Element>::iterator tmpit = element_list.find((*it).name);
	      
	      if( (tmpit != iterEnd) && ( (*tmpit).lst != NULL) ) { // sublist found and not empty
		
#ifdef DEBUG
		printf("inserting sequence for %s - %s ...",(*it).name.c_str(),(*tmpit).name.c_str());
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
		
	      } else if ( tmpit != iterEnd ) // entry points to a scalar element type -
		//transfer properties from the main list
		{ 
#ifdef DEBUG 
		  printf("keeping element...%s\n",(*it).name.c_str());
#endif
		  // copy properties
		  //		  copy_properties(it,tmpit);
		  // better use default assign operator:
		  (*it) = (*tmpit);
#ifdef DEBUG 
		  printf("done\n");
#endif
		  
		} else  // element of undefined type - neglecting
		{
		  printf("Warning : Expanding line %s : element %s has not been defined , skipping \n",charname,(*it).name.c_str());
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
                     \n possible recursive line definition,quitting \n",charname);
	  exit(0);
	}
      
    }// while
  
  
  // leave only the desired range
  //
  // rule - from first occurence of 'start' till first 'end' coming after 'start'
  
  
  if( (start!=NULL)) // determine the start element
    {
      sit = beamline_list.find(std::string(start));
      
      if(sit==beamline_list.end())
	{
	  sit = beamline_list.begin();
	}
      
      if(!strcmp(start,"#s")) sit = beamline_list.begin(); 
      
      beamline_list.erase(beamline_list.begin(),sit);
      
    }
  
  if( (end!=NULL)) // determine the end element
    {
      eit = beamline_list.find(std::string(end));
      
      if(!strcmp(end,"#e")) eit = beamline_list.end();
      
      beamline_list.erase(++eit,beamline_list.end());
    }
  
  
  // insert the tunnel if present
  
  it = element_list.find("tunnel");
  if(it!=iterEnd)
    beamline_list.push_back(*it);
  
  return 0;
}

// insert a sampler into beamline_list
void add_sampler(char *name, char *before, int before_count)
{
#ifdef DEBUG 
  std::cout<<"inserting sampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = _SAMPLER;
  e.name = name;
  e.lst = NULL;

  std::list<struct Element>::iterator it = beamline_list.find(std::string(before),before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    return;
  }
  beamline_list.insert(it,e);
}

// insert a cylindrical sampler into beamline_list
void add_csampler(char *name, char *before, int before_count, double length, double rad)
{
#ifdef DEBUG 
  std::cout<<"inserting csampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = _CSAMPLER;
  e.l = length;
  e.r = rad;
  e.name = name;
  e.lst = NULL;

  std::list<struct Element>::iterator it = beamline_list.find(std::string(before),before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    return;
  }
  beamline_list.insert(it,e);
}

// insert a beam dumper into beamline_list
void add_dump(char *name, char *before, int before_count)
{
#ifdef DEBUG 
  std::cout<<"inserting dump before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = _DUMP;
  e.name = name;
  e.lst = NULL;

  std::list<struct Element>::iterator it = beamline_list.find(std::string(before),before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    return;
  }
  beamline_list.insert(it,e);
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

double property_lookup(ElementList& el_list, char *element_name, char *property_name)
{
   std::list<struct Element>::iterator it = el_list.find(std::string(element_name));
   std::list<struct Element>::const_iterator iterEnd = el_list.end();

   if(it == iterEnd) {
     std::cerr << "parser.h> Error: unknown element \"" << element_name << "\". Returning 0." << std::endl; 
     exit(1);
   }

   return (*it).property_lookup(property_name);
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
