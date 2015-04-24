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
#ifdef BDSDEBUG 
    std::cout << src->data[i] << " ";
#endif
  }
#ifdef BDSDEBUG 
  std::cout << std::endl;
#endif
}


void set_vector(std::list<char*>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back(src->symbols[i]);
#ifdef BDSDEBUG 
    std::cout << src->symbols[i] << " ";
#endif
  }
#ifdef BDSDEBUG 
  std::cout << std::endl;
#endif
}

void set_vector(std::list<const char*>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back(src->symbols[i]);
#ifdef BDSDEBUG 
    std::cout << src->symbols[i] << " ";
#endif
  }
#ifdef BDSDEBUG 
  std::cout << std::endl;
#endif
}

void set_vector(std::list<std::string>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back((std::string)src->symbols[i]);
#ifdef BDSDEBUG 
    std::cout << (std::string)src->symbols[i] << " ";
#endif
  }
#ifdef BDSDEBUG 
  std::cout << std::endl;
#endif
}


void set_vector(std::list<int>& dst, struct Array *src)
{
  for(int i=0; i< src->size;i++){
    dst.push_back((int)(src->data[i]));
#ifdef BDSDEBUG 
    std::cout << (int)(src->data[i]) << " ";
#endif
  }
#ifdef BDSDEBUG 
  std::cout << std::endl;
#endif
}

/// method that transfers parameters to element properties
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
#ifdef BDSDEBUG 
  printf("k1=%.10g, k2=%.10g, k3=%.10g, type=%d, lset = %d\n", params.k1, params.k2, params.k3, type, params.lset);
