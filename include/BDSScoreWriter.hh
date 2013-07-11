#ifndef BDSScoreWriter_h
#define BDSScoreWriter_h 1

// based on ROOT
#ifdef USE_ROOT

#include "globals.hh"
#include "G4VScoreWriter.hh"
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH3D.h"

//const enum ProjectionPlanes {xy=100, yz=010, zx=001};

// class description:
//
//  This class represents storing the scored quantity into a file.
//


class BDSScoreWriter : public G4VScoreWriter {

public:
	BDSScoreWriter();
	virtual ~BDSScoreWriter();
	void setMaterial(G4String material) {fMaterial = material;};
	void setFireAngle(G4double fireAngle) {fFireAngle = fireAngle;};
	G4String getMaterial() {return fMaterial;};
	G4String getFireAngle() {return fFireAngle;};
public:
	// store a quantity into a file
  void DumpQuantityToFile(G4String & psName, G4String & fileName, G4String & option);
  void DumpQuantityToFile(G4String & psName, TFile* tFile, G4String & option);
	char fStartTime[50];
	char fEndTime[50];

	G4String fMaterial;
	G4double fFireAngle;
};

#endif // USE_ROOT

#endif
