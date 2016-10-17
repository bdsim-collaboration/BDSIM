#ifndef FIELD_H
#define FIELD_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Field class for parser
   * 
   * @author Laurie Nevay
   */
  
  class Field : public Published<Field>
  {
  public:
    std::string name;                 ///< Name.
    std::string type;                 ///< Type.
    double      scaling;              ///< Scaling factor.
    std::string integrator;           ///< Which integrator to use.
    bool        globalTransform;      ///< Provide global to curvilinear coordinate transform.
    std::string magneticFile;         ///< File for magnetic field map.
    std::string magneticInterpolator; ///< Interpolator for the magnetic field.
    std::string electricFile;         ///< File for the electric field map.
    std::string electricInterpolator; ///< Interpolator for the electric field.

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
    bool   axisAngle; ///< Flag to use the axis angle construction of rotation.
    
    /// Constructor
    Field();
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
    void Field::set_value(std::string property, T value)
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
