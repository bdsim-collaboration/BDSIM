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

extern FILE* yyin;
extern int yylex();
int yyerror(const char *);

namespace GMAD {
  extern const int ECHO_GRAMMAR;
  extern const int PEDANTIC;

  /**
   * @brief Parser class
   * 
   * Parser class that holds all objects and relevant methods
   *
   * Singleton pattern
   *
   * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
   */
  
  class Parser
  {
  protected:
    /// Constructor from filename
    Parser(std::string filename);
  private:
    /// Instance
    static Parser* instance;
  public:
    /// No default constructor
    Parser() = delete;
    /// Constructor method
    static Parser* Instance(std::string filename);
    /// Access method
    static Parser* Instance();
    /// Destructor
    ~Parser();

    // public access methods, all const
    
    /// Return options
    const Options& GetOptions()const;
    /// Return beamline
    const FastList<Element>& GetBeamline()const;
    /// Return biasing list
    const FastList<PhysicsBiasing>& GetBiasing()const;
    /// Return material list
    const std::list<Element>& GetMaterials()const;
    /// Return atom list
    const std::list<Element>& GetAtoms()const;

  private:
    /// Initialisation of parser functions and constants
    void Initialise();
    /// Parse the input file and construct beamline_list and options 
    void ParseFile(FILE *f);
    
  public:
    const int MAX_EXPAND_ITERATIONS = 50;

    std::list<double> _tmparray;  // for reading of arrays
    std::list<std::string> _tmpstring;
    
    /// globals
    Parameters params;
    Options options;
    Tunnel tunnel;
    PhysicsBiasing xsecbias;
    
    // list of all encountered elements
    FastList<Element> element_list;
    
    // temporary list
    std::list<Element> tmp_list;
    
    // beamline
    FastList<Element> beamline_list;
    // list of parser defined materials
    std::list<Element>  material_list;
    // list of parser defined atoms
    std::list<Element>  atom_list;
    // list of parser defined tunnels
    std::vector<Tunnel> tunnel_list;
    // list of parser defined cross section biasing objects
    FastList<PhysicsBiasing> xsecbias_list;
    
    std::string current_line;
    std::string current_start;
    std::string current_end;
    
    // parser symbol map
    std::map<std::string, symtab*> symtab_map;
    // variable vector for memory storage
    std::vector<std::string*> var_list;
    
    // ***********************
    // functions declaration *
    // ***********************
    
    void quit();
    /// method that transfers parameters to element properties
    int write_table(std::string* name, ElementType type, std::list<Element> *lst=nullptr);
    int expand_line(std::string name, std::string start, std::string end);
    /// insert a sampler into beamline_list
    void add_sampler(std::string name, int before_count);
    /// insert a cylindrical sampler into beamline_list
    void add_csampler(std::string name, int before_count, double length, double rad);
    /// insert a beam dumper into beamline_list
    void add_dump(std::string name, int before_count);
    /// insert tunnel
    void add_tunnel(Tunnel& tunnel);
    /// insert xsecbias
    void add_xsecbias(PhysicsBiasing& xsecbias);
    double property_lookup(FastList<Element>& el_list, std::string element_name, std::string property_name);
    /// add element to temporary element sequence tmp_list
    void add_element_temp(std::string name, int number, bool pushfront, ElementType linetype);
    /// add element to beamline
    void add_element(Element& e, std::string before, int before_count);
    /// copy properties from Element into params, returns element type as integer, returs _NONE if not found
    int copy_element_to_params(std::string elementName);
    
    // parser functions
    int add_func(std::string name, double (*func)(double));
    int add_var(std::string name, double value, int is_reserved = 0);


    /// create new parser symbol
    symtab * symcreate(std::string s);

    /// look up parser symbol
    symtab * symlook(std::string s);


  };

}
    
#endif
