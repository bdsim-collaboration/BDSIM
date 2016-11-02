#ifndef BDSFIELDLOADERBDSIM_H
#define BDSFIELDLOADERBDSIM_H

#include "BDSFieldValue.hh"

#include "G4String.hh"

#include <map>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

class BDSArray4DCoords;
class BDSArray3DCoords;
class BDSArray2DCoords;
class BDSArray1DCoords;

/**
 * @brief Loader for BDSIM format fields.
 *
 * Loaders are given through specific public methods so the exact
 * type can be returned.
 *
 * This can exit the program if an invalid specification is loaded.
 * 
 * @author Laurie Nevay
 */

template <class T>
class BDSFieldLoaderBDSIM
{
public:
  BDSFieldLoaderBDSIM();
  ~BDSFieldLoaderBDSIM();
  
  BDSArray4DCoords* Load4D(G4String fileName); ///< Load a 4D array.
  BDSArray3DCoords* Load3D(G4String fileName); ///< Load a 3D array.
  BDSArray2DCoords* Load2D(G4String fileName); ///< Load a 2D array.
  BDSArray1DCoords* Load1D(G4String fileName); ///< Load a 1D array.

private:
  /// Ensure any member variables are reset between usages.
  void CleanUp();

  /// General loader for any number of dimensions.
  void Load(G4String fileName,
	    const unsigned long nDim);

  /// Process on line of data. Index of 0 corresponds to a default value of field of 0.
  /// This allows various dimensional loading to use the same function.
  void ProcessData(const std::string& line,
		   const unsigned long xIndex,
		   const unsigned long yIndex = 0,
		   const unsigned long zIndex = 0);

  /// Templated iostream for std::ifstream and gzstream as well
  T file;
  
  /// Number of columns to read.
  unsigned long nColumns;                ///< Number of columns to read.
  std::vector<G4float> lineData; ///< Vector to store line data during loading.
  BDSFieldValue fv;              ///< Temporary field value.
  std::map<G4String, double> header; ///< Header keys and values
  BDSArray4DCoords* result;
};

#endif
