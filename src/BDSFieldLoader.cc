#include "BDSArray1DCoords.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray4DCoords.hh"
#include "BDSArray2DCoordsRDipole.hh"
#include "BDSArray2DCoordsRQuad.hh"
#include "BDSDebug.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldLoaderBDSIM.hh"
#include "BDSFieldLoaderPoisson.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated1D.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldMagInterpolated3D.hh"
#include "BDSFieldMagInterpolated4D.hh"
#include "BDSFieldEInterpolated1D.hh"
#include "BDSFieldEInterpolated2D.hh"
#include "BDSFieldEInterpolated3D.hh"
#include "BDSFieldEInterpolated4D.hh"
#include "BDSFieldEMInterpolated1D.hh"
#include "BDSFieldEMInterpolated2D.hh"
#include "BDSFieldEMInterpolated3D.hh"
#include "BDSFieldEMInterpolated4D.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"
#include "BDSInterpolator1DCubic.hh"
#include "BDSInterpolator1DLinear.hh"
#include "BDSInterpolator1DNearest.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolator2DCubic.hh"
#include "BDSInterpolator2DLinear.hh"
#include "BDSInterpolator2DNearest.hh"
#include "BDSInterpolator3D.hh"
#include "BDSInterpolator3DCubic.hh"
#include "BDSInterpolator3DLinear.hh"
#include "BDSInterpolator3DNearest.hh"
#include "BDSInterpolator4D.hh"
#include "BDSInterpolator4DCubic.hh"
#include "BDSInterpolator4DLinear.hh"
#include "BDSInterpolator4DNearest.hh"
#include "BDSInterpolatorType.hh"

//new code
#include "BDSFieldMagGradient.hh"

#include "globals.hh" // geant4 types / globals

#include <algorithm>
#include <fstream>

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

BDSFieldLoader* BDSFieldLoader::instance = nullptr;

BDSFieldLoader* BDSFieldLoader::Instance()
{
  if (!instance)
    {instance = new BDSFieldLoader();}
  return instance;
}

BDSFieldLoader::BDSFieldLoader()
{;}

BDSFieldLoader::~BDSFieldLoader()
{
  DeleteArrays();
  instance = nullptr;
}

void BDSFieldLoader::DeleteArrays()
{
  for (auto a : arrays1d)
    {delete a.second;}
  for (auto a : arrays2d)
    {delete a.second;}
  for (auto a : arrays3d)
    {delete a.second;}
  for (auto a : arrays4d)
    {delete a.second;}
}

