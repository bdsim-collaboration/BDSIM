#ifndef __ARRAY_H
#define __ARRAY_H

#include <iostream>
#include <list>
#include <string>
#include <vector>

// representation of arrays used in tokens
struct Array {
  std::vector<char*> symbols;
  double *data;
  int size;

  // helper methods
  void set_vector(std::list<double>& dst)
  {
    for(int i=0; i< size;i++){
      dst.push_back(data[i]);
#ifdef BDSDEBUG 
      std::cout << data[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<char*>& dst)
  {
    for(int i=0; i< size;i++){
      dst.push_back(symbols[i]);
#ifdef BDSDEBUG 
      std::cout << symbols[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<const char*>& dst)
  {
    for(int i=0; i< size;i++){
      dst.push_back(symbols[i]);
#ifdef BDSDEBUG 
      std::cout << symbols[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<std::string>& dst)
  {
    for(int i=0; i< size;i++){
      dst.push_back((std::string)symbols[i]);
#ifdef BDSDEBUG 
      std::cout << (std::string)symbols[i] << " ";
#endif
    }
#ifdef BDSDEBUG 
    std::cout << std::endl;
#endif
  }

  void set_vector(std::list<int>& dst)
  {
    for(int i=0; i< size;i++){
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
