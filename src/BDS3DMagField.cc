//Based on the Geant4 example examples/advanced/purging_magnet/src/PurgMagTabulatedField3D.cc
#include <cmath>
#include <fstream>
#include <iostream>

#include "BDSGlobalConstants.hh"
#include "BDS3DMagField.hh"

BDS3DMagField::BDS3DMagField( const char* filename, double zOffset ) 
  :fZoffset(zOffset),invertX(false),invertY(false),invertZ(false)
{    
  _lenUnit= CLHEP::cm;
  _fieldUnit= CLHEP::tesla; 

  G4cout << "\n-----------------------------------------------------------"
	 << "\n      Magnetic field"
	 << "\n-----------------------------------------------------------";
    
  G4cout << "\n ---> " "Reading the field grid from " << filename << " ... " << G4endl; 
  // Open the file for reading.
  std::ifstream file;
  file.open(filename);
  G4String exceptionString = "Unable to load field map file: " + (G4String)filename;
  if(!file) G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  
  // Ignore first blank line
  char buffer[256];
  file.getline(buffer,256);
  
  // Read table dimensions 
  file >> nx >> ny >> nz; // Note dodgy order

  G4cout << "  [ Number of values x,y,z: " 
	 << nx << " " << ny << " " << nz << " ] "
	 << G4endl;

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
  
  // Read in the data
  double xval=0.0,yval=0.0,zval=0.0,bx,by,bz;
  //  double permeability; // Not used in this example.
  minx = miny = minz = 0.0;
  for (ix=0; ix<nx; ix++) {
    for (iy=0; iy<ny; iy++) {
      for (iz=0; iz<nz; iz++) {
        file >> xval >> yval >> zval >> bx >> by >> bz;
	G4cout << "Read: " << xval << " " << yval << " " << zval << " " << bx << " " << by << " " << bz << G4endl;
        if ( ix==0 && iy==0 && iz==0 ) {
          minx = xval * _lenUnit;
          miny = yval * _lenUnit;
          minz = zval * _lenUnit;
        }
        xField[ix][iy][iz] = bx * _fieldUnit;
        yField[ix][iy][iz] = by * _fieldUnit;
        zField[ix][iy][iz] = bz * _fieldUnit;
      }
    }
  }
  file.close();

  maxx = xval * _lenUnit;
  maxy = yval * _lenUnit;
  maxz = zval * _lenUnit;

  G4cout << "\n ---> ... done reading " << G4endl;

  // G4cout << " Read values of field from file " << filename << G4endl; 
  G4cout << " ---> assumed the order:  x, y, z, Bx, By, Bz "
	 << "\n ---> Min values x,y,z: " 
	 << minx/CLHEP::cm << " " << miny/CLHEP::cm << " " << minz/CLHEP::cm << " cm "
	 << "\n ---> Max values x,y,z: " 
	 << maxx/CLHEP::cm << " " << maxy/CLHEP::cm << " " << maxz/CLHEP::cm << " cm "
	 << "\n ---> The field will be offset by " << zOffset/CLHEP::cm << " cm " << G4endl;

  // Should really check that the limits are not the wrong way around.
  if (maxx < minx) {std::swap(maxx,minx); invertX = true;} 
  if (maxy < miny) {std::swap(maxy,miny); invertY = true;} 
  if (maxz < minz) {std::swap(maxz,minz); invertZ = true;} 
  G4cout << "\nAfter reordering if neccesary"  
	 << "\n ---> Min values x,y,z: " 
	 << minx/CLHEP::cm << " " << miny/CLHEP::cm << " " << minz/CLHEP::cm << " cm "
	 << " \n ---> Max values x,y,z: " 
	 << maxx/CLHEP::cm << " " << maxy/CLHEP::cm << " " << maxz/CLHEP::cm << " cm ";

  dx = maxx - minx;
  dy = maxy - miny;
  dz = maxz - minz;
  G4cout << "\n ---> Dif values x,y,z (range): " 
	 << dx/CLHEP::cm << " " << dy/CLHEP::cm << " " << dz/CLHEP::cm << " cm in z "
	 << "\n-----------------------------------------------------------" << G4endl;
}

