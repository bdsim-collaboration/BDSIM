#ifndef __ARRAY_H
#define __ARRAY_H

#include <iostream>
#include <list>
#include <string>
#include <vector>

// representation of arrays used in tokens
struct Array {
  std::vector<std::string> symbols;
  std::vector<double> data;

  // helper methods
  void set_vector(std::list<double>& dst)
  {
    for(unsigned int i=0; i< data.size();i++){
      dst.push_back(data[i]);
#ifdef BDSDEBUG 
      std::cout << data[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<std::string>& dst)
  {
    for(unsigned int i=0; i< symbols.size();i++){
      dst.push_back(symbols[i]);
#ifdef BDSDEBUG 
      std::cout << symbols[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<int>& dst)
  {
    for(unsigned int i=0; i< data.size();i++){
      dst.push_back((int)(data[i]));
#ifdef BDSDEBUG 
      std::cout << (int)(data[i]) << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }
};

#endif
