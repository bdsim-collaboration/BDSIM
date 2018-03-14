#ifndef PHYSICSBIASING_H
#define PHYSICSBIASING_H

#include <string>
#include <list>
#include <vector>

namespace GMAD
{
  class Array;
}

namespace GMAD
{
  enum class PhysicsBiasingType {ALL=1,PRIMARY=2,SECONDARY=3};
  
  /**
   * @brief Physics biasing class for parser
   * 
   * @author Jochem Snuverink
   */
  class PhysicsBiasing {
    
  public:
    std::string name; ///< name
    /// particle name
    std::string particle;
    /// geant4 process: single string, but can have multiple processes separated with a space
    std::string process;
    std::vector<std::string> processList;
    /// factors corresponding to process
    std::vector<double> factor;
    /// flag which particles are biased
    std::vector<PhysicsBiasingType> flag;
    
    /// constructor
    PhysicsBiasing();
    /// reset
    void clear();
    /// print some properties
    void print()const;

    /// set methods by property name, numeric values
    void set_value(const std::string& property, double value);
    /// set methods by property name, list of numeric values
    void set_value(const std::string& property, Array* value);
    /// set methods by property name, string values
    void set_value(const std::string& property, std::string value);
  };

}

#endif
