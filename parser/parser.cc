#include "parser.h"

#include <cmath>

#include "array.h"
#include "sym_table.h"

using namespace GMAD;

namespace GMAD {
  extern std::string yyfilename;
}

extern int yyparse();
extern FILE *yyin;

Parser* Parser::instance = nullptr;

Parser* Parser::Instance()
{
  if(instance==nullptr) {
    std::cerr << "Parser has not been initialized!" << std::endl;
    exit(1);
  }
  return instance;
}

Parser* Parser::Instance(std::string name)
{
  if(instance) {
    std::cerr << "Parser is already initialized!" << std::endl;
    exit(1);
  }
  instance = new Parser(name);
  return instance;
}

Parser::~Parser()
{
  instance = nullptr;
}

Parser::Parser(std::string name)
{
  instance = this;
#ifdef BDSDEBUG
  std::cout << "gmad_parser> opening file" << std::endl;
#endif
  FILE *f = fopen(name.c_str(),"r");

  if(f==nullptr) {
    std::cerr << "gmad_parser> Can't open input file " << name << std::endl;
    exit(1);
  }

  yyfilename = std::string(name);

  Initialise();
  
  ParseFile(f);
}

void Parser::ParseFile(FILE *f)
{
  yyin=f; 

#ifdef BDSDEBUG
  std::cout << "gmad_parser> beginning to parse file" << std::endl;
#endif

  while(!feof(yyin))
    {
      yyparse();
    }

#ifdef BDSDEBUG
  std::cout << "gmad_parser> finished to parsing file" << std::endl;
#endif

  // clear temporary stuff

#ifdef BDSDEBUG
  std::cout << "gmad_parser> clearing temporary lists" << std::endl;
#endif
  element_list.clear();
  tmp_list.clear();
  std::map<std::string,Symtab*>::iterator it;
  for(it=symtab_map.begin();it!=symtab_map.end();++it) {
    delete (*it).second;
  }
  symtab_map.clear();
  for(auto it : var_list)
    {delete it;}

#ifdef BDSDEBUG
  std::cout << "gmad_parser> finished" << std::endl;
#endif

  fclose(f);
}

void Parser::Initialise()
{
  const int reserved = 1;
  // embedded arithmetical functions
  add_func("sqrt",std::sqrt);
  add_func("cos",std::cos);
  add_func("sin",std::sin);
  add_func("exp",std::exp);
  add_func("log",std::log);
  add_func("tan",std::tan);
  add_func("asin",std::asin);
  add_func("acos",std::acos);
  add_func("atan",std::atan);
  add_func("abs",std::abs);
 
  add_var("pi",4.0*std::atan(1),reserved);

  add_var("TeV",1e+3,reserved);
  add_var("GeV",1.0 ,reserved);
  add_var("MeV",1e-3,reserved);
  add_var("keV",1e-6,reserved);
  add_var("KeV",1e-6,reserved); // for compatibility
  add_var("eV" ,1e-9,reserved);

  add_var("MV",1.0,reserved);

  add_var("Tesla",1.0,reserved);

  add_var("km" ,1e3 ,reserved);
  add_var("m"  ,1.0 ,reserved);
  add_var("cm" ,1e-2,reserved);
  add_var("mm" ,1e-3,reserved);
  add_var("um" ,1e-6,reserved);
  add_var("mum",1e-6,reserved);
  add_var("nm" ,1e-9,reserved);
  add_var("pm" ,1e-12,reserved);

  add_var("s"  ,1.0  ,reserved);
  add_var("ms" ,1.e-3,reserved);
  add_var("us" ,1.e-6,reserved);
  add_var("ns" ,1.e-9,reserved);
  add_var("ps" ,1.e-12,reserved);

  add_var("rad" ,1.0  ,reserved);
  add_var("mrad",1.e-3,reserved);
  add_var("urad",1.e-6,reserved);

  add_var("clight",2.99792458e+8,reserved);

  params.flush();
}

const Options& Parser::GetOptions()const
{
  return options;
}

const FastList<Element>& Parser::GetBeamline()const
{
  return beamline_list;
}

const FastList<PhysicsBiasing>& Parser::GetBiasing()const
{
  return xsecbias_list;
}

const std::list<Element>& Parser::GetMaterials()const
{
  return material_list;
}

const std::list<Element>& Parser::GetAtoms()const
{
  return atom_list;
}

const FastList<Element>& Parser::GetElements()const
{
  return element_list;
}

void Parser::quit()
{
  printf("parsing complete...\n");
  exit(0);
}

void Parser::write_table(std::string* name, ElementType type, bool isLine)
{
  if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *name << " : " << type << std::endl;
#ifdef BDSDEBUG 
  printf("k1=%.10g, k2=%.10g, k3=%.10g, type=%s, lset = %d\n", params.k1, params.k2, params.k3, typestr(type).c_str(), params.lset);
#endif

  Element e;
  e.set(params,*name,type);
  if (isLine)
    {
      e.lst = new std::list<Element>(tmp_list);
      // clean list
      tmp_list.clear();
    }

  switch(type) {

  case ElementType::_MATERIAL:
    material_list.push_back(e);
    return;
    
  case ElementType::_ATOM:
    atom_list.push_back(e);
    return;
    
  default:
    break;
  }
  
  // insert element with uniqueness requirement
  element_list.push_back(e,true);
}

