#include "PSAmbientDose.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "Data.hh"

PSAmbientDose::PSAmbientDose(G4String name, G4int depth)
:G4VPrimitiveScorer(name,depth),HCID(-1),EvtMap(0)
{

    G4cout << "-------------------------------------------------" << G4endl;
    G4cout << "PSAmbientDose::LoadTable()" << G4endl;
    /// Load tables for each particle
    conversion_factor_protons = Data::Instance()->LoadTable("h10_coeffs/h10protons.txt");
    conversion_factor_neutrons = Data::Instance()->LoadTable("h10_coeffs/h10neutrons.txt");
    conversion_factor_photons = Data::Instance()->LoadTable("h10_coeffs/h10photons.txt");
    conversion_factor_electrons = Data::Instance()->LoadTable("h10_coeffs/h10electrons.txt");
    ///
    G4cout << "-------------------------------------------------" << G4endl;
}

PSAmbientDose::~PSAmbientDose()
{
    conversion_factor_protons.clear();
    conversion_factor_neutrons.clear();
    conversion_factor_photons.clear();
    conversion_factor_electrons.clear();
}

G4bool PSAmbientDose::ProcessHits(G4Step* aStep,G4TouchableHistory*)
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

    EvtMap->add(index,ambient_dose);
    return TRUE;
}

void PSAmbientDose::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap = new G4THitsMap<G4double>(detector->GetName(),
                                      GetName());
    if ( HCID < 0 ) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID,EvtMap);
}

void PSAmbientDose::EndOfEvent(G4HCofThisEvent*)
{
    ;
}

void PSAmbientDose::clear()
{

    EvtMap->clear();
}

inline G4double PSAmbientDose::GetInterpFactor(G4double E_x, vector<vector<G4double> > conversion_factor)
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

