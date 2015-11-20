#include "parser.h"

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
  std::map<std::string,symtab*>::iterator it;
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
  add_func("sqrt",sqrt);
  add_func("cos",cos);
  add_func("sin",sin);
  add_func("exp",exp);
  add_func("log",log); 
  add_func("tan",tan);
  add_func("asin",asin); 
  add_func("acos",acos);
  add_func("atan",atan);
  add_func("abs",fabs);
 
  add_var("pi",4.0*atan(1),reserved);

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

void Parser::quit()
{
  printf("parsing complete...\n");
  exit(0);
}

int Parser::write_table(std::string* name, ElementType type, std::list<struct Element> *lst)
{
  if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *name << " : " << type << std::endl;
#ifdef BDSDEBUG 
  printf("k1=%.10g, k2=%.10g, k3=%.10g, type=%s, lset = %d\n", params.k1, params.k2, params.k3, typestr(type).c_str(), params.lset);
#endif

  struct Element e;
  e.set(params,*name,type,lst);

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

int Parser::expand_line(std::string name, std::string start, std::string end)
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
      it = ++beamline_list.begin();
      for(;it!=beamline_list.end();it++)
	{
#ifdef BDSDEBUG 
	  std::cout << (*it).name << " , " << (*it).type << std::endl;
#endif
	  if((*it).type == ElementType::_LINE || (*it).type == ElementType::_REV_LINE)  // list - expand further	  
	    {
	      is_expanded = false;
	      // lookup the line in main list
	      std::list<struct Element>::const_iterator tmpit = element_list.find((*it).name);
	      
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
      std::list<struct Element>::const_iterator startIt = beamline_list.find(std::string(start));
      
      if(startIt==beamline_list.end())
	{
	  startIt = beamline_list.begin();
	}
      
      if(start == "#s") startIt = beamline_list.begin(); 
      
      beamline_list.erase(beamline_list.begin(),startIt);
      
    }
  
  if( !end.empty()) // determine the end element
    {
      std::list<struct Element>::iterator endIt = beamline_list.find(std::string(end));
      
      if(end == "#e") endIt = beamline_list.end();
      
      beamline_list.erase(++endIt,beamline_list.end());
    }
  
  
  // insert the tunnel if present
  
  it = element_list.find("tunnel");
  if(it!=iterEnd)
    beamline_list.push_back(*it);
  
  return 0;
}

void Parser::add_element(struct Element& e, std::string before, int before_count)
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
	// add element name to name
	e.name += it->name;
	beamline_list.insert(it,e);
	// reset name
	e.name = origName;
      }
      // add final element
      e.name += "end";
      beamline_list.push_back(e);
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
 
void Parser::add_sampler(std::string name, int before_count)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting sampler before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_SAMPLER;
  e.name = "Sampler_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count);
}

void Parser::add_csampler(std::string name, int before_count, double length, double rad)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting csampler before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_CSAMPLER;
  e.l = length;
  e.r = rad;
  e.name = "CSampler_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count);
}

void Parser::add_dump(std::string name, int before_count)
{
#ifdef BDSDEBUG 
  std::cout<<"inserting dump before "<<name;
  if (before_count!=-1) std::cout<<"["<<before_count<<"]";
  std::cout<<std::endl;
#endif

  struct Element e;
  e.type = ElementType::_DUMP;
  e.name = "Dump_" + name;
  e.lst = nullptr;

  // add element to beamline
  add_element(e, name, before_count);
}

void Parser::add_tunnel(Tunnel& tunnel)
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

void Parser::add_xsecbias(PhysicsBiasing& xsecbias)
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
 
double Parser::property_lookup(FastList<Element>& el_list, std::string element_name, std::string property_name)
{
  std::list<struct Element>::iterator it = el_list.find(element_name);
  std::list<struct Element>::const_iterator iterEnd = el_list.end();

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

int Parser::copy_element_to_params(std::string elementName)
{
  int type;
#ifdef BDSDEBUG
  std::cout << "newinstance : VARIABLE -- " << elementName << std::endl;
#endif
  std::list<struct Element>::iterator it = element_list.find(elementName);
  std::list<struct Element>::iterator iterEnd = element_list.end();
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
  struct symtab *sp=symcreate(name);
  sp->funcptr=func;
  return 0;
}

int Parser::add_var(std::string name, double value, int is_reserved)
{
  struct symtab *sp=symcreate(name);
  sp->value=value;
  sp->is_reserved = is_reserved;
  return 0;
}

struct symtab * Parser::symcreate(std::string s)
{
  std::map<std::string,symtab*>::iterator it = symtab_map.find(s);
  if (it!=symtab_map.end()) {
    std::cerr << "ERROR Variable " << s << " is already defined!" << std::endl;
    exit(1);
  }
    
  struct symtab* sp = new symtab(s);
  std::pair<std::map<std::string,symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(s,sp));
  return (*(ret.first)).second;
}
  
struct symtab * Parser::symlook(std::string s)
{
  std::map<std::string,symtab*>::iterator it = symtab_map.find(s);
  if (it==symtab_map.end()) {
    return nullptr;
  } 
  return (*it).second;
}
