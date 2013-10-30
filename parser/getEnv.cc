#include <cstdlib>
#include <iostream>
#include "getEnv.h"

std::string getEnv(const char* env)
{
  //Check if it is only spaces
  const char* tmp = std::getenv(env);
  bool spacesOnly=true;
  if(tmp){
    std::string test = tmp;
    spacesOnly =  test.find_first_not_of (' ') == test.npos;
    std::cout << __FUNCTION__ << "> getEnv - test = " << test << std::endl;
    std::cout << __FUNCTION__ << "> getEnv - spacesOnly =" << spacesOnly << std::endl;
  }
  if((tmp==NULL) || (spacesOnly)){
    std::cout << __FUNCTION__ << "> getEnv - no environment variable \"" << env << "\" is set" << std::endl;
    return (const char*)"";
  } else {
    std::string stmp = (std::string)tmp+ "/";
    std::cout << __FUNCTION__ << "> getEnv - environment variable \"" << stmp << "\" is set" << std::endl;
    return stmp;
  }
}
