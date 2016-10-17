#ifndef BDSLENS_H
#define BDSLENS_H 

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

class BDSLens 
{
public:
  BDSLens(G4String name, G4double diameter, G4double rho, G4double centreThickness);
  ~BDSLens();
  G4LogicalVolume* log();
  G4String name();
  inline G4double centreThickness(){return _centreThickness;}
  inline G4double diameter(){return _diameter;}
  inline G4double rho(){return _rho;} //Radius of curvature

private:
  void build();
  void visAtt();

  G4String _name;
  G4double _diameter;
  G4double _rho; //radius of curvature
  G4double _centreThickness;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
};

#endif
