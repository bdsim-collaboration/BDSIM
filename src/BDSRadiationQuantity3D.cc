//
// Created by rtesse on 28/03/19.
//

#include "BDSRadiationQuantity3D.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

BDSRadiationQuantity3D::BDSRadiationQuantity3D(G4String scorer_name, G4String filename,
        G4int ni, G4int nj, G4int nk,
        G4int depi, G4int depj, G4int depk):G4VPrimitiveScorer(scorer_name),
        HCID3D(-1),EvtMap3D(nullptr), fDepthi(depi),fDepthj(depj),fDepthk(depk)
{
    fNi=ni;
    fNj=nj;
    fNk=nk;

    G4cout << "-------------------------------------------------" << G4endl;
    G4cout << "PSAmbientDose3D::LoadTable()" << G4endl;
    /// Load tables for each particle
    if(filename == "")
    {
        G4ExceptionDescription ED;
        ED << "ERROR: no file provided" << G4endl;
        G4Exception("PSRadiationQuantity3D::PSRadiationQuantity3D","DetPS0006",FatalException,ED);
    }
    //TODO Load the data conversion_factor = Data::Instance()->LoadTable(filename);
    ///
    G4cout << "-------------------------------------------------" << G4endl;

}

BDSRadiationQuantity3D::~BDSRadiationQuantity3D()
{
    conversion_factor.clear();
}

G4bool BDSRadiationQuantity3D::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
    G4double stepLength = aStep->GetStepLength()/CLHEP::cm;
    G4double radiation_quantity;

    /// Fluence computation
    if ( stepLength == 0. ) return FALSE;

    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    auto solid = touchable->GetSolid();
    auto cubicVolume = solid->GetCubicVolume()/CLHEP::cm3;

    G4double CellFlux = (stepLength / cubicVolume);
    CellFlux *= aStep->GetPreStepPoint()->GetWeight();

    auto energy = (aStep->GetPreStepPoint()->GetKineticEnergy()) / MeV;

    radiation_quantity = (GetInterpFactor(energy, conversion_factor))*CellFlux;
    G4int index = GetIndex(aStep);

    EvtMap3D->add(index,radiation_quantity);
    return TRUE;

}

void BDSRadiationQuantity3D::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap3D = new G4THitsMap<G4double>(detector->GetName(),
                                        GetName());
    if ( HCID3D < 0 ) HCID3D = GetCollectionID(0);
    HCE->AddHitsCollection(HCID3D,EvtMap3D);
}

void BDSRadiationQuantity3D::EndOfEvent(G4HCofThisEvent*)
{;}

void BDSRadiationQuantity3D::clear()
{

    EvtMap3D->clear();
}

G4int BDSRadiationQuantity3D::GetIndex(G4Step* aStep)
{
    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int i = touchable->GetReplicaNumber(fDepthi);
    G4int j = touchable->GetReplicaNumber(fDepthj);
    G4int k = touchable->GetReplicaNumber(fDepthk);

    if(i<0||j<0||k<0)
    {
        G4ExceptionDescription ED;
        ED << "GetReplicaNumber is negative" << G4endl
           << "touchable->GetReplicaNumber(fDepthi) returns i,j,k = "
           << i << "," << j << "," << k << " for volume "
           << touchable->GetVolume(fDepthi)->GetName() << ","
           << touchable->GetVolume(fDepthj)->GetName() << ","
           << touchable->GetVolume(fDepthk)->GetName() << G4endl;
        G4Exception("PSRadiationQuantity3D::GetIndex","DetPS0006",JustWarning,ED);
    }

    return i*fNj*fNk+j*fNk+k;
}

inline G4double BDSRadiationQuantity3D::GetInterpFactor(G4double E_x, vector<vector<G4double> > conversion_factor)
{
    // TODO with geant4
    /// Linear interpolation, maybe not the best way

    if(E_x < conversion_factor[0][0] || E_x > conversion_factor.back()[0])
    {
        return 0;
    }

    int flag = 0;
    while(conversion_factor[flag][0] < E_x)
    {
        flag ++;
    }

    double E_a = conversion_factor[flag-1][0];
    double E_b = conversion_factor[flag][0];
    double y_a = conversion_factor[flag-1][1];
    double y_b = conversion_factor[flag][1];

    double y;

    double slope = (y_b-y_a)/(E_b-E_a);
    y = slope*(E_x-E_a)+y_a;

    return y;
}

