#ifndef BDSFIELDLOADER_H
#define BDSFIELDLOADER_H

#include "BDSFieldFormat.hh"
#include "BDSInterpolatorType.hh"

#include "G4String.hh"
#include "G4Transform3D.hh"

class BDSArray2DCoords;
class BDSFieldInfo;
class BDSFieldLoaderBase;
class BDSFieldMag;
class BDSFieldE;
class BDSFieldEM;
class BDSInterpolator2D;

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
  
  BDSFieldE*   LoadEField  (G4String filePath, BDSFieldFormat format);
  BDSFieldEM*  LoadEMField (G4String filePath, BDSFieldFormat format);

private:
  /// Private default constructor as singleton
  BDSFieldLoader();

  /// Singleton instance
  static BDSFieldLoader* instance;

  
  BDSFieldMag* LoadBDSIM2D(G4String filePath);
  BDSFieldMag* LoadBDSIM3D(G4String filePath);
  
  BDSFieldMag* LoadPoissonSuperFishB(G4String            filePath,
				     BDSInterpolatorType interpolatorType,
				     G4Transform3D       transform);

  BDSInterpolator2D* CreateInterpolator2D(BDSArray2DCoords*    array,
  					  BDSInterpolatorType  interpolatorType);
  
};

#endif
