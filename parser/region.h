#ifndef REGION_H
#define REGION_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Region class for parser
   * 
   * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
   */
  class Region : public Published<Region> {
  public:
    std::string name; ///< name

    double   prodCutPhotons;
    double   prodCutElectrons;
    double   prodCutPositrons;
    double   prodCutHadrons;

    /// constructor
    Region();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name
    template <typename T>
      void set_value(std::string name, T value);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
    void Region::set_value(std::string name, T value)
    {
#ifdef BDSDEBUG
      std::cout << "parser> Setting value " << std::setw(25) << std::left << name << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
	set(this,name,value);
      }
      catch(std::runtime_error) {
	std::cerr << "Error: parser> unknown option \"" << name << "\" with value " << value  << std::endl;
	exit(1);
      }
    }
}

#endif
