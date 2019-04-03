/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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
#include "BDSException.hh"
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include "G4SDParticleWithEnergyFilter.hh"

#include "G4ScoringManager.hh"
#include "G4ScoringBox.hh"
#include "G4PSDoseDeposit3D.hh"
#include "BDSRadiationQuantity3D.hh"


BDSScorerFactory* BDSScorerFactory::instance = nullptr;

BDSScorerFactory* BDSScorerFactory::Instance()
{
    if (!instance)
    {instance = new BDSScorerFactory();}
    return instance;
}

BDSScorerFactory::BDSScorerFactory()
{;}

BDSScorerFactory::~BDSScorerFactory()
{
    instance = nullptr;
}

G4VPrimitiveScorer* BDSScorerFactory::CreateScorer(G4String name,
        const BDSScorerInfo* info)
{
    /// Here create the scorer with the informations inside BDSScorerInfo.
    G4VPrimitiveScorer* primitiveScorer = GetAppropriateScorer(name, info->scorerType, info->filename);

    G4String particleName = info->particle->GetParticleName();
    G4SDParticleWithEnergyFilter* scorer_filter= new G4SDParticleWithEnergyFilter("particle_filter",
            info->minimumEnergy,
            info->maximumEnergy);

    scorer_filter->add(particleName);
    primitiveScorer->SetFilter(scorer_filter);

    return primitiveScorer;

    /// For ambient dose, check if the file is given in info

/*
    G4ScoringBox* scorer_box = new G4ScoringBox(name);

    /// size of the scoring mesh
    G4double vsize[3];
    vsize[0] = 0.5*20.*CLHEP::cm;
    vsize[1] = 0.5*20.*CLHEP::cm;
    vsize[2] = 0.5*20.*CLHEP::cm;
    B1Scorer_box->SetSize(vsize);

    /// divisions of the scoring mesh
    G4int nSegment[3];
    nSegment[0] = 2;
    nSegment[1] = 5;
    nSegment[2] = 10;

    B1Scorer_box->SetNumberOfSegments(nSegment);

    /// Position of the scoring mesh
    G4double centerPosition[3];
    centerPosition[0] = 0.*CLHEP::cm;
    centerPosition[1] = 0.*CLHEP::cm;
    centerPosition[2] = 0.*CLHEP::cm;
    B1Scorer_box->SetCenterPosition(centerPosition);

    /// Dose scorer

    G4PSDoseDeposit3D* ps =new G4PSDoseDeposit3D("Dose");
    B1Scorer_box->SetPrimitiveScorer(ps);

    /// Ambient dose scorer
    PSAmbientDose3D* ambientdose_ps = new PSAmbientDose3D("Ambient_dose_mesh");
    B1Scorer_box->SetPrimitiveScorer(ambientdose_ps);
    //B1Scorer_box->SetFilter(new G4SDParticleFilter("pfilter","proton"));

    /// Radioprotection scorer
    PSRadiationQuantity3D* ambientdose_scorer_proton = new PSRadiationQuantity3D("ambient_dose_proton","h10_coeffs/h10protons.txt");
    B1Scorer_box->SetPrimitiveScorer(ambientdose_scorer_proton);
    B1Scorer_box->SetFilter(new G4SDParticleFilter("pfilter","proton"));

    PSRadiationQuantity3D* ambientdose_scorer_neutron = new PSRadiationQuantity3D("ambient_dose_neutron","h10_coeffs/h10neutrons.txt");
    B1Scorer_box->SetPrimitiveScorer(ambientdose_scorer_neutron);
    B1Scorer_box->SetFilter(new G4SDParticleFilter("nfilter","neutron"));

    /// register the mesh in the scoring manager
    scManager->RegisterScoringMesh(B1Scorer_box);
    scManager->SetScoreWriter(new B1UserScoreWriter()); // #OWN WRITER, TO DO WITH ROOT
    scManager->SetCurrentMesh(B1Scorer_box);
    info->filename;
*/
    /*
    BDSCavityFactoryBase* factory = GetAppropriateFactory(info->cavityType);

    return factory->CreateCavity(name, totalChordLength, info, vacuumMaterial);
     */
}

G4VPrimitiveScorer* BDSScorerFactory::GetAppropriateScorer(G4String name,
							   const BDSScorerType scorerType,
							   G4String filename)
{
  switch (scorerType.underlying())
    {
    case BDSScorerType::deposited_dose:
      {return new G4PSDoseDeposit3D(name); break;}
    case BDSScorerType::ambient_dose:
      {return new BDSRadiationQuantity3D(name,filename); break;}
    default:
      {
	throw BDSException(__METHOD_NAME__, "unknown scorer type \"" + scorerType.ToString() + "\"");
	break;
      }
    }
}
