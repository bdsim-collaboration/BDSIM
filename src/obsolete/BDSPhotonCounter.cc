#include "BDSPhotonCounter.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"

BDSPhotonCounter* BDSPhotonCounter::_instance = nullptr;

BDSPhotonCounter* BDSPhotonCounter::Instance(){
  if(_instance==nullptr) {
    _instance = new BDSPhotonCounter();
  }
  return _instance;
}

BDSPhotonCounter::BDSPhotonCounter(){
  _nPhotons=0;
  _energy=0;
}

BDSPhotonCounter::~BDSPhotonCounter(){
  _instance = nullptr;
}

void BDSPhotonCounter::countPhoton(const G4Track* aTrack){
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    { // particle is optical photon                                                                                                                             
      if(aTrack->GetParentID()>0)
	{ // particle is secondary                                                                                                                                
	  _nPhotons++;
	  _energy+=aTrack->GetTotalEnergy();
	}
    }
}


