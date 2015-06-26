#include "BDSUtilities.hh"
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <limits>

#include <signal.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h> // for executable path
#endif

#include "BDSRunManager.hh"
#include "parser/getEnv.h"

G4bool BDS::non_alpha::operator()(char c)
{
  return !isalpha(c);
}

G4String BDS::PrepareSafeName(G4String name)
{
  //remove white space
  name.erase(std::remove_if(name.begin(),name.end(),isspace),name.end());
  //remove non alpha numeric characters
  std::replace_if(name.begin(),name.end(),BDS::non_alpha(),'_');
  
  return name;
}

G4int BDS::CalculateOrientation(G4double angle)
{
  G4int orientation;
  if (angle < 0)
    {orientation = 1;}
  else
    {orientation = -1;}
  return orientation;
}

std::string BDS::GetBDSIMExecPath()
{
  // get path to executable (platform dependent)
  char path[1024];
#ifdef __linux__
  // get path from /proc/self/exe
  ssize_t len = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1) {
    path[len] = '\0';
  }
#elif __APPLE__
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0)
    std::cout << "buffer too small; need size " << size << std::endl;
#endif
  std::string bdsimPath(path);
  // remove executable from path
  std::string::size_type found = bdsimPath.rfind("/"); // find the last '/'
  if (found != std::string::npos){
    bdsimPath = bdsimPath.substr(0,found+1); // the path is the bit before that, including the '/'
  }
  return bdsimPath;
}

G4String BDS::GetFullPath(G4String fileName, bool excludePathFromFileName)
{
  //Set fullPath to mirror what is done in parser.l (i.e. if no environment varible set, assume base filename path is that of the gmad file).
  G4String fullPath = getEnv("BDSIMPATH");
  if(fullPath.length()<=0){
    G4String inputFilepath = "";
    if (!excludePathFromFileName) {
      // get the path part of the supplied path to the main input file
      G4String::size_type found = fileName.rfind("/"); // find the last '/'
      if (found != G4String::npos){
	inputFilepath = fileName.substr(0,found); // the path is the bit before that
      } // else remains empty string
    }
    // need to know whether it's an absolute or relative path
    if ((fileName.substr(0,1)) == "/"){
      // the main file has an absolute path
      fullPath = inputFilepath;
    } else {
      // the main file has a relative path or just the file name
      char cwdchars[200]; //filepath up to 200 characters
      // get current working directory
      G4String cwd = (G4String)getcwd(cwdchars, sizeof(cwdchars)) + "/";
      fullPath = cwd + inputFilepath;
    }
  }
  // add additional slash just to be safe
  fullPath += "/";
  return fullPath;
}

void BDS::HandleAborts(int signal_number)
{
  /** 
      Try to catch abort signals. This is not guaranteed to work.
      Main goal is to close output stream / files.
  */
  // prevent recursive calling
  static int nrOfCalls=0;
  if (nrOfCalls>0) exit(1);
  nrOfCalls++;
  std::cout << "BDSIM is about to crash or was interrupted! " << std::endl;
  std::cout << "With signal: " << strsignal(signal_number) << std::endl;
  std::cout << "Trying to write and close output file" << std::endl;
  std::cout << "Terminate run" << std::endl;
  BDSRunManager::GetRunManager()->RunTermination();
  std::cout << "Ave, Imperator, morituri te salutant!" << std::endl;
}

G4bool BDS::IsFinite(const G4double& variable)
{
  if (std::abs(variable) > std::numeric_limits<double>::epsilon())
    {return true;}
  else
    {return false;}
}

// a robust compiler-invariant method to convert from integer to G4String
G4String BDS::StringFromInt(G4int N)
{
  if (N==0) return "0";
  G4int nLocal=N, nDigit=0, nMax=1;
  do { nDigit++;
      nMax*=10;} while(N > nMax-1);
  nMax/=10;
  G4String Cnum;
  do {Cnum+=BDS::StringFromDigit(nLocal/nMax);
      nLocal-= nLocal/nMax * nMax;
      nMax/=10;}   while(nMax>1);
  if(nMax!=0)Cnum+=BDS::StringFromDigit(nLocal/nMax);
  return Cnum;
}

// a robust compiler-invariant method to convert from digit to G4String
G4String BDS::StringFromDigit(G4int N) 
{
  if(N<0 || N>9)
    G4Exception("Invalid Digit in BDS::StringFromDigit", "-1", FatalException, "");
  G4String Cnum;
  if(N==0)Cnum="0";
  else if(N==1)Cnum="1";
  else if(N==2)Cnum="2";
  else if(N==3)Cnum="3";
  else if(N==4)Cnum="4";
  else if(N==5)Cnum="5";
  else if(N==6)Cnum="6";
  else if(N==7)Cnum="7";
  else if(N==8)Cnum="8";
  else if(N==9)Cnum="9"; 
  return Cnum;
}
