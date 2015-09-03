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
