#include "G4Electron.hh"
#include "G4Positron.hh"

#include "BDSGlobalConstants.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "BDSWorld.hh"
#include "BDSMaterials.hh"
#include "BDSBeamline.hh"

#include "G4Region.hh"

#include "G4IStore.hh"
#include "G4GeometrySampler.hh"

//GFlash parameterisation
#include "GFlashHomoShowerParameterisation.hh"
#include "G4FastSimulationManager.hh"
#include "BDSShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"
#include "BDSRegions.hh"

#include "G4UserLimits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4ScoringBox.hh"
#include "G4ScoringManager.hh"
#include "G4PSCellFlux3D.hh"
#include "BDSScoreWriter.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "globals.hh"
#include "G4ios.hh"
#include <iostream>
#include <list>
#include <map>
#include "BDSAcceleratorComponent.hh"

#include "G4Navigator.hh"
#include "G4UniformMagField.hh"

#include "G4Material.hh"
#include "BDSEnergyCounterSD.hh"

extern G4int gflash;
extern G4double gflashemax;
extern G4double gflashemin;

BDSRegions::BDSRegions(){
  buildRegions();
}

BDSRegions::~BDSRegions(){
  delete _precisionRegion;
  delete _worldRegion;
}

void BDSRegions::buildRegions(){
  buildPrecisionRegion();
  buildGFlashRegion();
}

void BDSRegions::buildPrecisionRegion(){
  _precisionRegion = new G4Region("precisionRegion");
  _precisionProductionCuts = new G4ProductionCuts();
  
  if(BDSGlobalConstants::Instance()->GetProdCutPhotonsP()>0)
    _precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsP(),G4ProductionCuts::GetIndex("gamma"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutElectronsP()>0)
    _precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsP(),G4ProductionCuts::GetIndex("e-"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutPositronsP()>0)
    _precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsP(),G4ProductionCuts::GetIndex("e+"));
  
  _precisionRegion->SetProductionCuts(_precisionProductionCuts);
#ifndef NOUSERLIMITS
  //  _precisionRegion->SetUserLimits(BDSWorld::Instance()->userLimits());
#endif
}

void BDSRegions::buildGFlashRegion(){
  _gFlashParticleBounds  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  _gFlashParticleBounds->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),gflashemax*GeV);
  _gFlashParticleBounds->SetMinEneToParametrise(*G4Electron::ElectronDefinition(),gflashemin*GeV);
  _gFlashParticleBounds->SetEneToKill(*G4Electron::ElectronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  
  _gFlashParticleBounds->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),gflashemax*GeV);
  _gFlashParticleBounds->SetMinEneToParametrise(*G4Positron::PositronDefinition(),gflashemin*GeV);
  _gFlashParticleBounds->SetEneToKill(*G4Positron::PositronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());

  _gFlashParticleBoundsVac  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  _gFlashParticleBoundsVac->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),0*GeV);
  _gFlashParticleBoundsVac->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),0*GeV);

  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - min E - electron: " << _gFlashParticleBounds->GetMinEneToParametrise(*G4Electron::ElectronDefinition())/GeV<< " GeV" << G4endl;
  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - max E - electron: " << _gFlashParticleBounds->GetMaxEneToParametrise(*G4Electron::ElectronDefinition())/GeV<< G4endl;
  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - kill E - electron: " << _gFlashParticleBounds->GetEneToKill(*G4Electron::ElectronDefinition())/GeV<< G4endl;

  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - min E - positron: " << _gFlashParticleBounds->GetMinEneToParametrise(*G4Positron::PositronDefinition())/GeV<< G4endl;
  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - max E - positron: " << _gFlashParticleBounds->GetMaxEneToParametrise(*G4Positron::PositronDefinition())/GeV<< G4endl;
  G4cout << "BDSRegions:buildGFlashRegion() -  _gFlashParticleBounds - kill E - positron: " << _gFlashParticleBounds->GetEneToKill(*G4Positron::PositronDefinition())/GeV<< G4endl;

 _gFlashHitMaker = new GFlashHitMaker();                    // Makes the EnergieSpots 
}

void BDSRegions::buildGFlashRegion(BDSAcceleratorComponent* /*var*/){

  /*
  vector<G4LogicalVolume*> MultipleSensVols = var->GetMultipleSensitiveVolumes();
  if( ( var->GetType()!="sampler" && var->GetType()!="csampler" )
      && MultipleSensVols.size()>0)
    {
      for(G4int i=0; i<(G4int)MultipleSensVols.size(); i++)
	{
	  if((MultipleSensVols.at(i)->GetRegion() != precisionRegion) && (var->GetType()==_ELEMENT)){//If not in the precision region....
	    //	    G4cout << "...adding " << MultipleSensVols[i]->GetName() << " to gFlashRegion" << G4endl;
	    G4String rname = "gFlashRegion_" + MultipleSensVols[i]->GetName();
	    gFlashRegion.push_back(new G4Region(rname.c_str()));
	    G4String mname = "fastShowerModel" + rname;
	    G4cout << "...making parameterisation..." << G4endl;
		  _gFlashFastShowerModel.push_back(new BDSShowerModel(mname.c_str(),gFlashRegion.back()));
		  _gFlashParameterisation.push_back(new GFlashHomoShowerParameterisation(BDSMaterials::Instance()->GetMaterial(MultipleSensVols[i]->GetMaterial()->GetName().c_str()))); 
		  _gFlashFastShowerModel.back()->SetParameterisation(*_gFlashParameterisation.back());
		  _gFlashFastShowerModel.back()->SetParticleBounds(*_gFlashParticleBounds) ;
		  _gFlashFastShowerModel.back()->SetHitMaker(*_gFlashHitMaker);
		  if(MultipleSensVols[i]->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
		    _gFlashFastShowerModel.back()->SetFlagParamType(1);//Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
		    _gFlashFastShowerModel.back()->SetFlagParticleContainment(1);//Turn on containment
		  } else {
		    _gFlashFastShowerModel.back()->SetFlagParamType(0);
		    _gFlashFastShowerModel.back()->SetFlagParticleContainment(0);
		    
		  }
		  MultipleSensVols[i]->SetRegion(gFlashRegion.back());
		  gFlashRegion.back()->AddRootLogicalVolume(MultipleSensVols[i]);
	  }		  
	}
    }
*/
}


void BDSRegions::buildGasRegion(){
  _gasRegion = new G4Region("gasRegion");
  _gasProductionCuts = new G4ProductionCuts();
  _gasProductionCuts->SetProductionCut(1*m,G4ProductionCuts::GetIndex("gamma"));
  _gasProductionCuts->SetProductionCut(1*m,G4ProductionCuts::GetIndex("e-"));
  _gasProductionCuts->SetProductionCut(1*m,G4ProductionCuts::GetIndex("e+"));
  _gasRegion->SetProductionCuts(_gasProductionCuts);
}
