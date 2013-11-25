#include "BDSGlobalConstants.hh" 
#include "BDSMultiFacetLayer.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "BDSDebug.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "parser/gmad.h"
#include "G4TwoVector.hh"

//============================================================
BDSMultiFacetLayer::BDSMultiFacetLayer (G4String name, BDSLensFacet* facet, G4double gapWidth, G4double nFacets):
  _facet(facet), _gapWidth(gapWidth), _nFacets(nFacets)
{
  _name = name+"_MultiFacetLayer";
  computeDimensions();
  build();
}

void BDSMultiFacetLayer::computeDimensions(){
  _size.setZ(_facet->totalHeight());
  _size.setY(_facet->size().y());
  _size.setX(_nFacets*_facet->size().x() + 
	     (_nFacets-1)*_gapWidth);
  //Compute positions of the facets relative to the mother volume.
  G4double x=0,y=0,z=0;
  for(double i=0; i<_nFacets; i++){
    x=(i+0.5)*_facet->size().x();
    x+=i*_gapWidth; //The width of the gaps between the facets.
    _facetPos.push_back(G4ThreeVector(x,y,z));
  }
}

void BDSMultiFacetLayer::build(){
  buildScreen();
  placeFacets();
}


void BDSMultiFacetLayer::buildScreen(){
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("vacuum"),(_name+"_log").c_str(),0,0,0);
}

void BDSMultiFacetLayer::placeFacets(){
  //Place the facets in the mother volume at the locations in _facetPos.
  
  for(int i=0; i<_nFacets; i++){
    new G4PVPlacement(_facet->rotation(), //The facet (trapezoid) must be rotated into the correct coordinate system (z normal to base).
		      _facetPos[i],
		      _facet->log(),
		      _facet->name(),
		      _log,
		      true,
		      i,
		      true);
  }
}

BDSMultiFacetLayer::~BDSMultiFacetLayer(){
}
