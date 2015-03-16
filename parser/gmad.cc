 /*
 * GMAD interface
 *
 */

#include "gmad.h"
#include "elementlist.h"
#include "parameters.h"
#include "sym_table.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cstring>

extern struct Parameters params;

extern int yyparse();

extern FILE *yyin;
extern char* yyfilename;

extern int add_func(const char *name, double (*func)(double));
extern int add_var(const char *name, double val,int is_reserved = 0);

// aux. parser lists - to clear
extern ElementList element_list;
extern std::list<struct Element> tmp_list;
extern std::map<std::string, struct symtab*> symtab_map;

extern void print(std::list<Element> l, int ident);

namespace {
void init()
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
  add_var("KeV",1e-6,reserved);
  add_var("eV" ,1e-9,reserved);

  add_var("MV",1.0,reserved);

  add_var("Tesla",1.0,reserved);

  add_var("m"  ,1.0 ,reserved);
  add_var("cm" ,1e-2,reserved);
  add_var("mm" ,1e-3,reserved);
  add_var("um",1e-6,reserved);
  add_var("mum",1e-6,reserved);
  add_var("nm" ,1e-9,reserved);

  add_var("s"  ,1.0  ,reserved);
  add_var("ms" ,1.e-3,reserved);
  add_var("us",1.e-6,reserved);
  add_var("ns" ,1.e-9,reserved);

  add_var("rad" ,1.0  ,reserved);
  add_var("mrad",1.e-3,reserved);


  add_var("clight",2.99792458e+8,reserved);

  params.flush();

}
}

int gmad_parser(FILE *f)
{
  init();

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

#ifdef BDSDEBUG
  std::cout << "gmad_parser> finished" << std::endl;
#endif

  fclose(f);

  return 0;
}

int gmad_parser(std::string name)
{
  const int maxfilenamelength = 200;
#ifdef BDSDEBUG
  std::cout << "gmad_parser> opening file" << std::endl;
#endif
  FILE *f = fopen(name.c_str(),"r");

  if(f==NULL) {

    std::cerr << "gmad_parser> Can't open input file " << name << std::endl;
    exit(1);
  }

  yyfilename = new char[maxfilenamelength];
  strncpy(yyfilename,name.c_str(),maxfilenamelength);

  gmad_parser(f);

  return 0;
}


/** Python interface **/ 
int GmadParser_c(char *name) 
{
  gmad_parser(std::string(name));
  return 0;
}

int GetNelements() 
{
  return beamline_list.size();
}  

const char* GetName(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return (it->name).c_str();
}

short GetType(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->type;
}

double GetLength(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->l;
}

double GetAngle(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->angle;  
}

double* GetKs(int i)
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  double* result = new double[5];
  result[0] = it->ks;
  result[1] = it->k0;
  result[2] = it->k1;
  result[3] = it->k2;
  result[4] = it->k3;
  return result;
}

double GetBeampipeThickness(int i)
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->beampipeThickness;
}
