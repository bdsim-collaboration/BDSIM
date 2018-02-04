#ifndef ATOM_H
#define ATOM_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Atom class
   * 
   * @author Jochem Snuverink
   */

  struct Atom : public Published<Atom>{
    /// name
    std::string name;

    double A; ///< g*mol^-1
    double Z; 
    std::string symbol;

    /// constructor
    Atom();
    /// reset
    void clear();
    /// printout
    void print()const;
    /// set methods by property name and value
    template <typename T>
      void set_value(std::string property, T value);

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template <typename T>
    void Atom::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
        set(this,property,value);
      }
      catch(std::runtime_error) {
        std::cerr << "Error: parser> unknown atom option \"" << property << "\" with value " << value << std::endl;
        exit(1);
      }
    }
}

#endif
