#ifndef __TUNNEL_H
#define __TUNNEL_H

#include <string>

/**
 * @brief Tunnel class for parser
 * 
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
 */

struct Tunnel {
  std::string name; ///< name
  /// geometry type
  std::string type;

  /// radius, aperture parameters
  double   aper1;
  double   aper2;
  /// offset x and y
  double   offsetX, offsetY;
  /// tunnel geometry parameters
  double   thickness;
  double   soilThickness;
  double   floorOffset;
  
  /// material
  std::string material;
  /// soil
  std::string soilMaterial;

  /// start and end element by name
  std::string startElement, endElement;
  
  /// constructor
  Tunnel();
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
