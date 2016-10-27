#ifndef BDSFIELDLOADER_H
#define BDSFIELDLOADER_H

#include "BDSFieldFormat.hh"
#include "BDSInterpolatorType.hh"

#include "G4String.hh"
#include "G4Transform3D.hh"

class BDSArray1DCoords;
class BDSArray2DCoords;
class BDSArray3DCoords;
class BDSArray4DCoords;
class BDSFieldInfo;
class BDSFieldMag;
class BDSFieldE;
class BDSFieldEM;
class BDSInterpolator1D;
class BDSInterpolator2D;
class BDSInterpolator3D;
class BDSInterpolator4D;

/**
 * @brief A loader for various field map formats.
 *
 * A factory to use the appropriate file loader for different field map formats
 * and constructo into required field including the correct interpolator and possilbe
 * reflections.
 * 
 * @author Laurie Nevay
 */

class BDSFieldLoader
{
public:
  /// Singleton accessor.
  static BDSFieldLoader* Instance();

  ~BDSFieldLoader();

  void DeleteArrays();

  BDSFieldMag* LoadMagField(const BDSFieldInfo& info);
  BDSFieldE*   LoadEField(const BDSFieldInfo& info);
  BDSFieldEM*  LoadEMField(const BDSFieldInfo& info);

private:
  /// Private default constructor as singleton
  BDSFieldLoader();

  /// Singleton instance
  static BDSFieldLoader* instance;

  BDSArray1DCoords* Get1DCached(G4String filePath);
  BDSArray2DCoords* Get2DCached(G4String filePath);
  BDSArray3DCoords* Get3DCached(G4String filePath);
  BDSArray4DCoords* Get4DCached(G4String filePath);

  BDSArray1DCoords* LoadBDSIM1D(G4String filePath);
  BDSArray2DCoords* LoadBDSIM2D(G4String filePath);
  BDSArray3DCoords* LoadBDSIM3D(G4String filePath);
  BDSArray4DCoords* LoadBDSIM4D(G4String filePath);

  /// Create the appropriate 1D interpolator for an array.
  BDSInterpolator1D* CreateInterpolator1D(BDSArray1DCoords*    array,
  					  BDSInterpolatorType  interpolatorType) const;
  
  /// Create the appropriate 2D interpolator for an array.
  BDSInterpolator2D* CreateInterpolator2D(BDSArray2DCoords*    array,
  					  BDSInterpolatorType  interpolatorType) const;

  /// Create the appropriate 3D interpolator for an array.
  BDSInterpolator3D* CreateInterpolator3D(BDSArray3DCoords*    array,
  					  BDSInterpolatorType  interpolatorType) const;

  /// Create the appropriate 4D interpolator for an array.
  BDSInterpolator4D* CreateInterpolator4D(BDSArray4DCoords*    array,
  					  BDSInterpolatorType  interpolatorType) const;

  BDSFieldMag* LoadBDSIM1DB(G4String            filePath,
			    BDSInterpolatorType interpolatorType,
			    G4Transform3D       transform);
  BDSFieldMag* LoadBDSIM2DB(G4String            filePath,
			    BDSInterpolatorType interpolatorType,
			    G4Transform3D       transform);
  BDSFieldMag* LoadBDSIM3DB(G4String            filePath,
			    BDSInterpolatorType interpolatorType,
			    G4Transform3D       transform);
  BDSFieldMag* LoadBDSIM4DB(G4String            filePath,
			    BDSInterpolatorType interpolatorType,
			    G4Transform3D       transform);

  /// Load a 2D poisson superfish B field map.
  BDSFieldMag* LoadPoissonSuperFishB(G4String            filePath,
				     BDSInterpolatorType interpolatorType,
				     G4Transform3D       transform);

  /// Similar to LoadPoissonSuperFishB() but the data below y = x is reflected
  /// and the data relfected from one quadrant to all four at the array level.
  BDSFieldMag* LoadPoissonSuperFishBQuad(G4String            filePath,
					 BDSInterpolatorType interpolatorType,
					 G4Transform3D       transform);

  BDSFieldE* LoadBDSIM1DE(G4String            filePath,
			  BDSInterpolatorType interpolatorType,
			  G4Transform3D       transform);
  BDSFieldE* LoadBDSIM2DE(G4String            filePath,
			  BDSInterpolatorType interpolatorType,
			  G4Transform3D       transform);
  BDSFieldE* LoadBDSIM3DE(G4String            filePath,
			  BDSInterpolatorType interpolatorType,
			  G4Transform3D       transform);
  BDSFieldE* LoadBDSIM4DE(G4String            filePath,
			  BDSInterpolatorType interpolatorType,
			  G4Transform3D       transform);

  BDSFieldEM* LoadBDSIM1DEM(G4String            eFilePath,
			    G4String            bFilePath,
			    BDSInterpolatorType eInterpolatorType,
			    BDSInterpolatorType bInterpolatorType,
			    G4Transform3D       transform);

  BDSFieldEM* LoadBDSIM2DEM(G4String            eFilePath,
			    G4String            bFilePath,
			    BDSInterpolatorType eInterpolatorType,
			    BDSInterpolatorType bInterpolatorType,
			    G4Transform3D       transform);
  
  BDSFieldEM* LoadBDSIM3DEM(G4String            eFilePath,
			    G4String            bFilePath,
			    BDSInterpolatorType eInterpolatorType,
			    BDSInterpolatorType bInterpolatorType,
			    G4Transform3D       transform);

  BDSFieldEM* LoadBDSIM4DEM(G4String            eFilePath,
			    G4String            bFilePath,
			    BDSInterpolatorType eInterpolatorType,
			    BDSInterpolatorType bInterpolatorType,
			    G4Transform3D       transform);

  /// @{ Map of cached field map array.
  std::map<G4String, BDSArray1DCoords*> arrays1d;
  std::map<G4String, BDSArray2DCoords*> arrays2d;
  std::map<G4String, BDSArray3DCoords*> arrays3d;
  std::map<G4String, BDSArray4DCoords*> arrays4d;
  /// @}
};

#endif
