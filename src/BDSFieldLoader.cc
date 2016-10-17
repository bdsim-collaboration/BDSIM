#include "BDSArray2DCoordsRQuad.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldLoaderPoisson.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DLinear.hh"
#include "BDSInterpolator2DNearest.hh"
#include "BDSInterpolatorType.hh"
#include "BDSThreeVector.hh"

#include "globals.hh" // geant4 types / globals

#include <algorithm>
#include <fstream>

class BDSArray2DCoords;

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
  delete instance;
  instance = nullptr;
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
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2D(filePath); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3D(filePath); break;}
    case BDSFieldFormat::poisson2d:
      {result = LoadPoissonSuperFishB(filePath, interpolatorType, transform); break;}
    case BDSFieldFormat::poisson2dquad:
      {result = LoadPoissonSuperFishBQuad(filePath, interpolatorType, transform); break;}
    default:
      break;
    }
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
  if ((array->XStep() - array->YStep()) > 1e-9)
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

BDSInterpolator2D* BDSFieldLoader::CreateInterpolator2D(BDSArray2DCoords*   array,
							BDSInterpolatorType interpolatorType)
{
  BDSInterpolator2D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest2d:
      {result = new BDSInterpolator2DNearest(array); break;}
    case BDSInterpolatorType::linear2d:
      {result = new BDSInterpolator2DLinear(array); break;}
    default:
      {
	G4cout << "Invalid interpolator type " << interpolatorType << G4endl;
	exit(1);
	break;
      }
    }
  return result;
}

BDSFieldE* BDSFieldLoader::LoadEField(G4String /*filePath*/, BDSFieldFormat /*format*/)
{
  return nullptr;
}

BDSFieldEM* BDSFieldLoader::LoadEMField(G4String /*filePath*/, BDSFieldFormat /*format*/)
{
  return nullptr;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM2D(G4String /*filePath*/)
{
  return nullptr;
}

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

  /*
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
  */
  file.close();

  /*
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
  */
  return nullptr;
}
