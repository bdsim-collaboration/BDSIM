//
// Created by rtesse on 27/03/19.
//
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "Data.hh"
#include "PSAmbientDose3D.hh"

PSAmbientDose3D::PSAmbientDose3D(G4String name,
        G4int ni, G4int nj, G4int nk,
        G4int depi, G4int depj, G4int depk):G4VPrimitiveScorer(name),HCID3D(-1),EvtMap3D(0),
        fDepthi(depi),fDepthj(depj),fDepthk(depk)
{
    fNi=ni;
    fNj=nj;
    fNk=nk;

    G4cout << "-------------------------------------------------" << G4endl;
    G4cout << "PSAmbientDose3D::LoadTable()" << G4endl;
    /// Load tables for each particle
    conversion_factor_protons = Data::Instance()->LoadTable("h10_coeffs/h10protons.txt");
    conversion_factor_neutrons = Data::Instance()->LoadTable("h10_coeffs/h10neutrons.txt");
    conversion_factor_photons = Data::Instance()->LoadTable("h10_coeffs/h10photons.txt");
    conversion_factor_electrons = Data::Instance()->LoadTable("h10_coeffs/h10electrons.txt");
    ///
    G4cout << "-------------------------------------------------" << G4endl;
}

PSAmbientDose3D::~PSAmbientDose3D()
{
    conversion_factor_protons.clear();
    conversion_factor_neutrons.clear();
    conversion_factor_photons.clear();
    conversion_factor_electrons.clear();
}

G4bool PSAmbientDose3D::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

    G4double stepLength = aStep->GetStepLength()/CLHEP::cm;
    G4double conversion_factor = 0.;
    G4double ambient_dose;

    /// Calcul de la Fluence
    if ( stepLength == 0. ) return FALSE;

    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    auto solid = touchable->GetSolid();
    auto cubicVolume = solid->GetCubicVolume()/CLHEP::cm3;

    G4double CellFlux = (stepLength / cubicVolume);
    CellFlux *= aStep->GetPreStepPoint()->GetWeight();

    auto energy = (aStep->GetPreStepPoint()->GetKineticEnergy()) / MeV;
    G4int partId = aStep->GetTrack()->GetDynamicParticle()->GetPDGcode();

    //G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << "\t" << partId << G4endl;

    switch (partId)
    {
        case 2212:
            conversion_factor = (GetInterpFactor(energy, conversion_factor_protons));
            break;

        case 2112:
            conversion_factor = (GetInterpFactor(energy, conversion_factor_neutrons));
            break;

        case 22:
            conversion_factor = (GetInterpFactor(energy, conversion_factor_photons));
            break;

        case 11:
            conversion_factor = (GetInterpFactor(energy, conversion_factor_electrons));
            break;

        default:
            break;
    }

    //G4cout << energy << "\t" << conversion_factor << G4endl;

    ambient_dose = conversion_factor*CellFlux;
    G4int index = GetIndex(aStep);

    EvtMap3D->add(index,ambient_dose);
    return TRUE;
}

void PSAmbientDose3D::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap3D = new G4THitsMap<G4double>(detector->GetName(),
                                      GetName());
    if ( HCID3D < 0 ) HCID3D = GetCollectionID(0);
    HCE->AddHitsCollection(HCID3D,EvtMap3D);
}

void PSAmbientDose3D::EndOfEvent(G4HCofThisEvent*)
{
    ;
}

void PSAmbientDose3D::clear()
{

    EvtMap3D->clear();
}

G4int PSAmbientDose3D::GetIndex(G4Step* aStep)
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
        G4Exception("PSAmbientDose3D::GetIndex","DetPS0006",JustWarning,ED);
    }

    return i*fNj*fNk+j*fNk+k;
}

inline G4double PSAmbientDose3D::GetInterpFactor(G4double E_x, vector<vector<G4double> > conversion_factor)
{
    /// Fonction qui renvoie la valeur de la section efficace en fonction de l'énergie.
    /// Une interpolation linéaire est faite entre deux énergies.

    if(E_x < conversion_factor[0][0] || E_x > conversion_factor.back()[0])
    {
        return 0;
    }


    // Use GSL to perform the computation ??
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

    double pente = (y_b-y_a)/(E_b-E_a);
    y = pente*(E_x-E_a)+y_a;

    return y;
}

