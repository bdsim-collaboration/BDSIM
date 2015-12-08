#ifndef __TUNNEL_H
#define __TUNNEL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD {
  /**
   * @brief Tunnel class for parser
   * 
   * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
   */
  struct Tunnel: public Published<Tunnel> {
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

    /// visibility
    int      visible;
  
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
    /// Publish members
    void PublishMembers();
    /// print some properties
    void print()const;
    /// set methods by property name
    template <typename T>
      void set_value(std::string name, T value);
  };
  template <typename T>
    void Tunnel::set_value(std::string name, T value)
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
