#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "atom.h"
#include "cavitymodel.h"
#include "element.h"
#include "elementtype.h"
#include "field.h"
#include "fastlist.h"
#include "material.h"
#include "options.h"
#include "parameters.h"
#include "physicsbiasing.h"
#include "region.h"
#include "symbolmap.h"
#include "tunnel.h"

/// parser error message, defined in parser.y
int yyerror(const char *);
/// declaration needed by bison
extern int yylex();

namespace GMAD
{
  class Array;
  class Symtab;
  /**
   * @brief Parser class
   * 
   * Parser class that holds all objects and relevant methods
   *
   * Singleton pattern
   *
   * @author Jochem Snuverink
   */
  
  class Parser
  {
  public:
    /// No default constructor
    Parser() = delete;
    /// Constructor method
    static Parser* Instance(std::string filename);
    /// Access method
    static Parser* Instance();
    /// Destructor
    virtual ~Parser();

  protected:
    /// Constructor from filename
    Parser(std::string filename);
  private:
    /// Instance
    static Parser* instance;
    /// Initialisation of parser functions and constants
    void Initialise();
    /// Parse the input file and construct beamline_list and options 
    void ParseFile(FILE *f);

  public:
    // ***********************
    // Public Parser methods *
    // ***********************

    /// Exit method
    void quit();
    /// Method that transfers parameters to element properties
    void write_table(std::string* name, ElementType type, bool isLine=false);
    /// Remove sublines from beamline, expand all into one LINE
    void expand_line(std::string name, std::string start, std::string end);
    /// insert a sampler into beamline_list
    void add_sampler(std::string name, int count, ElementType type);
    /// insert a cylindrical sampler into beamline_list
    void add_csampler(std::string name, int count, ElementType type);
    /// Insert global object of parser class C in Container class
    template <class C, class Container=std::vector<C>>
      void Add();
    /// Get global object of parser class C
    template <class C>
      C& GetGlobal();
    /// Get list for parser class C
    template <class C, class Container=std::vector<C>>
      Container& GetList();

    /// access property of Element with element_name
    double property_lookup(std::string element_name, std::string property_name);
    /// add element to temporary element sequence tmp_list
    void add_element_temp(std::string name, int number, bool pushfront, ElementType linetype);
    /// copy properties from Element into params, returns element type as integer, returs _NONE if not found
    int copy_element_to_params(std::string elementName);

    /// create new parser symbol
    Symtab * symcreate(std::string s);
    /// look up parser symbol
    Symtab * symlook(std::string s);

    ///@{ Add value to front of temporary list
    void Store(double value);
    void Store(std::string name);
    ///@}
    ///@{ Fill array object from temporary list and clear temporary list
    void FillArray(Array*);
    void FillString(Array*);
    ///@}
    /// Reset parameters
    void ClearParams();
    /// Set value for parser class
    template <class C, typename T>
      void SetValue(std::string property, T value);

    /// Overwrite element with current parameters
    void OverwriteElement(std::string elementName);
    /// Add variable memory to variable list for memory management
    void AddVariable(std::string* name);
    ///@{ Print methods
    void PrintBeamline()const;
    void PrintElements()const;
    void PrintOptions()const;
    ///@}
    
    ///@{ Name of beamline
    std::string current_line;
    std::string current_start;
    std::string current_end;
    ///@}
    /// Beamline Access (for pybdsim)
    const FastList<Element>& GetBeamline()const;
    
  private:
    // *****************
    // Private methods *
    // *****************
    
    /// Set sampler
    void set_sampler(std::string name, int count, ElementType type, std::string samplerType, double samplerRadius=0.0);
    /// Add function to parser
    void add_func(std::string name, double (*func)(double));
    /// Add reserved variable to parser
    void add_var(std::string name, double value, int is_reserved = 0);

    // *****************
    // Private members *
    // *****************
    /// maximum number of nested lines
    const int MAX_EXPAND_ITERATIONS = 50;
    const int PEDANTIC = 1; ///< strict checking, exits when element or parameter is not known

    ///@{ temporary list for reading of arrays in parser
    std::list<double> tmparray;
    std::list<std::string> tmpstring;
    ///@}
    /// vector of defined lines for memory management
    std::vector<std::list<Element>*> allocated_lines;

    // protected implementation (for inheritance to BDSParser - hackish)
  protected:
    /// Parameters to copy to Element
    Parameters params;
    /// General options
    Options options;
    /// Atom instance;
    Atom atom;
    /// Material instance;
    Material material;
    /// Region instance;
    Region region;
    /// Field instance;
    Field field;
    /// Tunnel instance
    Tunnel tunnel;
    /// PhysicsBiasing instance 
    PhysicsBiasing xsecbias;
    /// RF Cavity model instance
    CavityModel cavitymodel;
    
    /// List of all encountered elements
    FastList<Element> element_list;
    
    /// Temporary list
    std::list<Element> tmp_list;
    
    /// Beamline
    FastList<Element>   beamline_list;
    /// List of parser defined atoms
    std::vector<Atom>   atom_list;
    /// List of parser defined materials
    std::vector<Material> material_list;
    /// List of parser defined regions
    std::vector<Region> region_list;
    /// List of parser defined fields
    std::vector<Field>  field_list;
    /// List of parser defined tunnels
    std::vector<Tunnel> tunnel_list;
    /// List of parser defined cross section biasing objects
    FastList<PhysicsBiasing> xsecbias_list;
    /// List of parser defined rf cavity models
    std::vector<CavityModel> cavitymodel_list;
    
    /// Parser symbol map
    SymbolMap symtab_map;
    /// Variable vector for memory storage
    std::vector<std::string*> var_list;
  };

  template <class C, class Container>
    void Parser::Add()
    {
      // copy from global
      C& global = GetGlobal<C>();
      C instance(global);
      // reset global
      global.clear();
#ifdef BDSDEBUG 
      instance.print();
#endif
      GetList<C, Container>().push_back(instance);
    }

  template <class C, typename T>
    void Parser::SetValue(std::string property, T value)
    {
      GetGlobal<C>().set_value(property, value);
    }
}

#endif
