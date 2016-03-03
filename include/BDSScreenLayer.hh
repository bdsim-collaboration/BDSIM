#ifndef BDSSCREENLAYER_H
#define BDSSCREENLAYER_H 

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4Color.hh"
#include "G4PVPlacement.hh"

class BDSScreenLayer 
{
public:
  BDSScreenLayer(G4ThreeVector size,
		 G4String name,
		 G4String material,
		 G4double grooveWidth,
		 G4double grooveSpatialFrequency);
  ~BDSScreenLayer();
  inline G4LogicalVolume* log(){return _log;}
  inline G4String name(){return _name;}
  inline G4ThreeVector size(){return _size;}
  void phys(G4PVPlacement* phys);
  inline G4PVPlacement* phys(){return _phys;}
  void color(G4Color col);
  void backInternalMirror();
  void frontInternalMirror();

  /// Make this plane a sampling plane.
  void sampler(); 

protected:
  BDSScreenLayer();
  G4ThreeVector _size;
  G4String _name;
  G4LogicalVolume* _log;
  G4PVPlacement* _phys;
  G4VSolid* _solid;

  /// Copy number for placement. Used if it's a sampler as an ID for output lookup.
  G4int copyNumber;

private:
  class InternalMirror{
  public:
    InternalMirror(G4int varside, G4ThreeVector size, G4String material, G4LogicalVolume* motherLog, G4PVPlacement* motherPhys);
    ~InternalMirror();
    void geom();
    void compute();
    void place();
    void optical();
    enum sides{_BACK, _FRONT};

  private:
    InternalMirror();
    G4int _side;
    G4VSolid* _solid;
    G4LogicalVolume* _log;
    G4PVPlacement* _phys;
    G4ThreeVector _motherSize;
    G4String _motherMaterial;
    G4LogicalVolume* _motherLog;
    G4PVPlacement* _motherPhys;
    G4double _thickness;
    G4double _pos;
  };
  InternalMirror* _internalMirror;
  virtual void build();
  void buildGroove();
  virtual void buildScreen();
  void visAtt();
  void cutGroove(G4double xPos);
  void cutGrooves();
  G4String _material;
  G4String _logName;
  G4String _solidName;
  // Geometrical objects:
  G4LogicalVolume* _grooveLog;
  G4VSolid* _grooveSolid;
  G4double _grooveWidth;
  G4double _grooveSpatialFrequency;
  G4int _nGrooves;
  G4Color _color;

};

#endif
