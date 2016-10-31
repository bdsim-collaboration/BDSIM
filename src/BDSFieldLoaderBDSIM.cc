#include "BDSArray1DCoords.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray4DCoords.hh"
#include "BDSFieldLoaderBDSIM.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"
#include "G4String.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>


BDSFieldLoaderBDSIM::BDSFieldLoaderBDSIM():
  nColumns(0),
  fv(BDSFieldValue()),
  result(nullptr)
{;}

BDSFieldLoaderBDSIM::~BDSFieldLoaderBDSIM()
{;}

void BDSFieldLoaderBDSIM::CleanUp()
{
  nColumns = 0;
  lineData.clear();
  fv = BDSFieldValue();
  std::vector<G4String> allKeys = {"nx", "ny", "nz", "nt",
				   "xmin", "xmax", "ymin", "ymax",
				   "zmin", "zmax", "tmin", "tmax"};
  for (const std::string s : allKeys)
    {header[s] = 0;}
  result = nullptr;
}

BDSArray1DCoords* BDSFieldLoaderBDSIM::Load1D(G4String fileName)
{
  Load(fileName,1);
  return static_cast<BDSArray1DCoords*>(result);
}

BDSArray2DCoords* BDSFieldLoaderBDSIM::Load2D(G4String fileName)
{
  Load(fileName,2);
  return static_cast<BDSArray2DCoords*>(result);
}

BDSArray3DCoords* BDSFieldLoaderBDSIM::Load3D(G4String fileName)
{
  Load(fileName,3);
  return static_cast<BDSArray3DCoords*>(result);
}

BDSArray4DCoords* BDSFieldLoaderBDSIM::Load4D(G4String fileName)
{
  Load(fileName,4);
  return result;
}

void BDSFieldLoaderBDSIM::Load(G4String fileName,
			       const G4int nDim)
{
  CleanUp();
  
  std::ifstream file;
  file.open(fileName);

  // test if file is valid
  if (!file.is_open())
    {G4cerr << "Invalid file name or no such file named \"" << fileName << "\"" << G4endl; exit(1);}
  else
    {G4cout << "BDSIM field format - loading \"" << fileName << "\"" << G4endl;}

  // temporary variables
  std::vector<G4String> keys;
  G4int xIndex = 0;
  G4int yIndex = 0;
  G4int zIndex = 0;
  G4bool intoData = false;
  std::string line;
  G4int indX = 0; // ind of 0 will be index to default value in line data
  G4int indY = 0;
  G4int indZ = 0;
  G4int indT = 0;
  G4int nX   = 1;
  G4int nY   = 1;
  G4int nZ   = 1;
  G4int nT   = 1;

  while (std::getline(file, line))
    {// read a line only if it's not a blank one

      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
	{continue;}

      // Process a line of field data
      if (intoData)
	{
	  ProcessData(line, xIndex, yIndex, zIndex); // changes member fv
	    
	  // Copy into array - we can always use 4d coords even for lower d arrays
	  // as they all inherite 4d.
	  (*result)(indX,indY,indZ,indT) = fv;
	  
	  indX++; // increment counter
	  if (indX == nX)
	    {// we've completed one set of x
	      indX = 0; // reset back to 0
	      indY++;   // advance y
	    }
	  if (indY == nY)
	    {// we've completed one set of y
	      indY = 0; // reset back to 0
	      indZ++;   // advance z
	    }
	  if (indZ == nZ)
	    {// we've copmleted one set of z
	      indZ = 0; // reset back to 0
	      indT++;   // advance t
	    }     
	  continue;
	}

      // key definition
      if (line.find(">") != std::string::npos)
	{// must be key definition
	  std::regex keyValue("(\\w*)\\s*>\\s*([0-9eE.+-]+)");
	  std::smatch match;
	  std::regex_search(line, match, keyValue);
	  G4String key = G4String(match[1]);
	  key.toLower();
	  header[key] = std::stod(match[2]);
	  continue;
	}
      
      // if starts with '!' - columns
      // check all required keys have been built up ok
      // set nColumns
      std::regex columnRow("\\s*^!"); // ignore any initial white space and look for '!'
      if (std::regex_search(line, columnRow))
 	{
	  // we only need to record the number of columns and which ones are
	  // the x,y,z field component ones.
	  std::regex afterExclamation("\\!(.*)");
	  std::smatch match;
	  std::regex_search(line, match, afterExclamation);
	  std::string restOfLine = match[0];
	  std::string columnName;
	  std::istringstream restOfLineSS(restOfLine);
	  while (restOfLineSS >> columnName)
	    {
	      if (columnName.find("Fx"))
		{xIndex = nColumns;}
	      if (columnName.find("Fy"))
		{yIndex = nColumns;}
	      if (columnName.find("Fz"))
		{zIndex = nColumns;}
	      nColumns++;
	    }
	  lineData.resize(nColumns+1); // +1 for default value
	  intoData = true;
	  
	  // we have all the information now, so initialise the container
	  switch (nDim)
	    {
	    case 1:
	      {
		nX = G4int(header["nx"]);
		result = new BDSArray1DCoords(nX,
					      header["xmin"]*CLHEP::cm, header["xmax"]*CLHEP::cm);
		break;
	      }
	    case 2:
	      {
		nX = G4int(header["nx"]);
		nY = G4int(header["ny"]);
		result = new BDSArray2DCoords(nX, nY,
					      header["xmin"]*CLHEP::cm, header["xmax"]*CLHEP::cm,
					      header["ymin"]*CLHEP::cm, header["ymax"]*CLHEP::cm);
		break;
	      }
	    case 3:
	      {
		nX = G4int(header["nx"]);
		nY = G4int(header["ny"]);
		nZ = G4int(header["nz"]);
		result = new BDSArray3DCoords(nX, nY, nZ,
					      header["xmin"]*CLHEP::cm, header["xmax"]*CLHEP::cm,
					      header["ymin"]*CLHEP::cm, header["ymax"]*CLHEP::cm,
					      header["zmin"]*CLHEP::cm, header["zmax"]*CLHEP::cm);
		break;
	      }
	    case 4:
	      {
		nX = G4int(header["nx"]);
		nY = G4int(header["ny"]);
		nZ = G4int(header["nz"]);
		nT = G4int(header["nt"]);
		result = new BDSArray4DCoords(nX, nY, nZ, nT,
					      header["xmin"]*CLHEP::cm, header["xmax"]*CLHEP::cm,
					      header["ymin"]*CLHEP::cm, header["ymax"]*CLHEP::cm,
					      header["zmin"]*CLHEP::cm, header["zmax"]*CLHEP::cm,
					      header["tmin"]*CLHEP::cm, header["tmax"]*CLHEP::cm);
		break;
	      }
	    default:
	      break;
	    }
	  continue;
	}
    }

  file.close();
}

void BDSFieldLoaderBDSIM::ProcessData(const std::string& line,
				      const G4int xIndex,
				      const G4int yIndex,
				      const G4int zIndex)
{
  std::istringstream liness(line);
  G4double value = 0;

  // put a default value in.
  lineData[0] = 0;
  
  // read all columns - indices shifted +1 for default value offset
  for (G4int i = 1; i < nColumns+1; ++i)
    {
      liness >> value;
      lineData[i] = value;
    }
  
  // Construct field value
  fv = BDSFieldValue(lineData[xIndex]*CLHEP::tesla,
		     lineData[yIndex]*CLHEP::tesla,
		     lineData[zIndex]*CLHEP::tesla);
}
