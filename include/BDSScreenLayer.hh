#ifndef BDSSCREENLAYER_H
#define BDSSCREENLAYER_H 

#include "globals.hh"
#include "G4Colour.hh"

class G4LogicalVolume;
class G4PVPlacement;
class G4VSolid;

/** 
 * @brief
 *
 * @author Lawrence Deacon
 */

class BDSScreenLayer 
{
public:
  BDSScreenLayer(G4ThreeVector size,
		 G4String name,
		 G4String material,
		 G4double grooveWidth,
		 G4double grooveSpatialFrequency);
  ~BDSScreenLayer();
  inline G4LogicalVolume* GetLog(){return log;}
  inline G4String GetName(){return name;}
  inline G4ThreeVector GetSize(){return size;}
  void SetPhys(G4PVPlacement* phys);
  inline G4PVPlacement* GetPhys(){return phys;}
  void SetColour(G4Colour col);
  void backInternalMirror();
  void frontInternalMirror();

  /// Make this plane a sampling plane.
  void sampler(); 

protected:
  /// protected constructor, currently used by AWAKE/BDSMultiFacetLayer
  BDSScreenLayer();
  G4ThreeVector size;
  G4String name;
  G4LogicalVolume* log = nullptr;
  G4PVPlacement* phys  = nullptr;
  G4VSolid* solid = nullptr;

private:
  class InternalMirror{
  public:
    InternalMirror(G4int varside, G4ThreeVector size, G4String material, G4LogicalVolume* motherLog, G4PVPlacement* motherPhys);
    ~InternalMirror();
    void geom();
    void compute();
    void place();
    void optical();
    enum sides{BACK, FRONT};

  private:
    InternalMirror();
    G4int side;
    G4VSolid* solid;
    G4LogicalVolume* log;
    G4PVPlacement* phys;
    G4ThreeVector motherSize;
    G4String motherMaterial;
    G4LogicalVolume* motherLog;
    G4PVPlacement* motherPhys;
    G4double thickness;
    G4double pos;
  };
  InternalMirror* internalMirror = nullptr;
  virtual void build();
  void buildGroove();
  virtual void buildScreen();
  void visAtt();
  void cutGroove(G4double xPos);
  void cutGrooves();
  G4String material;
  G4String logName;
  G4String solidName;
  // Geometrical objects:
  G4LogicalVolume* grooveLog = nullptr;
  G4VSolid* grooveSolid  = nullptr;
  G4double grooveWidth = 0.0;
  G4double grooveSpatialFrequency = 0.0;
  /// Counter for the number of grooves etched into the screen.
  G4int nGrooves = 0;
  G4Colour colour;

};

#endif
