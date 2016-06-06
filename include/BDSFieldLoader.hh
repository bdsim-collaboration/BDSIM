#ifndef BDSFIELDLOADER_H
#define BDSFIELDLOADER_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"

class BDSFieldMag;
class BDSFieldE;
class BDSFieldEM;

/**
 * @brief A loader for various field map formats.
 * 
 * @author Laurie Nevay
 */

class BDSFieldLoader
{
public:
  /// Singleton accessor.
  static BDSFieldLoader* Instance();

  ~BDSFieldLoader();

  BDSFieldMag* LoadMagField(const BDSFieldInfo& info);
  BDSFieldMag* LoadMagField(G4String filePath, BDSFieldFormat format);
  BDSFieldE*   LoadEField  (G4String filePath, BDSFieldFormat format);
  BDSFieldEM*  LoadEMField (G4String filePath, BDSFieldFormat format);

private:
  /// Private default constructor as singleton
  BDSFieldLoader();

  /// Singleton instance
  static BDSFieldLoader* instance;

  BDSFieldMag* LoadBDSIM2D(G4String filepath);
  BDSFieldMag* LoadBDSIM3D(G4String filepath);
  
};

#endif
