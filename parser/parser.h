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
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "element.h"
#include "fastlist.h"
#include "elementtype.h"
#include "gmad.h"
#include "options.h"
#include "parameters.h"
#include "physicsbiasing.h"
#include "tunnel.h"

int yyerror(const char *);

extern FILE* yyin;
extern int yylex();

namespace GMAD {

extern const int ECHO_GRAMMAR;

const int MAX_EXPAND_ITERATIONS = 50;

std::list<double> _tmparray;  // for reading of arrays
std::list<std::string> _tmpstring;

/// globals
struct Parameters params;
struct Options options;
//struct Element element;
struct Tunnel tunnel;
class PhysicsBiasing xsecbias;
 
// list of all encountered elements
FastList<Element> element_list;

// temporary list
std::list<struct Element> tmp_list;

FastList<Element> beamline_list;
std::list<struct Element>  material_list;
std::list<struct Element>  atom_list;
std::vector<struct Tunnel> tunnel_list;
FastList<PhysicsBiasing> xsecbias_list;

std::string current_line;
std::string current_start;
std::string current_end;

//struct symtab *symtab; 
std::map<std::string, struct symtab*> symtab_map;

extern struct symtab * symlook(std::string s);

// ***********************
// functions declaration *
// ***********************

void quit();
/// method that transfers parameters to element properties
int write_table(const struct Parameters& pars,std::string name, ElementType type, std::list<struct Element> *lst=nullptr);
int expand_line(std::string name, std::string start, std::string end);
/// insert a sampler into beamline_list
void add_sampler(std::string name, std::string before, int before_count);
/// insert a cylindrical sampler into beamline_list
void add_csampler(std::string name, std::string before, int before_count, double length, double rad);
/// insert a beam dumper into beamline_list
void add_dump(std::string name, std::string before, int before_count);
/// insert tunnel
void add_tunnel(Tunnel& tunnel);
/// insert xsecbias
void add_xsecbias(PhysicsBiasing& xsecbias);
double property_lookup(FastList<Element>& el_list, std::string element_name, std::string property_name);
/// add element to temporary element sequence tmp_list
void add_element_temp(std::string name, int number, bool pushfront, ElementType linetype);

// parser functions
int add_func(std::string name, double (*func)(double));
int add_var(std::string name, double value, int is_reserved = 0);

// **************************
// functions implementation *
// **************************

void quit()
{
  printf("parsing complete...\n");
  exit(0);
}

int write_table(const struct Parameters& params,std::string name, ElementType type, std::list<struct Element> *lst)
{
  if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << name << " : " << type << std::endl;
#ifdef BDSDEBUG 
  printf("k1=%.10g, k2=%.10g, k3=%.10g, type=%s, lset = %d\n", params.k1, params.k2, params.k3, typestr(type).c_str(), params.lset);
#endif

  struct Element e;
  e.set(params,name,type,lst);

  switch(type) {

  case ElementType::_MATERIAL:
    material_list.push_back(e);
    return 0;

  case ElementType::_ATOM:
   atom_list.push_back(e);
   return 0;

  default:
    break;
  }
  
  // insert element with uniqueness requirement
  element_list.push_back(e,true);

  return 0;
}

int expand_line(std::string name, std::string start, std::string end)
{
  std::list<struct Element>::const_iterator iterEnd = element_list.end();
  std::list<struct Element>::iterator it;
  
  struct Element e;
  it = element_list.find(name);

  if (it==iterEnd) {
    std::cout << "line '" << name << "' not found" << std::endl;
    return 1;
  }
  if((*it).type != ElementType::_LINE && (*it).type != ElementType::_REV_LINE ) {
    std::cout << "'" << name << "' is not a line" << std::endl;
  }

  // delete the previous beamline
  
  beamline_list.clear();
  
  // expand the desired beamline
  
  e.type = (*it).type;
  e.name = name;
  e.l = 0;
  e.lst = nullptr;
  
  beamline_list.push_back(e);

#ifdef BDSDEBUG 
  std::cout << "expanding line " << name << ", range = " << start << end << std::endl;
#endif
  if(!(*it).lst) return 0; //list empty
    
  // first expand the whole range 
  std::list<struct Element>::iterator sit = (*it).lst->begin();
  std::list<struct Element>::iterator eit = (*it).lst->end();
  
  // copy the list into the resulting list
  switch((*it).type){
  case ElementType::_LINE:
    beamline_list.insert(beamline_list.end(),sit,eit);
    break;
  case ElementType::_REV_LINE:
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
	  std::cout << (*it).name << " , " << (*it).type << std::endl;
#endif
	  if((*it).type == ElementType::_LINE || (*it).type == ElementType::_REV_LINE)  // list - expand further	  
	    {
	      is_expanded = false;
	      // lookup the line in main list
	      std::list<struct Element>::iterator tmpit = element_list.find((*it).name);
	      
	      if( (tmpit != iterEnd) && ( (*tmpit).lst != nullptr) ) { // sublist found and not empty
		
#ifdef BDSDEBUG
		printf("inserting sequence for %s - %s ...",(*it).name.c_str(),(*tmpit).name.c_str());
#endif
		if((*it).type == ElementType::_LINE)
		  beamline_list.insert(it,(*tmpit).lst->begin(),(*tmpit).lst->end());
		else if((*it).type == ElementType::_REV_LINE){
		  //iterate over list and invert any sublines contained within. SPM
		  std::list<struct Element> tmpList;
		  tmpList.insert(tmpList.end(),(*tmpit).lst->begin(),(*tmpit).lst->end());
		  for(std::list<struct Element>::iterator itLineInverter = tmpList.begin();
		      itLineInverter != tmpList.end(); itLineInverter++){
		    if((*itLineInverter).type == ElementType::_LINE)
		      (*itLineInverter).type = ElementType::_REV_LINE;
		    else if ((*itLineInverter).type == ElementType::_REV_LINE)
		      (*itLineInverter).type = ElementType::_LINE;
		  }
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
		  
		} else  // element of undefined type
		{
		  std::cerr << "Error : Expanding line \"" << name << "\" : element \"" << (*it).name << "\" has not been defined! " << std::endl;
		  exit(1);
		  // beamline_list.erase(it--);
		}
	      
	    } else  // element - keep as it is 
	    {
	      // do nothing
	    }
	  
	}
      iteration++;
      if( iteration > MAX_EXPAND_ITERATIONS )
	{
	  std::cerr << "Error : Line expansion of '" << name << "' seems to loop, " << std::endl
		    << "possible recursive line definition, quitting" << std::endl;
	  exit(1);
	}
    }// while
  
  
  // leave only the desired range
  //
  // rule - from first occurence of 'start' till first 'end' coming after 'start'
  
  
  if( !start.empty()) // determine the start element
    {
      sit = beamline_list.find(std::string(start));
      
      if(sit==beamline_list.end())
	{
	  sit = beamline_list.begin();
	}
      
      if(start == "#s") sit = beamline_list.begin(); 
      
      beamline_list.erase(beamline_list.begin(),sit);
      
    }
  
  if( !end.empty()) // determine the end element
    {
      eit = beamline_list.find(std::string(end));
      
      if(end == "#e") eit = beamline_list.end();
      
      beamline_list.erase(++eit,beamline_list.end());
    }
  
  
  // insert the tunnel if present
  
  it = element_list.find("tunnel");
  if(it!=iterEnd)
    beamline_list.push_back(*it);
  
  return 0;
}

void add_sampler(std::string name, std::string before, int before_count)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting sampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_SAMPLER;
  e.name = name;
  e.lst = nullptr;

