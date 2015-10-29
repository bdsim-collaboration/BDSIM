#ifndef __ARRAY_H
#define __ARRAY_H

#include <iostream>
#include <string>
#include <vector>

namespace GMAD {
  /**
   * @brief Representation of arrays used in tokens
   *
   * Used for both std::string and double
   */

  struct Array {
    std::vector<std::string> symbols;
    std::vector<double> data;

    // helper methods

    // copy symbols into STL string containers
    template<template <typename, typename> class Container>
      void set_vector(Container<std::string, std::allocator<std::string>>& dst)
    {
      for(std::string symbol : symbols){
	dst.push_back(symbol);
#ifdef BDSDEBUG 
	std::cout << symbol << " ";
#endif
      }
#ifdef BDSDEBUG 
      std::cout << std::endl;
#endif
    }

    // copy data into STL numerical containers
    template<typename T, template <typename, typename> class Container>
      void set_vector(Container<T, std::allocator<T>>& dst) 
    {
      for(auto value : data){
	dst.push_back((T)value);
#ifdef BDSDEBUG 
	std::cout << (T)value << " ";
#endif
      }
#ifdef BDSDEBUG 
      std::cout << std::endl;
#endif
    }
  };
}

#endif
