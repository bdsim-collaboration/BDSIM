#ifndef PLACEMENT_H
#define PLACEMENT_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Placement class for parser
   * 
   * @author Laurie Nevay
   */
  class Placement : public Published<Placement>
  {
  public:
    std::string name;         ///< Name of this placement.
    std::string geometryFile; ///< Geometry to load in format:path.
    double x; ///< Offset in x.
    double y; ///< Offset in y.
    double z; ///< Offset in z.
    /// @{ Euler angle for rotation.
    double phi;
    double theta;
    double psi;
    /// @}
    /// @{ Axis angle rotation.
    double axisX;
    double axisY;
    double axisZ;
    double angle;
    /// @}
    bool   checkOverlaps; ///< Whether to recursively check overlaps when placing.
    bool   sensitive;     ///< Whether to record hits or not.
    
    /// constructor
    Placement();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
      void set_value(std::string property, T value);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
    void Placement::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
	set(this,property,value);
      }
      catch(std::runtime_error) {
	std::cerr << "Error: parser> unknown option \"" << property << "\" with value " << value  << std::endl;
	exit(1);
      }
    }
}

#endif
