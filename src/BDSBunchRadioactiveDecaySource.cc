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
    radioactiveDecaySourceFile = beam.distrFile;
    radioactiveDecaySourceName = BDS::SplitOnWhiteSpace(beam.radioactiveDecaySourceName);

    int numberOfSources = radioactiveDecaySourceName.size();

    activity = new TH1D("activity", "activity", numberOfSources, 0.0, numberOfSources);

    TFile* myFile = TFile::Open(radioactiveDecaySourceFile.c_str());

    std::string treeNameTranslation = "RadioactiveDecaySource/translation";
    TTree* treeTranslation = (TTree*) myFile->Get(treeNameTranslation.c_str());

    std::string treeNameRotation = "RadioactiveDecaySource/rotation";
    TTree* treeRotation = (TTree*) myFile->Get(treeNameRotation.c_str());

    std::string treeNameBins = "RadioactiveDecaySource/bins";
    TTree* treeBins = (TTree*) myFile->Get(treeNameBins.c_str());

    std::string treeNameActivity = "RadioactiveDecaySource/activity";
    TTree* treeActivity = (TTree*) myFile->Get(treeNameActivity.c_str());

    for (int j = 0; j < numberOfSources; ++j){

        TBranch* branchTranslation = treeTranslation->GetBranch(radioactiveDecaySourceName[j]);

        double valueTranslation;
        branchTranslation->SetAddress(& valueTranslation);
        std::vector<double> vTranslation;
        vTranslation.reserve(3);
        for (auto i = 0; i < 3; ++i) {
            branchTranslation->GetEntry(i);
            vTranslation.push_back(valueTranslation*CLHEP::m);
        }
        translation.push_back(G4ThreeVector(vTranslation[0], vTranslation[1], vTranslation[2]));

        TBranch* branchRotation = treeRotation->GetBranch(radioactiveDecaySourceName[j]);

        double valueRotation;
        branchRotation->SetAddress(& valueRotation);
        std::vector<double> vRotation;
        vRotation.reserve(9);
        for (auto i = 0; i < 9; ++i) {
            branchRotation->GetEntry(i);
            vRotation.push_back(valueRotation);
        }
        CLHEP::HepRotation rot = CLHEP::HepRotation();
        G4ThreeVector row_1 = G4ThreeVector(vRotation[0], vRotation[1], vRotation[2]);
        G4ThreeVector row_2 = G4ThreeVector(vRotation[3], vRotation[4], vRotation[5]);
        G4ThreeVector row_3 = G4ThreeVector(vRotation[6], vRotation[7], vRotation[8]);
        rot.setRows(row_1, row_2, row_3);
        rotation.push_back(rot);

        TBranch* branchBins = treeBins->GetBranch(radioactiveDecaySourceName[j]);

        double valueBins;
        branchBins->SetAddress(& valueBins);
        std::vector<double> vBins;
        vBins.reserve(3);
        for (auto i = 0; i < 3; ++i) {
            branchBins->GetEntry(i);
            vBins.push_back(valueBins*CLHEP::m);
        }
        bins.push_back(G4ThreeVector(vBins[0],vBins[1],vBins[2]));

        TBranch* branchActivity = treeActivity->GetBranch(radioactiveDecaySourceName[j]);

        double valueActivity;
        branchActivity->SetAddress(& valueActivity);
        std::vector<double> vActivity;
        vActivity.reserve(1);
        for (auto i = 0; i < 1; ++i) {
            branchActivity->GetEntry(i);
            vActivity.push_back(valueActivity);
        }
        activity->Fill(j,vActivity[0]);

        std::string histTreeName = "RadioactiveDecaySource/histograms/" + radioactiveDecaySourceName[j];
        histVector.push_back((TH3D*) myFile->Get(histTreeName.c_str()));

    }

    myFile->Close();

    BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
}

BDSParticleCoordsFull BDSBunchRadioactiveDecaySource::GetNextParticleLocal()
{

    double h = activity->GetRandom();
    int bin_h = ((TAxis*)activity->GetXaxis())->FindBin(h)-1;

    TH3D* hist = histVector[bin_h];

    hist->GetRandom3(hx, hy, hz);

    double bin_x = ((TAxis*)hist->GetXaxis())->FindBin(hx);
    double bin_y = ((TAxis*)hist->GetYaxis())->FindBin(hy);
    double bin_z = ((TAxis*)hist->GetZaxis())->FindBin(hz);

    double bin_centerx = ((TAxis*)hist->GetXaxis())->GetBinCenter(bin_x)*CLHEP::m;
    double bin_centery = ((TAxis*)hist->GetYaxis())->GetBinCenter(bin_y)*CLHEP::m;
    double bin_centerz = ((TAxis*)hist->GetZaxis())->GetBinCenter(bin_z)*CLHEP::m;

    G4ThreeVector bin_center = G4ThreeVector(bin_centerx, bin_centery, bin_centerz);
    G4Transform3D transform = G4Transform3D(rotation[bin_h], translation[bin_h]);
    const HepGeom::Point3D<G4double> bin_center_new = transform * (HepGeom::Point3D<G4double>) bin_center;

    G4double t  = T0;
    G4double z  = Z0 + bin_center_new.z() + (flatGen->shoot()-0.5)* bins[bin_h][2];
    G4double x  = X0 + bin_center_new.x() + (flatGen->shoot()-0.5)* bins[bin_h][0];
    G4double y  = Y0 + bin_center_new.y() + (flatGen->shoot()-0.5)* bins[bin_h][1];
    G4double xp = Xp0;
    G4double yp = Yp0;
    G4double zp = CalculateZp(xp,yp,Zp0);
    G4double E  = E0;

    return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0,E,/*weight=*/1.0);
}
