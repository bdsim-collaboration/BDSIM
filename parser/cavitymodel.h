#ifndef __CAVITYMODEL_H
#define __CAVITYMODEL_H

#include <string>

/**
 * @brief RF CavityModel class for parser
 * 
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
 */

struct CavityModel {
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
  /// number of points that will be used to make the cell shape
  /// more will be more accurate but will also increase tracking time
  int numberOfPoints;
  /// number of cells per cavity
  int numberOfCells;
  
  /// constructor
  CavityModel();
  /// reset
  void clear();
  /// print some properties
  void print()const;

  /// set methods by property name, numeric values
  void set_value(std::string property, double value);
  /// set methods by property name, string values
  void set_value(std::string property, std::string value);
};

#endif
