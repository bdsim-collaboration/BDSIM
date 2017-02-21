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
#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

template <class T>
BDSFieldLoaderBDSIM<T>::BDSFieldLoaderBDSIM():
  nColumns(0),
  fv(BDSFieldValue()),
  result(nullptr)
{;}

template <class T>
BDSFieldLoaderBDSIM<T>::~BDSFieldLoaderBDSIM()
{;}

template <class T>
void BDSFieldLoaderBDSIM<T>::CleanUp()
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

template <class T>
BDSArray1DCoords* BDSFieldLoaderBDSIM<T>::Load1D(G4String fileName)
{
  Load(fileName,1);
  return static_cast<BDSArray1DCoords*>(result);
}

template <class T>
BDSArray2DCoords* BDSFieldLoaderBDSIM<T>::Load2D(G4String fileName)
{
  Load(fileName,2);
  return static_cast<BDSArray2DCoords*>(result);
}

template <class T>
BDSArray3DCoords* BDSFieldLoaderBDSIM<T>::Load3D(G4String fileName)
{
  Load(fileName,3);
  return static_cast<BDSArray3DCoords*>(result);
}

template <class T>
BDSArray4DCoords* BDSFieldLoaderBDSIM<T>::Load4D(G4String fileName)
{
  Load(fileName,4);
  return result;
}

template <class T>
void BDSFieldLoaderBDSIM<T>::Load(G4String fileName,
				  const unsigned long nDim)
{
  CleanUp();
  
  file.open(fileName);

  // test if file is valid
  bool validFile = false;
#ifdef USE_GZSTREAM
  validFile = file.rdbuf()->is_open();
#else
  validFile = file.is_open();
#endif
  if (!validFile)
    {G4cerr << "Invalid file name or no such file named \"" << fileName << "\"" << G4endl; exit(1);}
  else
    {G4cout << "BDSIM field format - loading \"" << fileName << "\"" << G4endl;}

  // temporary variables
  unsigned long xIndex = 0;
  unsigned long yIndex = 0;
  unsigned long zIndex = 0;
  G4bool intoData     = false;
  G4bool dataFinished = false; // just in case there's empty stuff at the end of the file
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
	  if (dataFinished)
	    {continue;} // just keep skipping lines after the appropriate amount of data is loaded
	  
	  ProcessData(line, xIndex, yIndex, zIndex); // changes member fv
	    
	  // Copy into array - we can always use 4d coords even for lower d arrays
	  // as they all inherit 4d.
	  (*result)(indX,indY,indZ,indT) = fv;

        //Loop order changed: from x, y, z, t to  z, y, x, t.
        indZ++; // increment counter
        if (indZ == nZ)
        {// we've completed one set of z
            indZ = 0; // reset back to 0
            indY++;   // advance y
        }
        if (indY == nY)
        {// we've completed one set of y
            indY = 0; // reset back to 0
            indX++;   // advance x
        }
        if (indX == nX)
        {// we've copmleted one set of x
            indX = 0; // reset back to 0
            indT++;   // advance t
        }
        if (indT == nT)
        {// we've completed one set of t
            indT = 0;
            dataFinished = true;
        }
	  continue;
	}

      // key definition
      if (line.find(">") != std::string::npos)
	{// must be key definition
	  std::regex keyValue("(\\w*)\\s*>\\s*([0-9eE.+-]+)");
	  std::smatch match;
	  std::regex_search(line, match, keyValue);
	  if (match.size() < 2)
	    {G4cerr << "Invalid key definition in field format:\n" << line << G4endl; exit(1);}
	  else
	    {
	      G4String key = G4String(match[1]);
	      key.toLower();
	      G4double value = 0;
	      try
		{value = std::stod(match[2]);}
	      catch (std::invalid_argument)
		{G4cerr << "Invalid argument "    << match[2] << G4endl; exit(1);}
	      catch (std::out_of_range)
		{G4cerr << "Number out of range " << match[2] << G4endl; exit(1);}
		    
	      header[key] = value;
	      continue;
	    }
	}
      
      // if starts with '!' - columns
      // check all required keys have been built up ok
      // set nColumns
      std::regex columnRow("\\s*^!"); // ignore any initial white space and look for '!'
      if (std::regex_search(line, columnRow))
 	{
	  // we only need to record the number of columns and which ones are
	  // the x,y,z field component ones.
	  std::regex afterExclamation("\\s*!\\s*(.+)");
	  std::smatch match;
	  std::regex_search(line, match, afterExclamation);
	  std::string restOfLine = match[1];
	  std::string columnName;
	  std::istringstream restOfLineSS(restOfLine);
	  while (restOfLineSS >> columnName)
	    {
	      nColumns++;
	      if (columnName.find("Fx") != std::string::npos)
		{xIndex = nColumns; continue;}
	      if (columnName.find("Fy") != std::string::npos)
		{yIndex = nColumns; continue;}
	      if (columnName.find("Fz") != std::string::npos)
		{zIndex = nColumns; continue;}

	    }
	  lineData.resize(nColumns+1); // +1 for default value
	  intoData = true;

	  if (nColumns < (nDim + 3)) // 3 for field components
	    {G4cerr << "Too few columns for " << nDim << "D field loading" << G4endl; exit(1);}
	  
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
					      header["tmin"]*CLHEP::s,  header["tmax"]*CLHEP::s);
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

template <class T>
void BDSFieldLoaderBDSIM<T>::ProcessData(const std::string& line,
					 const unsigned long xIndex,
					 const unsigned long yIndex,
					 const unsigned long zIndex)
{
  std::istringstream liness(line);
  G4float value = 0;

  // put a default value in.
  lineData[0] = 0;
  
  // read all columns - indices shifted +1 for default value offset
  for (unsigned long i = 1; i < nColumns+1; ++i)
    {
      liness >> value;
      if (i < xIndex)// x is the first
	{value *= CLHEP::cm;}
      lineData[i] = value;
    }
  
  // Construct field value
  fv = BDSFieldValue(lineData[xIndex],
		     lineData[yIndex],
		     lineData[zIndex]);
}


template class BDSFieldLoaderBDSIM<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSFieldLoaderBDSIM<igzstream>;
#endif
