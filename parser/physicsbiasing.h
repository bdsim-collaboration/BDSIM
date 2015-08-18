#ifndef __PHYSICSBIASING_H
#define __PHYSICSBIASING_H

#include <string>

/**
 * @brief Physicsbiasing class for parser
 * 
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> 
 */

namespace GMAD {
  struct Physicsbiasing {
   
  public:
    std::string name; ///< name
    /// particle name
    std::string particle;
    /// geant4 process: single string, but can have multiple processes separated with a space
    std::string process;
    /// factors corresponding to process
    std::list<double> factor;
    /// primary
    std::list<double> primary;

    /// set methods by property name, numeric values
    void set_value(std::string property, double value);
    /// set methods by property name, string values
    void set_value(std::string property, std::string value);
  };
}

#endif
