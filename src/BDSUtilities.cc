#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSRunManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>

#include <signal.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h> // for executable path
#endif


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

G4bool BDS::FileExists(G4String fileName)
{
  std::ifstream infile(fileName.c_str());
  return infile.good();
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

G4String BDS::GetFullPath(G4String fileName, bool excludeNameFromPath)
{
  //Return fullPath of a file:
  //mirror what is done in parser.l (i.e. if no environment varible set, assume base filename path is that of the gmad file).
  // 1) if absolute path (starting with a slash) return that
  // 2) if relative path, then
  // 2a) return path relative to environment variable BDSIMPATH (if set)
  // 2b) return path relative to main BDSIM input gmad file

  // return value
  G4String fullPath;

  // split input into path and filename
  G4String inputFilepath, inputFilename;
  G4String::size_type found = fileName.rfind("/"); // find the last '/'
  if (found != G4String::npos){
    inputFilepath = fileName.substr(0,found); // the path is the bit before that
    inputFilename = fileName.substr(found); // the rest
  } else {
    // no slash, only filename
    inputFilepath = "";
    inputFilename = fileName;
  }
  
  // need to know whether it's an absolute or relative path
  if ((fileName.substr(0,1)) == "/"){
    fullPath = inputFilepath;
  } else {
    // the main file has a relative path or just the file name, add bdsimpath
    fullPath = BDSExecOptions::Instance()->GetBDSIMPATH() + "/" + inputFilepath;
  }
  // add additional slash just to be safe
  fullPath += "/";
  // add filename if not excluded
  if (!excludeNameFromPath) {
    fullPath += inputFilename;
  }
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

G4bool BDS::IsFinite(const G4ThreeVector& variable)
{
  G4bool resultX = BDS::IsFinite(variable.x());
  G4bool resultY = BDS::IsFinite(variable.y());
  G4bool resultZ = BDS::IsFinite(variable.z());
  if (resultX || resultY || resultZ)
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

void BDS::PrintRotationMatrix(G4RotationMatrix* rm, G4String keyName)
{
  G4cout << "Rotation matrix - reference: \"" << keyName << "\"" << *rm << G4endl;
  G4cout << "unit x -> " << G4ThreeVector(1,0,0).transform(*rm) << G4endl;
  G4cout << "unit y -> " << G4ThreeVector(0,1,0).transform(*rm) << G4endl;
  G4cout << "unit z -> " << G4ThreeVector(0,0,1).transform(*rm) << G4endl;
}

G4bool BDS::Geant4EnvironmentIsSet()
{
  std::vector<G4String> variables = {//"G4ABLADATA",
				     "G4NEUTRONHPDATA",
				     "G4RADIOACTIVEDATA",
				     "G4LEDATA",
				     "G4NEUTRONXSDATA",
				     "G4REALSURFACEDATA",
				     "G4LEVELGAMMADATA",
				     "G4PIIDATA",
				     "G4SAIDXSDATA"};

  G4bool result = true;
  for (auto it = variables.begin(); it != variables.end(); ++it)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "testing for variable: " << *it;
#endif
      const char* env_p = std::getenv( (*it).c_str() );
      if (!env_p)
	{
	  result = false;
#ifdef BDSDEBUG
	  G4cout << " - not found" << G4endl;
	}
      else
	{
	  G4cout << " - found" << G4endl;
#endif
	}

    }
  return result;
}