void Parser::expand_line(std::string name, std::string start, std::string end)
{
  std::list<Element>::const_iterator iterEnd = element_list.end();
  
  std::list<Element>::const_iterator itName = element_list.find(name);
  
  if (itName==iterEnd) {
    std::cerr << "ERROR: line '" << name << "' not found" << std::endl;
    exit(1);
  }
  if((*itName).type != ElementType::_LINE && (*itName).type != ElementType::_REV_LINE ) {
    std::cerr << "'ERROR" << name << "' is not a line" << std::endl;
    exit(1);
  }

  // delete the previous beamline
  
  beamline_list.clear();
  
  // expand the desired beamline
  
  Element e;
  e.type = (*itName).type;
  e.name = name;
  e.l = 0;
  e.lst = nullptr;
  
  beamline_list.push_back(e);

#ifdef BDSDEBUG 
  std::cout << "expanding line " << name << ", range = " << start << end << std::endl;
#endif
  if(!(*itName).lst) return; //list empty
    
  // first expand the whole range 
  std::list<Element>::iterator sit = (*itName).lst->begin();
  std::list<Element>::iterator eit = (*itName).lst->end();
  
  // copy the list into the resulting list
  switch((*itName).type){
  case ElementType::_LINE:
    beamline_list.insert(beamline_list.end(),sit,eit);
    break;
  case ElementType::_REV_LINE:
    beamline_list.insert(beamline_list.end(),(*itName).lst->rbegin(),(*itName).lst->rend());
    break;
  default:
    beamline_list.insert(beamline_list.end(),sit,eit);
  }
  // bool to check if beamline is fully expanded
  bool is_expanded = false;
  
  // insert material entries.
  // TODO:::
  
  // parse starting from the second element until the list is expanded
  int iteration = 0;
  while(!is_expanded)
    {
      is_expanded = true;
      // start at second element
      std::list<Element>::iterator it = ++beamline_list.begin();
      for(;it!=beamline_list.end();it++)
	{
#ifdef BDSDEBUG 
	  std::cout << (*it).name << " , " << (*it).type << std::endl;
#endif
	  if((*it).type == ElementType::_LINE || (*it).type == ElementType::_REV_LINE)  // list - expand further	  
	    {
	      is_expanded = false;
	      // lookup the line in main list
	      std::list<Element>::const_iterator tmpit = element_list.find((*it).name);
	      
	      if( (tmpit != iterEnd) && ( (*tmpit).lst != nullptr) ) { // sublist found and not empty
		
#ifdef BDSDEBUG
		printf("inserting sequence for %s - %s ...",(*it).name.c_str(),(*tmpit).name.c_str());
#endif
		if((*it).type == ElementType::_LINE)
		  beamline_list.insert(it,(*tmpit).lst->begin(),(*tmpit).lst->end());
		else if((*it).type == ElementType::_REV_LINE){
		  //iterate over list and invert any sublines contained within. SPM
		  std::list<Element> tmpList;
		  tmpList.insert(tmpList.end(),(*tmpit).lst->begin(),(*tmpit).lst->end());
		  for(std::list<Element>::iterator itLineInverter = tmpList.begin();
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
      std::list<Element>::const_iterator startIt = beamline_list.find(std::string(start));
      
      if(startIt!=beamline_list.end())
	{
	  beamline_list.erase(beamline_list.begin(),startIt);
	}
    }
  
  if( !end.empty()) // determine the end element
    {
      std::list<Element>::const_iterator endIt = beamline_list.find(std::string(end));
      
      if(endIt!=beamline_list.end())
	{
	  beamline_list.erase(++endIt,beamline_list.end());
	}
    }
  
  // insert the tunnel if present
  
  std::list<Element>::iterator itTunnel = element_list.find("tunnel");
  if(itTunnel!=iterEnd)
    beamline_list.push_back(*itTunnel);
}

void Parser::add_element(Element& e, std::string before, int before_count, ElementType type)
{
  // if before_count equal to -2 add to all elements regardless of name
  // typically used for output elements like samplers
  // skip first element and add one at the end
  if (before_count==-2)
    {
      std::string origName = e.name;
      // flag to see if first element has already been skipped
      bool skip = false;
      for (auto it=beamline_list.begin(); it!=beamline_list.end(); it++) {
	// skip LINEs
	if((*it).type == ElementType::_LINE || (*it).type == ElementType::_REV_LINE)
	  {continue;}
	// skip first real element
	if (skip == false) {
	  skip=true;
	  continue;
	}
	// skip all elements of type not equal to NONE
	if (type != ElementType::_NONE && type != (*it).type) {
	  continue;
	}
	
	// add element name to name
	e.name += it->name;
	beamline_list.insert(it,e);
	// reset name
	e.name = origName;
      }
      // if add sampler to all also add to final element
      if (type == ElementType::_NONE) {
	// add final element
	e.name += "end";
	beamline_list.push_back(e);
      }
      // reset name (not really needed)
      e.name = origName;
    }
  // if before_count equal to -1 add to all element instances
  else if (before_count==-1)
    {
      auto itPair = beamline_list.equal_range(before);
      if (itPair.first==itPair.second) {
	std::cerr<<"current beamline doesn't contain element "<< before << std::endl;
	exit(1);
      }
      for (auto it = itPair.first; it!= itPair.second; ++it) 
	{beamline_list.insert(it->second,e);}
    }
  else
    {
      auto it = beamline_list.find(before,before_count);
      if (it==beamline_list.end()) {
	std::cerr<<"current beamline doesn't contain element "<<before<<" with number "<<before_count<<std::endl;
	exit(1);
      }
      beamline_list.insert(it,e);
    }
}
 
void Parser::add_sampler(std::string name, int before_count, ElementType type)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting sampler before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  Element e;
  e.type = ElementType::_SAMPLER;
  e.name = "Sampler_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count, type);
}

void Parser::add_csampler(std::string name, int before_count, ElementType type)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting csampler before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  Element e;
  e.type = ElementType::_CSAMPLER;
  e.l = params.l;
  e.r = params.r;
  e.name = "CSampler_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count, type);
}

void Parser::add_dump(std::string name, int before_count, ElementType type)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting dump before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  Element e;
  e.type = ElementType::_DUMP;
  e.name = "Dump_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count, type);
}

void Parser::add_tunnel()
{
  // copy from global
  Tunnel t(tunnel);
  // reset tunnel
  tunnel.clear();
#ifdef BDSDEBUG 
  t.print();
#endif
  tunnel_list.push_back(t);
}

void Parser::add_xsecbias()
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
 
double Parser::property_lookup(const FastList<Element>& el_list, std::string element_name, std::string property_name)
{
  std::list<Element>::const_iterator it = el_list.find(element_name);
  std::list<Element>::const_iterator iterEnd = el_list.end();

  if(it == iterEnd) {
    std::cerr << "parser.h> Error: unknown element \"" << element_name << "\"." << std::endl; 
    exit(1);
  }

  return (*it).property_lookup(property_name);
}

void Parser::add_element_temp(std::string name, int number, bool pushfront, ElementType linetype)
{
#ifdef BDSDEBUG
  std::cout << "matched sequence element, " << name;
  if (number > 1) std::cout << " * " << number;
  std::cout << std::endl;
#endif
  // add to temporary element sequence
  Element e;
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

int Parser::copy_element_to_params(std::string elementName)
{
  int type;
#ifdef BDSDEBUG
  std::cout << "newinstance : VARIABLE -- " << elementName << std::endl;
#endif
  std::list<Element>::iterator it = element_list.find(elementName);
  std::list<Element>::iterator iterEnd = element_list.end();
  if(it == iterEnd)
    {
      std::cout << "type " << elementName << " has not been defined" << std::endl;
      if (PEDANTIC) exit(1);
      type = static_cast<int>(ElementType::_NONE);
    }
  else
    {
      // inherit properties from the base type
      type = static_cast<int>((*it).type);
      params.inherit_properties(*it);
    }
  return type;
}

int Parser::add_func(std::string name, double (*func)(double))
{
  Symtab *sp=symcreate(name);
  sp->funcptr=func;
  return 0;
}

int Parser::add_var(std::string name, double value, int is_reserved)
{
  Symtab *sp=symcreate(name);
  sp->value=value;
  sp->is_reserved = is_reserved;
  return 0;
}

Symtab * Parser::symcreate(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it!=symtab_map.end()) {
    std::cerr << "ERROR Variable " << s << " is already defined!" << std::endl;
    exit(1);
  }
    
  Symtab* sp = new Symtab(s);
  std::pair<std::map<std::string,Symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(s,sp));
  return (*(ret.first)).second;
}
  
Symtab * Parser::symlook(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it==symtab_map.end()) {
    return nullptr;
  } 
  return (*it).second;
}

void Parser::Store(double value)
{
  tmparray.push_front(value);
}

void Parser::Store(std::string name)
{
  tmpstring.push_front(name);
}

void Parser::FillArray(Array* array)
{
  for(double value : tmparray)
    {
      array->data.push_back(value);
    }
  tmparray.clear();
}

void Parser::FillString(Array* array)
{  
  for(std::string name : tmpstring)
    {
      array->symbols.push_back(name);
    }
  tmpstring.clear();
}

void Parser::ClearParams()
{
  params.flush();
}

void Parser::OverwriteElement(std::string elementName)
{
  std::list<Element>::iterator it = element_list.find(elementName);
  std::list<Element>::const_iterator iterEnd = element_list.end();
  if(it == iterEnd)
    {
      std::cout << "element " << elementName << " has not been defined" << std::endl;
      if (PEDANTIC) exit(1);
    }
  else
    {
      // add and overwrite properties if set
      (*it).set(params);
    }
  ClearParams();
}

void Parser::AddVariable(std::string* name)
{
  var_list.push_back(name);
}
