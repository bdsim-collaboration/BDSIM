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

  /// radius
  double   radius;
  /// offset x and y
  double   offsetX, offsetY;
  /// tunnel geometry parameters
  double   tunnelThickness;
  double   tunnelSoilThickness;
  double   tunnelFloorOffset;
  
  /// material
  std::string material;
  //std::string tunnelCavityMaterial;
  /// soil
  std::string soilMaterial;

  /// start and end element by name
  std::string startElement, endElement;
  
  /// constructor
  Tunnel();
  
};

#endif