BDSFieldMag* BDSFieldLoader::LoadMagField(const BDSFieldInfo& info)
{
  G4String                    filePath = info.MagneticFile();
  BDSFieldFormat                format = info.MagneticFormat();
  BDSInterpolatorType interpolatorType = info.MagneticInterpolatorType();
  G4Transform3D              transform = info.Transform();
  G4double                    bScaling = info.BScaling();
  
  BDSFieldMag* result = nullptr;
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DB(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DB(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DB(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DB(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::poisson2d:
      {result = LoadPoissonSuperFishB(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::poisson2dquad:
      {result = LoadPoissonSuperFishBQuad(filePath, interpolatorType, transform, bScaling); break;}
    case BDSFieldFormat::poisson2ddipole:
      {result = LoadPoissonSuperFishBDipole(filePath, interpolatorType, transform, bScaling); break;}
    default:
      break;
    }
  return result;
}

BDSFieldE* BDSFieldLoader::LoadEField(const BDSFieldInfo& info)
{
  G4String                    filePath = info.ElectricFile();
  BDSFieldFormat                format = info.ElectricFormat();
  BDSInterpolatorType interpolatorType = info.ElectricInterpolatorType();
  G4Transform3D              transform = info.Transform();
  G4double                    eScaling = info.EScaling();
  
  BDSFieldE* result = nullptr;
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DE(filePath, interpolatorType, transform, eScaling); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DE(filePath, interpolatorType, transform, eScaling); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DE(filePath, interpolatorType, transform, eScaling); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DE(filePath, interpolatorType, transform, eScaling); break;}
    default:
      break;
    }
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadEMField(const BDSFieldInfo& info)
{
  G4String           eFilePath = info.ElectricFile();
  G4String           bFilePath = info.MagneticFile();
  BDSFieldFormat     eFormat   = info.ElectricFormat();
  BDSFieldFormat     bFormat   = info.MagneticFormat();
  BDSInterpolatorType eIntType = info.ElectricInterpolatorType();
  BDSInterpolatorType bIntType = info.MagneticInterpolatorType();
  G4Transform3D      transform = info.Transform();
  G4double            eScaling = info.EScaling();
  G4double            bScaling = info.BScaling();

  // As the different dimension interpolators don't inherit each other, it's very
  // very hard to make a compact polymorphic construction routine here.  In future,
  // if we use delayed construction with setters, we could piece together the BDSFieldEM
  // one bit at a time. This is more an issue with the number of dimensions than anything.
  if (bFormat != eFormat)
    {
      G4cerr << __METHOD_NAME__ << "different formats for E and B fields are not currently "
	    << "supported for an EM field " << G4endl;
      exit(1);
    }
  
  BDSFieldEM* result = nullptr;
  switch (eFormat.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DEM(eFilePath, bFilePath, eIntType, bIntType, transform, eScaling, bScaling); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DEM(eFilePath, bFilePath, eIntType, bIntType, transform, eScaling, bScaling); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DEM(eFilePath, bFilePath, eIntType, bIntType, transform, eScaling, bScaling); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DEM(eFilePath, bFilePath, eIntType, bIntType, transform, eScaling, bScaling); break;}
    default:
      break;
    }
  return result;  
}

BDSArray1DCoords* BDSFieldLoader::Get1DCached(G4String filePath)
{
  auto result = arrays1d.find(filePath);
  if (result != arrays1d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray2DCoords* BDSFieldLoader::Get2DCached(G4String filePath)
{
  auto result = arrays2d.find(filePath);
  if (result != arrays2d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray3DCoords* BDSFieldLoader::Get3DCached(G4String filePath)
{
  auto result = arrays3d.find(filePath);
  if (result != arrays3d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray4DCoords* BDSFieldLoader::Get4DCached(G4String filePath)
{
  auto result = arrays4d.find(filePath);
  if (result != arrays4d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray2DCoords* BDSFieldLoader::LoadPoissonMag2D(G4String filePath)
{
  BDSArray2DCoords* cached = Get2DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray2DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderPoisson<igzstream> loader;
      result = loader.LoadMag2D(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSFieldLoaderPoisson<std::ifstream> loader;
      result = loader.LoadMag2D(filePath);
    }
  arrays2d[filePath] = result;
  return result;  
}

BDSArray1DCoords* BDSFieldLoader::LoadBDSIM1D(G4String filePath)
{
  BDSArray1DCoords* cached = Get1DCached(filePath);
  if (cached)
    {return cached;}

  // Don't want to template this class and there's no base class pointer
  // for BDSFieldLoader so unfortunately, there's a wee bit of repetition.
  BDSArray1DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load1D(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load1D(filePath);
    }
  arrays1d[filePath] = result;
  return result;
}

BDSArray2DCoords* BDSFieldLoader::LoadBDSIM2D(G4String filePath)
{
  BDSArray2DCoords* cached = Get2DCached(filePath);
  if (cached)
    {return cached;}
  
  BDSArray2DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load2D(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load2D(filePath);
    }
  arrays2d[filePath] = result;
  return result;
}

BDSArray3DCoords* BDSFieldLoader::LoadBDSIM3D(G4String filePath)
{
  BDSArray3DCoords* cached = Get3DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray3DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos )
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load3D(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load3D(filePath);
}
  arrays3d[filePath] = result;
  return result;
}

BDSArray4DCoords* BDSFieldLoader::LoadBDSIM4D(G4String filePath)
{
  BDSArray4DCoords* cached = Get4DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray4DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load4D(filePath);
#else
      G4cout << "Compressed file loading - but BDSIM not compiled with ZLIB." << G4endl; exit(1);
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load4D(filePath);
    }
  arrays4d[filePath] = result;
  return result;
}

BDSInterpolator1D* BDSFieldLoader::CreateInterpolator1D(BDSArray1DCoords*   array,
							BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator1D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest1d:
      {result = new BDSInterpolator1DNearest(array); break;}
    case BDSInterpolatorType::linear1d:
      {result = new BDSInterpolator1DLinear(array); break;}
    case BDSInterpolatorType::cubic1d:
      {result = new BDSInterpolator1DCubic(array); break;}
    default:
      {
	G4cout << "Invalid interpolator type for 1D field: " << interpolatorType << G4endl;
	exit(1);
	break;
      }
    }
  return result;
}

BDSInterpolator2D* BDSFieldLoader::CreateInterpolator2D(BDSArray2DCoords*   array,
							BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator2D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest2d:
      {result = new BDSInterpolator2DNearest(array); break;}
    case BDSInterpolatorType::linear2d:
      {result = new BDSInterpolator2DLinear(array); break;}
    case BDSInterpolatorType::cubic2d:
      {result = new BDSInterpolator2DCubic(array); break;}
    default:
      {
	G4cout << "Invalid interpolator type for 2D field: " << interpolatorType << G4endl;
	exit(1);
	break;
      }
    }
  return result;
}

BDSInterpolator3D* BDSFieldLoader::CreateInterpolator3D(BDSArray3DCoords*   array,
							BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator3D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest3d:
      {result = new BDSInterpolator3DNearest(array); break;}
    case BDSInterpolatorType::linear3d:
      {result = new BDSInterpolator3DLinear(array); break;}
    case BDSInterpolatorType::cubic3d:
      {result = new BDSInterpolator3DCubic(array); break;}
    default:
      {
	G4cout << "Invalid interpolator type for 3D field: " << interpolatorType << G4endl;
	exit(1);
	break;
      }
    }
  return result;
}

BDSInterpolator4D* BDSFieldLoader::CreateInterpolator4D(BDSArray4DCoords*   array,
							BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator4D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest4d:
      {result = new BDSInterpolator4DNearest(array); break;}
    case BDSInterpolatorType::linear4d:
      {result = new BDSInterpolator4DLinear(array); break;}
    case BDSInterpolatorType::cubic4d:
      {result = new BDSInterpolator4DCubic(array); break;}
    default:
      {
	G4cout << "Invalid interpolator type for 4D field: " << interpolatorType << G4endl;
	exit(1);
	break;
      }
    }
  return result;	
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM1DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform,
					  G4double            bScaling)

{
  G4double  bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray1DCoords* array = LoadBDSIM1D(filePath);
  BDSInterpolator1D*   ar = CreateInterpolator1D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated1D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM2DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform,
					  G4double            bScaling)
{
  G4double  bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray2DCoords* array = LoadBDSIM2D(filePath);
  BDSInterpolator2D*   ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM3DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform,
					  G4double            bScaling)
{
  G4double  bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray3DCoords* array = LoadBDSIM3D(filePath);
  BDSInterpolator3D*   ar = CreateInterpolator3D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated3D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM4DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform,
					  G4double            bScaling)
{
  G4double  bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray4DCoords* array = LoadBDSIM4D(filePath);
  BDSInterpolator4D*   ar = CreateInterpolator4D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated4D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadPoissonSuperFishB(G4String            filePath,
						   BDSInterpolatorType interpolatorType,
						   G4Transform3D       transform,
						   G4double            bScaling,
						   G4bool              calculateScaling,
						   G4int               maximumOrder)
{
  G4double  bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords* array = LoadPoissonMag2D(filePath);

  BDSMagnetStrength* associatedStrength = nullptr;
  if (calculateScaling)
    {
      BDSInterpolator2D* artemp = CreateInterpolator2D(array, BDSInterpolatorType::cubic2d);
      BDSFieldMagInterpolated2D* tempField = new BDSFieldMagInterpolated2D(artemp, transform, bScalingUnits);
      BDSFieldMagGradient calculator;
      G4cout << "creating calculator" << G4endl;
      associatedStrength = calculator.CalculateMultipoles(tempField, maximumOrder,-4.27);
      delete tempField;
      delete artemp;
    } 
  
  BDSInterpolator2D*   ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadPoissonSuperFishBQuad(G4String            filePath,
						       BDSInterpolatorType interpolatorType,
						       G4Transform3D       transform,
						       G4double            bScaling,
                               G4bool              calculateScaling,
                               G4int               maximumOrder)
{
  G4double  bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords* array = LoadPoissonMag2D(filePath);
  if (std::abs(array->XStep() - array->YStep()) > 1e-9)
    {
      G4cerr << G4endl
	     << "Warning - asymmetric grid spacing for reflected quadrupole will result in"
	     << " a distorted field map - please regenerate the map with even spatial samples."
	     << G4endl;
    }
  BDSArray2DCoordsRQuad* rArray = new BDSArray2DCoordsRQuad(array);
  BDSMagnetStrength* associatedStrength = nullptr;
  if (calculateScaling)
  {
    BDSInterpolator2D* artemp = CreateInterpolator2D(rArray, BDSInterpolatorType::cubic2d);
    BDSFieldMagInterpolated2D* tempField = new BDSFieldMagInterpolated2D(artemp, transform, bScalingUnits);
    BDSFieldMagGradient calculator;
    G4cout << "creating calculator" << G4endl;
    associatedStrength = calculator.CalculateMultipoles(tempField, maximumOrder);
    delete tempField;
    //delete artemp;
  }
  BDSInterpolator2D*         ar = CreateInterpolator2D(rArray, interpolatorType);
  BDSFieldMag*           result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadPoissonSuperFishBDipole(G4String            filePath,
							 BDSInterpolatorType interpolatorType,
							 G4Transform3D       transform,
							 G4double            bScaling)
{
  G4double  bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords* array = LoadPoissonMag2D(filePath);
  
  BDSArray2DCoordsRDipole* rArray = new BDSArray2DCoordsRDipole(array);
  BDSInterpolator2D*           ar = CreateInterpolator2D(rArray, interpolatorType);
  BDSFieldMag*             result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM1DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform,
					G4double            eScaling)
{
  G4double  eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray1DCoords* array = LoadBDSIM1D(filePath);
  BDSInterpolator1D*   ar = CreateInterpolator1D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated1D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM2DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform,
					G4double            eScaling)
{
  G4double  eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray2DCoords* array = LoadBDSIM2D(filePath);
  BDSInterpolator2D*   ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated2D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM3DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform,
					G4double            eScaling)
{
  G4double  eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray3DCoords* array = LoadBDSIM3D(filePath);
  BDSInterpolator3D*   ar = CreateInterpolator3D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated3D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM4DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform,
					G4double            eScaling)
{
  G4double  eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray4DCoords* array = LoadBDSIM4D(filePath);
  BDSInterpolator4D*   ar = CreateInterpolator4D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated4D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM1DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform,
					  G4double            eScaling,
					  G4double            bScaling)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray1DCoords* eArray = LoadBDSIM1D(eFilePath);
  BDSArray1DCoords* bArray = LoadBDSIM1D(bFilePath);
  BDSInterpolator1D*  eInt = CreateInterpolator1D(eArray, eInterpolatorType);
  BDSInterpolator1D*  bInt = CreateInterpolator1D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated1D(eInt, bInt, transform,
							  eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM2DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform,
					  G4double            eScaling,
					  G4double            bScaling)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray2DCoords* eArray = LoadBDSIM2D(eFilePath);
  BDSArray2DCoords* bArray = LoadBDSIM2D(bFilePath);
  BDSInterpolator2D*  eInt = CreateInterpolator2D(eArray, eInterpolatorType);
  BDSInterpolator2D*  bInt = CreateInterpolator2D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated2D(eInt, bInt, transform,
							  eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM3DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform,
					  G4double            eScaling,
					  G4double            bScaling)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray3DCoords* eArray = LoadBDSIM3D(eFilePath);
  BDSArray3DCoords* bArray = LoadBDSIM3D(bFilePath);
  BDSInterpolator3D*  eInt = CreateInterpolator3D(eArray, eInterpolatorType);
  BDSInterpolator3D*  bInt = CreateInterpolator3D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated3D(eInt, bInt, transform,
							  eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM4DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform,
					  G4double            eScaling,
					  G4double            bScaling)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray4DCoords* eArray = LoadBDSIM4D(eFilePath);
  BDSArray4DCoords* bArray = LoadBDSIM4D(bFilePath);
  BDSInterpolator4D*  eInt = CreateInterpolator4D(eArray, eInterpolatorType);
  BDSInterpolator4D*  bInt = CreateInterpolator4D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated4D(eInt, bInt, transform,
							  eScalingUnits, bScalingUnits);
  return result;
}
