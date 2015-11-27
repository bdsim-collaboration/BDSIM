#ifndef __ARRAY_H
#define __ARRAY_H

#include <iostream>
#include <string>
#include <vector>

namespace GMAD {
  class Symtab;
}

namespace GMAD {
  /**
   * @brief Representation of arrays used in tokens
   *
   * Used for both std::string and double
   */

  class Array {
    // TODO: needs better data protection

  public:
    /// Representation of string array
    std::vector<std::string> symbols;
    /// Representation of double array
    std::vector<double> data;

  public:
    /// Symtabs and arrays are kind of interchangeable
    friend class Symtab;
    /// Default Constructor
    Array();
    /// Constructor from array
    Array(Array*, bool isString);
    /// Constructor from symbol
    explicit Array(Symtab*);
    /// Constructor from adding 2 double arrays
    static Array* Add(Array* a1, Array* a2);
    /// Constructor from subtracting 2 double arrays
    static Array* Subtract(Array* a1, Array* a2);
    /// Constructor from adding an array
    static Array* Add(Array* a, double d);
    /// Constructor from subtracting an array
    static Array* Subtract(Array* a, double d);
    /// Constructor from multiplying an array
    static Array* Multiply(Array* a, double d);
    /// Constructor from dividing an array
    static Array* Divide(Array* a, double d);
    /// Scalar vector product
    double Product(Array* a);

    /// Clear data
    void Clear();
    /// Print data
    void Print();
    /// Copy symbols into STL string containers
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

    /// Copy data into STL numerical containers
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