  std::list<struct Element>::iterator it = beamline_list.find(before,before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    exit(1);
  }
  beamline_list.insert(it,e);
}

void add_csampler(std::string name, std::string before, int before_count, double length, double rad)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting csampler before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_CSAMPLER;
  e.l = length;
  e.r = rad;
  e.name = name;
  e.lst = nullptr;

  std::list<struct Element>::iterator it = beamline_list.find(before,before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    exit(1);
  }
  beamline_list.insert(it,e);
}

void add_dump(std::string name, std::string before, int before_count)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting dump before "<<before<<"["<<before_count<<"]"<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_DUMP;
  e.name = name;
  e.lst = nullptr;

  std::list<struct Element>::iterator it = beamline_list.find(before,before_count);
  if (it==beamline_list.end()) {
    std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
    exit(1);
  }
  beamline_list.insert(it,e);
}

void add_tunnel(Tunnel& tunnel)
{
  // copy from global
  struct Tunnel t(tunnel);
  // reset tunnel
  tunnel.clear();
#ifdef BDSDEBUG 
  t.print();
#endif
  tunnel_list.push_back(t);
}

void add_xsecbias(PhysicsBiasing& xsecbias)
{
  // copy from global
  PhysicsBiasing b(xsecbias);
  // reset xsecbias
  xsecbias.clear();
#ifdef BDSDEBUG 
  b.print();
#endif
  xsecbias_list.push_back(b);
}
 
double property_lookup(FastList<Element>& el_list, std::string element_name, std::string property_name)
{
  std::list<struct Element>::iterator it = el_list.find(element_name);
  std::list<struct Element>::const_iterator iterEnd = el_list.end();

  if(it == iterEnd) {
    std::cerr << "parser.h> Error: unknown element \"" << element_name << "\"." << std::endl; 
    exit(1);
  }

  return (*it).property_lookup(property_name);
}

void add_element_temp(std::string name, int number, bool pushfront, ElementType linetype)
{
#ifdef BDSDEBUG
  std::cout << "matched sequence element, " << name;
  if (number > 1) std::cout << " * " << number;
  std::cout << std::endl;
#endif
  // add to temporary element sequence
  struct Element e;
  e.name = name;
  e.type = linetype;
  e.lst = nullptr;
  if (pushfront) {
    for(int i=0;i<number;i++) {
      tmp_list.push_front(e);
    }
  }
  else {
    for(int i=0;i<number;i++) {
      tmp_list.push_back(e);
    }
  }
}

// ******************************************************
// parser functions
// ******************************************************


int add_func(std::string name, double (*func)(double))
{
  struct symtab *sp=symlook(name);
  sp->funcptr=func;
  return 0;
}

int add_var(std::string name, double value, int is_reserved)
{
  struct symtab *sp=symlook(name);
  sp->value=value;
  sp->is_reserved = is_reserved;
  return 0;
}

} // namespace
#endif
