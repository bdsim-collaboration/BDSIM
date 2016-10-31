#include "BDSArray1DCoords.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray4DCoords.hh"
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
#include "BDSThreeVector.hh"

#include "globals.hh" // geant4 types / globals

#include <algorithm>
#include <fstream>

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
  BDSFieldFormat              format   = info.MagneticFormat();
  BDSInterpolatorType interpolatorType = info.MagneticInterpolatorType();
  G4Transform3D              transform = info.Transform();
  
  BDSFieldMag* result = nullptr;
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::poisson2d:
      {result = LoadPoissonSuperFishB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::poisson2dquad:
      {result = LoadPoissonSuperFishBQuad(filePath, interpolatorType, transform); break;}
    default:
      break;
    }
  return result;
}

BDSFieldE* BDSFieldLoader::LoadEField(const BDSFieldInfo& info)
{
  G4String                    filePath = info.ElectricFile();
  BDSFieldFormat              format   = info.ElectricFormat();
  BDSInterpolatorType interpolatorType = info.ElectricInterpolatorType();
  G4Transform3D              transform = info.Transform();
  
  BDSFieldE* result = nullptr;
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DE(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DE(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DE(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DE(filePath, interpolatorType, transform); break;}
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
      {result = LoadBDSIM1DEM(eFilePath, bFilePath, eIntType, bIntType, transform); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DEM(eFilePath, bFilePath, eIntType, bIntType, transform); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DEM(eFilePath, bFilePath, eIntType, bIntType, transform); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DEM(eFilePath, bFilePath, eIntType, bIntType, transform); break;}
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

BDSArray1DCoords* BDSFieldLoader::LoadBDSIM1D(G4String filePath)
{
  BDSArray1DCoords* cached = Get1DCached(filePath);
  if (cached)
    {return cached;}

  BDSFieldLoaderBDSIM loader;
  BDSArray1DCoords* result = loader.Load1D(filePath);
  arrays1d[filePath] = result;
  return result;
}

BDSArray2DCoords* BDSFieldLoader::LoadBDSIM2D(G4String filePath)
{
  BDSArray2DCoords* cached = Get2DCached(filePath);
  if (cached)
    {return cached;}
  
  BDSFieldLoaderBDSIM loader;
  BDSArray2DCoords* result = loader.Load2D(filePath);
  arrays2d[filePath] = result;
  return result;
}

BDSArray3DCoords* BDSFieldLoader::LoadBDSIM3D(G4String filePath)
{
  BDSArray3DCoords* cached = Get3DCached(filePath);
  if (cached)
    {return cached;}
  
  BDSFieldLoaderBDSIM loader;
  BDSArray3DCoords* result = loader.Load3D(filePath);
  arrays3d[filePath] = result;
  return result;
}

BDSArray4DCoords* BDSFieldLoader::LoadBDSIM4D(G4String filePath)
{
  BDSArray4DCoords* cached = Get4DCached(filePath);
  if (cached)
    {return cached;}
  
  BDSFieldLoaderBDSIM loader;
  BDSArray4DCoords* result = loader.Load4D(filePath);
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
					  G4Transform3D       transform)
{
  BDSArray1DCoords* array = LoadBDSIM1D(filePath);
  BDSInterpolator1D*   ar = CreateInterpolator1D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated1D(ar, transform);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM2DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform)
{
  BDSArray2DCoords* array = LoadBDSIM2D(filePath);
  BDSInterpolator2D*   ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated2D(ar, transform);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM3DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform)
{
  BDSArray3DCoords* array = LoadBDSIM3D(filePath);
  BDSInterpolator3D*   ar = CreateInterpolator3D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated3D(ar, transform);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM4DB(G4String            filePath,
					  BDSInterpolatorType interpolatorType,
					  G4Transform3D       transform)
{
  BDSArray4DCoords* array = LoadBDSIM4D(filePath);
  BDSInterpolator4D*   ar = CreateInterpolator4D(array, interpolatorType);
  BDSFieldMag*     result = new BDSFieldMagInterpolated4D(ar, transform);
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadPoissonSuperFishB(G4String            filePath,
						   BDSInterpolatorType interpolatorType,
						   G4Transform3D       transform)
{
  BDSFieldLoaderPoisson* loader = new BDSFieldLoaderPoisson();
  BDSArray2DCoords*       array = loader->LoadMag2D(filePath);
  BDSInterpolator2D*         ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldMag*           result = new BDSFieldMagInterpolated2D(ar, transform);
  delete loader;
  return result;
}

BDSFieldMag* BDSFieldLoader::LoadPoissonSuperFishBQuad(G4String            filePath,
						       BDSInterpolatorType interpolatorType,
						       G4Transform3D       transform)
{
  BDSFieldLoaderPoisson* loader = new BDSFieldLoaderPoisson();
  BDSArray2DCoords*       array = loader->LoadMag2D(filePath);
  if (std::abs(array->XStep() - array->YStep()) > 1e-9)
    {
      G4cerr << G4endl
	     << "Warning - asymmetric grid spacing for reflected quadrupole will result in"
	     << " a distorted field map - please regenerate the map with even spatial samples."
	     << G4endl;
    }
  BDSArray2DCoordsRQuad* rArray = new BDSArray2DCoordsRQuad(array);
  BDSInterpolator2D*         ar = CreateInterpolator2D(rArray, interpolatorType);
  BDSFieldMag*           result = new BDSFieldMagInterpolated2D(ar, transform);
  delete loader;
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM1DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform)
{
  BDSArray1DCoords* array = LoadBDSIM1D(filePath);
  BDSInterpolator1D*   ar = CreateInterpolator1D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated1D(ar, transform);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM2DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform)
{
  BDSArray2DCoords* array = LoadBDSIM2D(filePath);
  BDSInterpolator2D*   ar = CreateInterpolator2D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated2D(ar, transform);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM3DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform)
{
  BDSArray3DCoords* array = LoadBDSIM3D(filePath);
  BDSInterpolator3D*   ar = CreateInterpolator3D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated3D(ar, transform);
  return result;
}

BDSFieldE* BDSFieldLoader::LoadBDSIM4DE(G4String            filePath,
					BDSInterpolatorType interpolatorType,
					G4Transform3D       transform)
{
  BDSArray4DCoords* array = LoadBDSIM4D(filePath);
  BDSInterpolator4D*   ar = CreateInterpolator4D(array, interpolatorType);
  BDSFieldE*       result = new BDSFieldEInterpolated4D(ar, transform);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM1DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform)
{
  BDSArray1DCoords* eArray = LoadBDSIM1D(eFilePath);
  BDSArray1DCoords* bArray = LoadBDSIM1D(bFilePath);
  BDSInterpolator1D*  eInt = CreateInterpolator1D(eArray, eInterpolatorType);
  BDSInterpolator1D*  bInt = CreateInterpolator1D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated1D(eInt, bInt, transform);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM3DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform)
{
  BDSArray2DCoords* eArray = LoadBDSIM2D(eFilePath);
  BDSArray2DCoords* bArray = LoadBDSIM2D(bFilePath);
  BDSInterpolator2D*  eInt = CreateInterpolator2D(eArray, eInterpolatorType);
  BDSInterpolator2D*  bInt = CreateInterpolator2D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated2D(eInt, bInt, transform);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM2DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform)
{
  BDSArray3DCoords* eArray = LoadBDSIM3D(eFilePath);
  BDSArray3DCoords* bArray = LoadBDSIM3D(bFilePath);
  BDSInterpolator3D*  eInt = CreateInterpolator3D(eArray, eInterpolatorType);
  BDSInterpolator3D*  bInt = CreateInterpolator3D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated3D(eInt, bInt, transform);
  return result;
}

BDSFieldEM* BDSFieldLoader::LoadBDSIM4DEM(G4String            eFilePath,
					  G4String            bFilePath,
					  BDSInterpolatorType eInterpolatorType,
					  BDSInterpolatorType bInterpolatorType,
					  G4Transform3D       transform)
{
  BDSArray4DCoords* eArray = LoadBDSIM4D(eFilePath);
  BDSArray4DCoords* bArray = LoadBDSIM4D(bFilePath);
  BDSInterpolator4D*  eInt = CreateInterpolator4D(eArray, eInterpolatorType);
  BDSInterpolator4D*  bInt = CreateInterpolator4D(bArray, bInterpolatorType);
  BDSFieldEM*       result = new BDSFieldEMInterpolated4D(eInt, bInt, transform);
  return result;
}

/*
BDSFieldMag* BDSFieldLoader::LoadBDSIM3D(G4String filePath)
{
  //G4double lenUnit   = CLHEP::cm;
  //G4double fieldUnit = CLHEP::tesla; 
  
  G4cout << "Reading field map from " << filePath << G4endl; 

  // Open the file for reading.
  std::ifstream file;
  file.open(filePath);
  if(!file)
    {G4cout << "Unable to load field map file: " << filePath << G4endl; exit(1);}
  
  // Ignore first blank line
  char buffer[256];
  file.getline(buffer,256);
  
  // Read table dimensions
  G4int nx, ny, nz;
  file >> nx >> ny >> nz; // Note dodgy order

#ifdef BDSDEBUG
  G4cout << "  [ Number of values x,y,z: " 
	 << nx << " " << ny << " " << nz << " ] "
	 << G4endl;
#endif

  // Set up storage space for table
  xField.resize( nx );
  yField.resize( nx );
  zField.resize( nx );
  int ix, iy, iz;
  for (ix=0; ix<nx; ix++) {
    xField[ix].resize(ny);
    yField[ix].resize(ny);
    zField[ix].resize(ny);
    for (iy=0; iy<ny; iy++) {
      xField[ix][iy].resize(nz);
      yField[ix][iy].resize(nz);
      zField[ix][iy].resize(nz);
    }
  }
  
  // Ignore other header information    
  // The first line whose second character is '0' is considered to
  // be the last line of the header.
  do {
    file.getline(buffer,256);
  } while ( buffer[1]!='0');
  
  //Set up progress display
  // 
  double ndis = static_cast<double>(nx);
  BDSProgressBar* progDis = new BDSProgressBar(ndis);
  //  boost::progress_display* progDis = new boost::progress_display(ndis,std::cout);
  double inc = 1;
  // Read in the data
  double xval=0.0,yval=0.0,zval=0.0,bx,by,bz;
  //  double permeability; // Not used in this example.
  for (ix=0; ix<nx; ix++) {
    //    G4cout << progDis->count() << " " << progDis->maxCount() << G4endl;
     for (iy=0; iy<ny; iy++) {
      for (iz=0; iz<nz; iz++) {
        file >> xval >> yval >> zval >> bx >> by >> bz;
#ifdef BDSDEBUG
	G4cout << "Read: " << xval << " " << yval << " " << zval << " " << bx << " " << by << " " << bz << G4endl;
#endif
        if ( ix==0 && iy==0 && iz==0 ) {
          minx = xval * _lenUnit + fXoffset;
          miny = yval * _lenUnit + fYoffset;
          minz = zval * _lenUnit + fZoffset;
        }
        xField[ix][iy][iz] = bx * _fieldUnit;
        yField[ix][iy][iz] = by * _fieldUnit;
        zField[ix][iy][iz] = bz * _fieldUnit;
      }
    }
     progDis->increment(inc);
  }
  
  //file.close();
  
  maxx = xval * _lenUnit + fXoffset;
  maxy = yval * _lenUnit + fYoffset;
  maxz = zval * _lenUnit + fZoffset;

  G4cout << "\n ---> ... done reading " << G4endl;

  // G4cout << " Read values of field from file " << filePath << G4endl; 
#ifdef BDSDEBUG
  G4cout << " ---> assumed the order:  x, y, z, Bx, By, Bz "
	 << "\n ---> Min values x,y,z: " 
	 << minx/CLHEP::cm << " " << miny/CLHEP::cm << " " << minz/CLHEP::cm << " cm "
	 << "\n ---> Max values x,y,z: " 
	 << maxx/CLHEP::cm << " " << maxy/CLHEP::cm << " " << maxz/CLHEP::cm << " cm "
	 << "\n ---> The field will be offset by " << fZoffset/CLHEP::cm << " cm in z" 
	 << "\n ---> The field will be offset by " << fXoffset/CLHEP::cm << " cm in x" << G4endl;
#endif
  // Should really check that the limits are not the wrong way around.
  if (maxx < minx) {std::swap(maxx,minx); invertX = true;} 
  if (maxy < miny) {std::swap(maxy,miny); invertY = true;} 
  if (maxz < minz) {std::swap(maxz,minz); invertZ = true;} 
  
  dx = maxx - minx;
  dy = maxy - miny;
  dz = maxz - minz;

  return nullptr;
}
*/
