/* BDSIM code.   
*/


#ifndef BDSSpectrVacChamb_h
#define BDSSpectrVacChamb_h 

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include <vector>
#include "G4Box.hh"
#include "G4GenericTrap.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"


class BDSSpectrVacChamb 
{

public:
  BDSSpectrVacChamb(const G4String& name, G4double lengthZ, G4double magStartZ, G4double vacuumEndZ, G4double screenWidth, G4double screenAngle, G4double sizeX, G4double sizeY, G4double thickness);
  ~BDSSpectrVacChamb();

  void Place(G4LogicalVolume* motherVolume);

private:
  BDSSpectrVacChamb();


  void SetParameters(const G4String &name, G4double lengthZ, G4double magStartZ, G4double vacuumEndZ, G4double screenWidth, G4double screenAngle, G4double sizeX, G4double sizeY, G4double thickness);
  G4double _lengthZ;
  G4double _magStartZ; //Z where the vacuum chamber starts to widen
  G4double _vacuumEndZ; //Z where the vacuum chamber screen ends
  G4double _screenWidth;
  G4double _screenAngle;
  G4double _sizeX; //The size of the incoming beam pipe
  G4double _sizeY;
  G4double _thickness;
  G4double _trapLengthZ;
  G4double _trapLengthX;
  G4String _name;
  
  G4LogicalVolume* _upperLogVolTrap;
  G4LogicalVolume* _innerLogVolTrap;
  G4LogicalVolume* _lowerLogVolTrap;
  G4VSolid* _upperSolidTrap;
  G4VSolid* _innerSolidTrap;
  G4VSolid* _lowerSolidTrap;

  G4LogicalVolume* _logVolBox1;
  G4LogicalVolume* _innerLogVolBox1;
  G4VSolid* _solidBox1;
  G4VSolid* _innerSolidBox1;

  G4LogicalVolume* _logVolBox2;
  G4LogicalVolume* _innerLogVolBox2;
  G4VSolid* _solidBox2;
  G4VSolid* _innerSolidBox2;

  G4LogicalVolume* _logVolBox3;
  G4LogicalVolume* _innerLogVolBox3;
  G4VSolid* _solidBox3;
  G4VSolid* _innerSolidBox3;

  G4LogicalVolume* _logVolSideWall;
  G4VSolid* _sideWallSolid;
  G4RotationMatrix* _rotSideWall;
  G4ThreeVector _transSideWall;
  G4double _sideWallLength;

  void Build();
  void BuildBox(); 
  void BuildBox1(); 
  void BuildBox2(); 
  void BuildBox3(); 
  void BuildTrap(); 
  void BuildSideWall(); 
  void BuildBoxInner(); 
  void BuildBox1Inner(); 
  void BuildBox2Inner(); 
  void BuildBox3Inner(); 
  void CalculateGeometry();
  void printGeom();
  void printTrapVertices(std::vector<G4TwoVector> vertices, const G4String& name);

  G4VSolid* _innerTrapSolid;
  std::vector<G4TwoVector> _innerTrapVertices;
  G4RotationMatrix* _innerTrapRotation;
  G4ThreeVector _innerTrapTranslation;

  G4GenericTrap* _upperTrapSolid;
  std::vector<G4TwoVector> _upperTrapVertices;
  G4RotationMatrix* _upperTrapRotation;
  G4ThreeVector _upperTrapTranslation;

  G4GenericTrap* _lowerTrapSolid;
  G4RotationMatrix* _lowerTrapRotation;
  G4ThreeVector _lowerTrapTranslation;

  G4double _box1LengthZ;
  G4double _box2LengthZ;
  G4double _box3LengthZ;
  G4Box* _box1SolidInner;
  G4RotationMatrix* _box1RotationInner;
  G4ThreeVector _box1TranslationInner;
  G4Box* _box1Solid;
  G4RotationMatrix* _box1Rotation;
  G4ThreeVector _box1Translation;

  G4Box* _box2SolidInner;
  G4RotationMatrix* _box2RotationInner;
  G4ThreeVector _box2TranslationInner;
  G4Box* _box2Solid;
  G4RotationMatrix* _box2Rotation;
  G4ThreeVector _box2Translation;

  G4Box* _box3SolidInner;
  G4RotationMatrix* _box3RotationInner;
  G4ThreeVector _box3TranslationInner;
  G4Box* _box3Solid;
  G4RotationMatrix* _box3Rotation;
  G4ThreeVector _box3Translation;

  G4double _screenAngle2;
};

#endif

