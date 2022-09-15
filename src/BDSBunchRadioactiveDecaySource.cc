/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSBunchRadioactiveDecaySource.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH3D.h"

BDSBunchRadioactiveDecaySource::BDSBunchRadioactiveDecaySource():
        BDSBunch("radioactivedecaysource"),
        radioactiveDecaySourceFile("radioactiveDeacySourceFile.root")
{
    flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchRadioactiveDecaySource::~BDSBunchRadioactiveDecaySource()
{
    delete flatGen;
}

void BDSBunchRadioactiveDecaySource::SetOptions(const BDSParticleDefinition* beamParticle,
                                     const GMAD::Beam& beam,
                                     const BDSBunchType& distrType,
                                     G4Transform3D beamlineTransformIn,
                                     const G4double beamlineSIn)
{
    radioactiveDecaySourceFile = beam.radioactiveDecaySourceFile;
    radioactiveDecaySourceName = beam.radioactiveDecaySourceName;

    TFile* myFile = TFile::Open(radioactiveDecaySourceFile.c_str());

    std::string treeName = "RadioactiveDecaySource/translation";
    TTree* tree = (TTree*) myFile->Get(treeName.c_str());
    TBranch* branch = tree->GetBranch(radioactiveDecaySourceName.c_str());

    double value;
    branch->SetAddress(& value);
    std::vector<double> v;
    v.reserve(3);
    for (auto i = 0; i < 3; ++i) {
        branch->GetEntry(i);
        v.push_back(value*CLHEP::m);
    }
    translation = G4ThreeVector(v[0],v[1],v[2]);

    std::string treeNameBins = "RadioactiveDecaySource/bins";
    TTree* treeBins = (TTree*) myFile->Get(treeNameBins.c_str());
    TBranch* branchBins = treeBins->GetBranch(radioactiveDecaySourceName.c_str());

    double valueBins;
    branchBins->SetAddress(& valueBins);
    std::vector<double> vBins;
    v.reserve(3);
    for (auto i = 0; i < 3; ++i) {
        branchBins->GetEntry(i);
        vBins.push_back(valueBins*CLHEP::m);
    }
    bins = G4ThreeVector(vBins[0],vBins[1],vBins[2]);

    std::string histTreeName = "RadioactiveDecaySource/histograms/" + radioactiveDecaySourceName;
    hist = (TH3D*) myFile->Get(histTreeName.c_str());
    myFile->Close();

    BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
}

BDSParticleCoordsFull BDSBunchRadioactiveDecaySource::GetNextParticleLocal()
{
    hist->GetRandom3(x,y,z);

    double bin_x = ((TAxis*)hist->GetXaxis())->FindBin(x);
    double bin_y = ((TAxis*)hist->GetYaxis())->FindBin(y);
    double bin_z = ((TAxis*)hist->GetZaxis())->FindBin(z);

    double bin_centerx = ((TAxis*)hist->GetXaxis())->GetBinCenter(bin_x)*CLHEP::m;
    double bin_centery = ((TAxis*)hist->GetYaxis())->GetBinCenter(bin_y)*CLHEP::m;
    double bin_centerz = ((TAxis*)hist->GetZaxis())->GetBinCenter(bin_z)*CLHEP::m;

    G4ThreeVector bin_center = G4ThreeVector(bin_centerx, bin_centery, bin_centerz);
    CLHEP::HepRotation mt = CLHEP::HepRotation();
    G4Transform3D transform = G4Transform3D(mt, translation);
    const HepGeom::Point3D<G4double> bin_center_new = transform * (HepGeom::Point3D<G4double>) bin_center;

    G4double t  = T0;
    G4double z  = Z0 + bin_center_new.z() + (flatGen->shoot()-0.5)* bins[2];
    G4double x  = X0 + bin_center_new.x() + (flatGen->shoot()-0.5)* bins[0];
    G4double y  = Y0 + bin_center_new.y() + (flatGen->shoot()-0.5)* bins[1];
    G4double xp = Xp0;
    G4double yp = Yp0;
    G4double zp = CalculateZp(xp,yp,Zp0);
    G4double E  = E0;

    return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0,E,/*weight=*/1.0);
}
