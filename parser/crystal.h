#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Crystal class for parser
   * 
   * @author Laurie Nevay
   */
  
  class Crystal: public Published<Crystal>
  {
  public:
    std::string name;                 ///< Name of object.
    std::string material;             ///< Material of crystal.
    std::string data;                 ///< Which channelling data to use.
    std::string shape;                ///< Which geometry to create.
    double      lengthX;              ///< X size.
    double      lengthY;              ///< Y size.
    double      lengthZ;              ///< Z size.
    double      sizeA;
    double      sizeB;
    double      sizeC;
    double      alpha;
    double      beta;
    double      gamma;
    int         spaceGroup;
    double      bendingAngleYAxis;    ///< Bending angle about Y axis.
    double      bendingAngleZAxis;    ///< Bending angle about Z axis.
    
    /// Constructor
    Crystal();
    /// Reset
    void clear();
    /// Print some properties
    void print()const;
    /// Set methods by property name and value
    template <typename T>
      void set_value(std::string property, T value);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
    void Crystal::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "crystal> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
        set(this,property,value);
      }
      catch(std::runtime_error) {
        std::cerr << "Error: crystal> unknown option \"" << property << "\" with value " << value  << std::endl;
        exit(1);
      }
    }
}

#endif
