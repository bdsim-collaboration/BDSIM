#ifndef __PARSER_H
#define __PARSER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "element.h"
#include "fastlist.h"
#include "elementtype.h"
#include "options.h"
#include "parameters.h"
#include "physicsbiasing.h"
#include "tunnel.h"

/// parser error message, defined in parser.y
int yyerror(const char *);
/// declaration needed by bison
extern int yylex();

namespace GMAD {
  extern const int ECHO_GRAMMAR;

  struct Array;
  struct symtab;
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
  public:
    /// No default constructor
    Parser() = delete;
    /// Constructor method
    static Parser* Instance(std::string filename);
    /// Access method
    static Parser* Instance();
    /// Destructor
    ~Parser();

    // Public reading methods, all const
    
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
    void add_sampler(std::string name, int before_count);
    /// insert a cylindrical sampler into beamline_list
    void add_csampler(std::string name, int before_count);
    /// insert a beam dumper into beamline_list
    void add_dump(std::string name, int before_count);
    /// insert tunnel
    void add_tunnel();
    /// insert cross section bias
    void add_xsecbias();
    /// access property of Element with element_name
    double property_lookup(const FastList<Element>& el_list, std::string element_name, std::string property_name);
    /// add element to temporary element sequence tmp_list
    void add_element_temp(std::string name, int number, bool pushfront, ElementType linetype);
    /// copy properties from Element into params, returns element type as integer, returs _NONE if not found
    int copy_element_to_params(std::string elementName);

    /// create new parser symbol
    symtab * symcreate(std::string s);

    /// look up parser symbol
    symtab * symlook(std::string s);

    /// Return list of all defined elements
    const FastList<Element>& GetElements() const;

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
    /// Set parameter value
    template <typename T>
      void SetParameterValue(std::string property, T value);
    /// Set tunnel value
    template <typename T>
      void SetTunnelValue(std::string property, T value);
    /// Set physics biasing value
    template <typename T>
      void SetPhysicsBiasValue(std::string property, T value);
    /// Set options value
    template <typename T>
      void SetOptionsValue(std::string property, T value);
    /// Overwrite element with current parameters
    void OverwriteElement(std::string elementName);
    /// Add variable memory to variable list for memory management
    void AddVariable(std::string* name);
    
    ///@{ Name of beamline
    std::string current_line;
    std::string current_start;
    std::string current_end;
    ///@}
    
  private:
    // *****************
    // Private methods *
    // *****************
    
    /// Add element to beamline
    void add_element(Element& e, std::string before, int before_count);
    /// Add function to parser
    int add_func(std::string name, double (*func)(double));
    /// Add reserved variable to parser
    int add_var(std::string name, double value, int is_reserved = 0);

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
    
    /// Parameters to copy to Element
    Parameters params;
    /// General options
    Options options;
    
    /// Tunnel instance
    Tunnel tunnel;
    /// PhysicsBiasing instance 
    PhysicsBiasing xsecbias;
    
    /// List of all encountered elements
    FastList<Element> element_list;
    
    /// Temporary list
    std::list<Element> tmp_list;
    
    /// Beamline
    FastList<Element>   beamline_list;
    /// List of parser defined materials
    std::list<Element>  material_list;
    /// List of parser defined atoms
    std::list<Element>  atom_list;
    /// List of parser defined tunnels
    std::vector<Tunnel> tunnel_list;
    /// List of parser defined cross section biasing objects
    FastList<PhysicsBiasing> xsecbias_list;
    
    /// Parser symbol map
    std::map<std::string, symtab*> symtab_map;
    /// Variable vector for memory storage
    std::vector<std::string*> var_list;
  };

  template <typename T>
    void Parser::SetParameterValue(std::string property, T value)
    {
      params.set_value(property, value);
    }
  template <typename T>
    void Parser::SetTunnelValue(std::string property, T value)
    {
      tunnel.set_value(property, value);
    }
  template <typename T>
    void Parser::SetPhysicsBiasValue(std::string property, T value)
    {
      xsecbias.set_value(property, value);
    }
  template <typename T>
    void Parser::SetOptionsValue(std::string property, T value)
    {
      options.set_value(property, value);
    }
}

#endif