#endif
  struct Element e;
  
  e.type = type;
  // common parameters for all elements
  e.name = std::string(name);
  e.lst = NULL;
  e.l = params.l;

  //new aperture model
  e.aper1 = params.aper1;
  e.aper2 = params.aper2;
  e.aper3 = params.aper3;
  e.aper4 = params.aper4;
  e.apertureType = params.apertureType;
  e.beampipeMaterial = params.beampipeMaterial;

  //magnet geometry
  e.outerDiameter = params.outerDiameter;
  e.outerMaterial = params.outerMaterial;
  e.magnetGeometryType = params.magnetGeometryType;
  
  e.xsize = params.xsize;
  e.ysize = params.ysize;
  e.material = params.material;  
  e.tunnelMaterial = params.tunnelMaterial;
  e.tunnelRadius = params.tunnelRadius;
  e.tunnelOffsetX = params.tunnelOffsetX;
  e.precisionRegion = params.precisionRegion;
  // end of common parameters

  // specific parameters
  // JS: perhaps add a printout warning in case it is not used doesn't match the element; how to do this systematically?

  // for transform3ds, lasers and for tracker
  e.xdir = params.xdir;
  e.ydir = params.ydir;
  e.zdir = params.zdir;

  // BLM
  if(params.blmLocZset)
    e.blmLocZ = params.blmLocZ;
  if(params.blmLocThetaset)
    e.blmLocTheta = params.blmLocTheta;

  // Drift
  if(params.phiAngleInset)
    e.phiAngleIn = params.phiAngleIn;
  if(params.phiAngleOutset)
    e.phiAngleOut = params.phiAngleOut;

  // Drift, Drift
  if(params.beampipeThicknessset)
    e.beampipeThickness = params.beampipeThickness;
  // RF
  e.gradient = params.gradient;
  // SBend, RBend, (Awake)Screen
  e.angle = params.angle;
  // SBend, RBend, HKick, VKick, Quad
  e.k1 = params.k1;
  // SBend, RBend, HKick, VKick, Solenoid, MuSpoiler
  e.B = params.B;
  // SBend, RBend, HKick, VKick, Quad, Sext, Oct, Mult
  if(params.tiltset) e.tilt = params.tilt;
  // Quad
  e.spec = params.spec;
  // Sext
  if(params.k2set) {
    if (type==_SEXTUPOLE) e.k2 = params.k2;
    else {
      printf("Warning: k2 will not be set for element %s of type %d\n",name, type);
    }
  }
  // Octupole
  if(params.k3set) {
    if (type==_OCTUPOLE) e.k3 = params.k3;
    else {
      printf("Warning: k3 will not be set for element %s of type %d\n",name, type);
    }
  }
  // Multipole
  if(params.knlset)
    e.knl = params.knl;
  if(params.kslset)
    e.ksl = params.ksl;
  // Solenoid
  e.ks = params.ks;
  // Laser
  e.waveLength = params.waveLength;
  // Element, Tunnel
  e.geometryFile = params.geometry;
  // Element
  e.bmapFile = params.bmap;
  if(params.bmapZOffsetset)
    e.bmapZOffset = params.bmapZOffset;
  // Transform3D
  e.theta = params.theta;
  e.phi = params.phi;
  e.psi = params.psi;
  // (Awake) Screen
  e.tscint = params.tscint;
  e.scintmaterial = params.scintmaterial;
  // Screen
  e.airmaterial = params.airmaterial;
  // AwakeScreen
  e.twindow = params.twindow;
  e.windowmaterial = params.windowmaterial;

  // overwriting of other parameters or specific printing
  switch(type) {

  case _LINE:
  case _REV_LINE:
    e.lst = lst;
    break;

  case _MATERIAL:
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
    e.A = params.A;
    e.Z = params.Z;
    e.symbol = params.symbol;
    atom_list.push_back(e);
    return 0;

  case _TUNNEL:
    e.l = -1;
    break;

  case _AWAKESCREEN:
    std::cout << "scintmaterial: " << e.scintmaterial << " " <<  params.scintmaterial << std::endl;
    std::cout << "windowmaterial: " << e.windowmaterial << " " <<  params.windowmaterial << std::endl;
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
#ifdef BDSDEBUG 
	  printf("%s , %s \n",(*it).name.c_str(),typestr((*it).type));
#endif
	  if((*it).type == _LINE || (*it).type == _REV_LINE)  // list - expand further	  
	    {
	      is_expanded = false;
	      // lookup the line in main list
	      std::list<struct Element>::iterator tmpit = element_list.find((*it).name);
	      
	      if( (tmpit != iterEnd) && ( (*tmpit).lst != NULL) ) { // sublist found and not empty
		
#ifdef BDSDEBUG
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
#ifdef BDSDEBUG
		printf("inserted\n");
#endif
		
		// delete the list pointer
		beamline_list.erase(it--);
		
	      } else if ( tmpit != iterEnd ) // entry points to a scalar element type -
		//transfer properties from the main list
		{ 
#ifdef BDSDEBUG 
		  printf("keeping element...%s\n",(*it).name.c_str());
#endif
		  // copy properties
		  //		  copy_properties(it,tmpit);
		  // better use default assign operator:
		  (*it) = (*tmpit);
#ifdef BDSDEBUG 
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
#ifdef BDSDEBUG 
  std::cout<<"inserting sampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = _SAMPLER;
  e.name = name;
  e.lst = NULL;

  std::list<struct Element>::iterator it = beamline_list.find(std::string(before),before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    exit(1);
  }
  beamline_list.insert(it,e);
}

// insert a cylindrical sampler into beamline_list
void add_csampler(char *name, char *before, int before_count, double length, double rad)
{
#ifdef BDSDEBUG 
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
    exit(1);
  }
  beamline_list.insert(it,e);
}

// insert a beam dumper into beamline_list
void add_dump(char *name, char *before, int before_count)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting dump before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = _DUMP;
  e.name = name;
  e.lst = NULL;

  std::list<struct Element>::iterator it = beamline_list.find(std::string(before),before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    exit(1);
  }
  beamline_list.insert(it,e);
}

// insert beam gas                                             
void add_gas(const char *name, const char *before, int before_count, std::string material)
{
  printf("gas %s will be inserted into %s number %d\n",material.c_str(),before,before_count);
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
