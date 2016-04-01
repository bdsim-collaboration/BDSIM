#ifndef CAVITYMODEL_H
#define CAVITYMODEL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief RF CavityModel class for parser
   * 
   * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
   */
  class CavityModel: public Published<CavityModel> {
  public:
    std::string name; ///< name
    /// geometry type
    std::string type;
    /// iris radius
    double irisRadius;
    /// equator radius
    double equatorRadius;
    /// half cell length
    double halfCellLength;
    /// equator ellipse semi axis;
    double equatorEllipseSemiAxis;
    /// iris horizontal axis
    double irisHorizontalAxis;
    /// iris vertical axis
    double irisVerticalAxis;
    /// tangent angle
    double tangentLineAngle;
    /// thickness
    double thickness;
    /// frequency in Hz
    double frequency;
    /// phase in radians
    double phase;
    /// Electric Field in Volts / m
    double eField;
    /// number of points that will be used to make the cell shape
    /// more will be more accurate but will also increase tracking time
    int numberOfPoints;
    /// number of cells per cavity
    int numberOfCells;
    
    /// constructor
    CavityModel();
    /// reset
    void clear();
    /// publish members
    void PublishMembers();
    /// print some properties
    void print()const;

    /// set methods by property name
    template <typename T>
      void set_value(std::string name, T value);
  };
  template <typename T>
    void CavityModel::set_value(std::string name, T value)
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
