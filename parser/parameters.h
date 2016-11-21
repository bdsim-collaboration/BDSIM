#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "element.h"

namespace GMAD
{
  class Array;

  /**
   * @brief Parameters - Element class with booleans
   * 
   * This is a temporary storage class of the parameters before the actual Element is created. 
   * The booleans are needed for inheritance and extension of already created Elements.
   * The class is a singleton.
   *
   * @author Jochem Snuverink (based on I. Agapov)
   */

  struct Parameters : public Element {

    /// Map that holds booleans for every member of element
    std::map<std::string,bool> setMap;

    /// Reset the parameters to defaults and setMap
    void flush();

    /// Copy parameters into temporary buffer params from element e
    /// Parameters already set in params have priority and are not overridden
    void inherit_properties(Element& e);

    /// Set method by property name and value
    template <typename T>
    void set_value(std::string property, T value);
    // Template overloading for Array pointers
    /// Set method for lists
    void set_value(std::string property, Array* value);

    /// Constructor
    Parameters();
  };

  template <typename T>
    void Parameters::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "element> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
	Published<Element>::set(this,property,value);
      }
      catch(std::runtime_error) {
	// not implemented mad parameters will be ignored
	if (property == "hgap" || property == "harmon" || property == "lag" || property == "volt")
	  {return;}
	
	std::cerr << "Error: element> unknown option \"" << property << "\" with value " << value  << std::endl;
	exit(1);
      }
      // record property set
      // property name can be different, so look up in alternate names
      std::string publishedName = getPublishedName(property);
      setMap.at(publishedName) = true;
    }
}

#endif
