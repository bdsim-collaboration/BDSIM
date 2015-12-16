#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"
#include "optionsBase.h"

namespace GMAD
{
  /**
   * @brief Options class
   * 
   * options passed with option and beam command
   *
   * @author I. Agapov
   */
  class Options : public Published<OptionsBase>, public OptionsBase
  {
  public:
    Options();

    /// set methods by property name
    template<typename T>
    void set_value(std::string name, T value);

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template<typename T>
  void Options::set_value(std::string name, T value)
  {
#ifdef BDSDEBUG
    std::cout << "parser> Setting value " << std::setw(25) << std::left << name << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
    {
      set(this, name, value);
    }
    catch (std::runtime_error)
    {
      std::cerr << "Error: parser> unknown option \"" << name << "\" with value " << value << std::endl;
      exit(1);
    }
  }
}

#endif