void BDS3DMagField::GetFieldValue(const double point[4],
				      double *Bfield ) const
{

  G4ThreeVector local;

  local[0] = point[0] + translation[0];
  local[1] = point[1] + translation[1];
  local[2] = point[2] + translation[2] + fZoffset;

  local *= Rotation();

#ifdef BDSDEBUG
  G4cout << "BDS3DMagField::GetFieldValue" << G4endl;
  G4cout << "point x       = " << point[0]/CLHEP::cm << " cm" << G4endl;
  G4cout << "point y       = " << point[1]/CLHEP::cm << " cm" << G4endl;
  G4cout << "point z       = " << point[2]/CLHEP::cm << " cm" << G4endl;
  G4cout << "translation x = " << translation[0]/CLHEP::cm << " cm" << G4endl;
  G4cout << "translation y = " << translation[1]/CLHEP::cm << " cm" << G4endl;
  G4cout << "translation z = " << translation[2]/CLHEP::cm << " cm" << G4endl;
  G4cout << "fZOffset = " << fZoffset/CLHEP::cm << " cm" << G4endl;
  G4cout << "local x       = " << local[0]/CLHEP::cm << " cm" << G4endl;
  G4cout << "local y       = " << local[1]/CLHEP::cm << " cm" << G4endl;
  G4cout << "local z       = " << local[2]/CLHEP::cm << " cm" << G4endl;
#endif

  double signy=1;
  double signz=1;

  //Mirror in x=0 plane and z=0 plane
  /*
  if( local[0] < 0 ){
#ifdef BDSDEBUG
    G4cout << "x = " << local[0]/cm << " cm. Mirroring in x=0 plane." << G4endl;
#endif
    local[0] *= -1;
    signy = -1;
    signz = -1;
  }

  if( local[2] <0 ){
#ifdef BDSDEBUG
    G4cout << "z = " << local[2]/cm << " cm. Mirroring in z=0 plane." << G4endl;
#endif
    local[2] *= -1;
    signz = -1;
  }
  */

  // Check that the point is within the defined region 
  if ( local[0]>=minx && local[0]<=maxx &&
       local[1]>=miny && local[1]<=maxy && 
       local[2]>=minz && local[2]<=maxz ) {
    
    // Position of given point within region, normalized to the range
    // [0,1]
    double xfraction = (local[0] - minx) / dx;
    double yfraction = (local[1] - miny) / dy; 
    double zfraction = (local[2] - minz) / dz;

    if (invertX) { xfraction = 1 - xfraction;}
    if (invertY) { yfraction = 1 - yfraction;}
    if (invertZ) { zfraction = 1 - zfraction;}

    // Need addresses of these to pass to modf below.
    // modf uses its second argument as an OUTPUT argument.
    double xdindex, ydindex, zdindex;
    
    // Position of the point within the cuboid defined by the
    // nearest surrounding tabulated points
    double xlocal = ( std::modf(xfraction*(nx-1), &xdindex));
    double ylocal = ( std::modf(yfraction*(ny-1), &ydindex));
    double zlocal = ( std::modf(zfraction*(nz-1), &zdindex));
    
    // The indices of the nearest tabulated point whose coordinates
    // are all less than those of the given point
    int xindex = static_cast<int>(xdindex);
    int yindex = static_cast<int>(ydindex);
    int zindex = static_cast<int>(zdindex);
    

#ifdef DEBUG_INTERPOLATING_FIELD
    G4cout << "Local x,y,z: " << xlocal << " " << ylocal << " " << zlocal << G4endl;
    G4cout << "Index x,y,z: " << xindex << " " << yindex << " " << zindex << G4endl;
    double valx0z0, mulx0z0, valx1z0, mulx1z0;
    double valx0z1, mulx0z1, valx1z1, mulx1z1;
    //    valx0z0= table[xindex  ][0][zindex];  mulx0z0=  (1-xlocal) * (1-zlocal);
    //    valx1z0= table[xindex+1][0][zindex];  mulx1z0=   xlocal    * (1-zlocal);
    //    valx0z1= table[xindex  ][0][zindex+1]; mulx0z1= (1-xlocal) * zlocal;
    //    valx1z1= table[xindex+1][0][zindex+1]; mulx1z1=  xlocal    * zlocal;
#endif

        // Full 3-dimensional version
    Bfield[0] =
      xField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      xField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      xField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      xField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      xField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      xField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      xField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      xField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
    Bfield[1] = signy *(
      yField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      yField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      yField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      yField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      yField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      yField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      yField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      yField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal );
    Bfield[2] = signz *(
      zField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      zField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      zField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      zField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      zField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      zField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      zField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      zField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal );

  } else {
    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
  }

#ifdef BDSDEBUG
  G4cout << "Bfield x,y,z = " << 
    Bfield[0]/_fieldUnit << " " <<
    Bfield[1]/_fieldUnit << " " <<
    Bfield[2]/_fieldUnit <<
    G4endl;
#endif
}

void BDS3DMagField::Prepare(G4VPhysicalVolume *referenceVolume){
  const G4RotationMatrix* Rot= referenceVolume->GetFrameRotation();
  const G4ThreeVector Trans=referenceVolume->GetFrameTranslation();
  SetOriginRotation(*Rot);
  SetOriginTranslation(Trans);
}
