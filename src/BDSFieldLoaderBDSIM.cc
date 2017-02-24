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
				   "zmin", "zmax", "tmin", "tmax", "loopOrder"};
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
				  const unsigned int nDim)
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
    G4int* ind1;
    G4int* ind2;
    G4int* ind3;
    G4int* ind4;
    G4int* n1;
    G4int* n2;
    G4int* n3;
    G4int* n4; //Pointers to the above, to allow different loop orderings.
    G4int loopOrder = 123; // The order in which to loop over the axes.

  while (std::getline(file, line)) {// read a line only if it's not a blank one

      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace)) { continue; }

      // Process a line of field data
      if (intoData) {
          if (dataFinished) { continue; } // just keep skipping lines after the appropriate amount of data is loaded

          ProcessData(line, xIndex, yIndex, zIndex); // changes member fv

          // Copy into array - we can always use 4d coords even for lower d arrays
          // as they all inherit 4d.
          (*result)(indX, indY, indZ, indT) = fv;

          (*ind1)++; // increment counter
          if ((*ind1) == (*n1)) {// we've completed one set of ind1.
              (*ind1) = 0; // reset back to 0
              (*ind2)++;   // advance ind2
          }
          if ((*ind2) == (*n2)) {// we've completed one set of ind2
              (*ind2) = 0; // reset back to 0
              (*ind3)++;   // advance ind3
          }
          if ((*ind3) == (*n3)) {// we've copmleted one set of ind3
              (*ind3) = 0; // reset back to 0
              (*ind4)++;   // advance ind4
          }
          if ((*ind4) == (*n4)) {// we've completed one set of ind4
              (*ind4) = 0;
              dataFinished = true;
          }
          continue;
      }

      // key definition
      if (line.find(">") != std::string::npos) {// must be key definition
          std::regex keyValue("(\\w*)\\s*>\\s*([0-9eE.+-]+)");
          std::smatch match;
          std::regex_search(line, match, keyValue);
          if (match.size() < 2) {
              G4cerr << "Invalid key definition in field format:\n" << line << G4endl;
              exit(1);
          }
          else {
              G4String key = G4String(match[1]);
              key.toLower();
              G4double value = 0;
              try { value = std::stod(match[2]); }
              catch (std::invalid_argument) {
                  G4cerr << "Invalid argument " << match[2] << G4endl;
                  exit(1);
              }
              catch (std::out_of_range) {
                  G4cerr << "Number out of range " << match[2] << G4endl;
                  exit(1);
              }

              header[key] = value;
              continue;
          }
      }

      // if starts with '!' - columns
      // check all required keys have been built up ok
      // set nColumns
      std::regex columnRow("\\s*^!"); // ignore any initial white space and look for '!'
      if (std::regex_search(line, columnRow)) {
          // we only need to record the number of columns and which ones are
          // the x,y,z field component ones.
          std::regex afterExclamation("\\s*!\\s*(.+)");
          std::smatch match;
          std::regex_search(line, match, afterExclamation);
          std::string restOfLine = match[1];
          std::string columnName;
          std::istringstream restOfLineSS(restOfLine);
          while (restOfLineSS >> columnName) {
              nColumns++;
              if (columnName.find("Fx") != std::string::npos) {
                  xIndex = nColumns;
                  continue;
              }
              if (columnName.find("Fy") != std::string::npos) {
                  yIndex = nColumns;
                  continue;
              }
              if (columnName.find("Fz") != std::string::npos) {
                  zIndex = nColumns;
                  continue;
              }

          }
          lineData.resize(nColumns + 1); // +1 for default value
          intoData = true;

          if (nColumns < (nDim + 3)) // 3 for field components
          {
              G4cerr << "Too few columns for " << nDim << "D field loading" << G4endl;
              exit(1);
          }

          // we have all the information now, so initialise the container
          switch (nDim) {
              case 1: {
                  nX = G4int(header["nx"]);
                  result = new BDSArray1DCoords(nX,
                                                header["xmin"] * CLHEP::cm, header["xmax"] * CLHEP::cm);
                  break;
              }
              case 2: {
                  if(header.count("loopOrder")) loopOrder = G4int(header["loopOrder"]); //loopOrder is optional
                  nX = G4int(header["nx"]);
                  nY = G4int(header["ny"]);
                  result = new BDSArray2DCoords(nX, nY, loopOrder,
                                                header["xmin"] * CLHEP::cm, header["xmax"] * CLHEP::cm,
                                                header["ymin"] * CLHEP::cm, header["ymax"] * CLHEP::cm);
                  break;
              }
              case 3: {
                  if(header.count("loopOrder")) loopOrder = G4int(header["loopOrder"]);
                  nX = G4int(header["nx"]);
                  nY = G4int(header["ny"]);
                  nZ = G4int(header["nz"]);
                  result = new BDSArray3DCoords(nX, nY, nZ, loopOrder,
                                                header["xmin"] * CLHEP::cm, header["xmax"] * CLHEP::cm,
                                                header["ymin"] * CLHEP::cm, header["ymax"] * CLHEP::cm,
                                                header["zmin"] * CLHEP::cm, header["zmax"] * CLHEP::cm);
                  break;
              }
              case 4: {
                  if(header.count("loopOrder")) loopOrder = G4int(header["loopOrder"]);
                  nX = G4int(header["nx"]);
                  nY = G4int(header["ny"]);
                  nZ = G4int(header["nz"]);
                  nT = G4int(header["nt"]);
                  result = new BDSArray4DCoords(nX, nY, nZ, nT, loopOrder,
                                                header["xmin"] * CLHEP::cm, header["xmax"] * CLHEP::cm,
                                                header["ymin"] * CLHEP::cm, header["ymax"] * CLHEP::cm,
                                                header["zmin"] * CLHEP::cm, header["zmax"] * CLHEP::cm,
                                                header["tmin"] * CLHEP::s, header["tmax"] * CLHEP::s);
                  break;
              }
              default:
                  break;
          }

          switch(loopOrder) {
              case 1:
                  loopOrder = 123;
                  break;
              case 2:
                  loopOrder = 213;
                  break;
              case 12:
                  loopOrder = 123;
                  break;
              case 21:
                  loopOrder = 213;
                  break;
          }

          }
          switch(loopOrder){
              case 321:
                  ind1 = &indZ;
                  n1 = &nZ;
                  ind2 = &indY;
                  n2 = &nY;
                  ind3 = &indX;
                  n3 = &nX;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              case 312:
                  ind1 = &indZ;
                  n1 = &nZ;
                  ind2 = &indX;
                  n2 = &nX;
                  ind3 = &indY;
                  n3 = &nY;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              case 123:
                  ind1 = &indX;
                  n1 = &nX;
                  ind2 = &indY;
                  n2 = &nY;
                  ind3 = &indZ;
                  n3 = &nZ;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              case 132:
                  ind1 = &indX;
                  n1 = &nX;
                  ind2 = &indZ;
                  n2 = &nZ;
                  ind3 = &indY;
                  n3 = &nY;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              case 213:
                  ind1 = &indY;
                  n1 = &nY;
                  ind2 = &indX;
                  n2 = &nX;
                  ind3 = &indZ;
                  n3 = &nZ;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              case 231:
                  ind1 = &indY;
                  n1 = &nY;
                  ind2 = &indZ;
                  n2 = &nZ;
                  ind3 = &indX;
                  n3 = &nX;
                  ind4 = &indT;
                  n4 = &nT;
                  break;
              default:
                  ind1 = &indZ;
                  n1 = &nZ;
                  ind2 = &indY;
                  n2 = &nY;
                  ind3 = &indX;
                  n3 = &nX;
                  ind4 = &indT;
                  n4 = &nT;
          }
          continue;
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
