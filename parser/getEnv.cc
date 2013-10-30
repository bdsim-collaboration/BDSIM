#include <cstdlib>
#include <iostream>
#include "getEnv.h"

std::string getEnv(const char* env)
{
  //Check if it is only spaces
  std::string test = env;
  bool spacesOnly = (!test.find_first_not_of(' ') != std::string::npos);
  const char* tmp = std::getenv(env);
  if((tmp==NULL) || (tmp=="") || spacesOnly){
    std::cout << __FUNCTION__ << "> getEnv - no environment variable \"" << env << "\" is set" << std::endl;
    return (const char*)"";
  } else {
    std::string stmp = (std::string)tmp+ "/";
    std::cout << __FUNCTION__ << "> getEnv - environment variable \"" << stmp << "\" is set" << std::endl;
    return stmp;
  }
}
