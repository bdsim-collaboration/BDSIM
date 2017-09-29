#ifndef MATERIAL_H
#define MATERIAL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  class Array;
  
  /**
   * @brief Material class
   * 
   * @author Jochem Snuverink
   *
   */

  struct Material : public Published<Material>{
    /// name
    std::string name;

    double A; ///< g*mol^-1
    double Z;
    double density; ///< g*cm-3 
    double temper; ///< Kelvin
    double pressure; ///<atm
    std::string state; ///< "solid", "liquid", or "gas"
    std::list<std::string> components;
    std::list<double> componentsFractions;
    std::list<int> componentsWeights;

    /// constructor
    Material();
    /// reset
    void clear();
    /// printout
    void print()const;
    /// set methods by property name and value
    template <typename T>
      void set_value(const std::string& property, T value);
    // Template overloading for Array pointers
    /// Set method for lists
    void set_value(const std::string& property, Array* value);

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template <typename T>
    void Material::set_value(const std::string& property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
	set(this,property,value);
      }
      catch(std::runtime_error) {
	std::cerr << "Error: parser> unknown material option \"" << property << "\" with value " << value << std::endl;
	exit(1);
      }
    }
}

#endif
