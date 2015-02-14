#include "BDSOutputVector.hh"

BDSOutputVector::BDSOutputVector(){}

BDSOutputVector::~BDSOutputVector(){
  for (unsigned int i=0; i<output.size(); i++) {
    delete output[i];
  }
}

void BDSOutputVector::Add(BDSOutputBase* derived) {
  output.push_back(derived);
}

void BDSOutputVector::WriteHits(BDSSamplerHitsCollection* a){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WriteHits(a);
  }
}

void BDSOutputVector::WriteEnergyLoss(BDSEnergyCounterHitsCollection* a){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WriteEnergyLoss(a);
  }
}

void BDSOutputVector::WritePrimaryLoss(BDSEnergyCounterHit* a){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WritePrimaryLoss(a);
  }
}

void BDSOutputVector::WritePrimaryHit(BDSEnergyCounterHit* a){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WritePrimaryHit(a);
  }
}

void BDSOutputVector::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WriteTrajectory(TrajVec);
  }
}

void BDSOutputVector::WritePrimary(G4String samplerName, 
				   G4double E,
				   G4double x0,
				   G4double y0,
				   G4double z0,
				   G4double xp,
				   G4double yp,
				   G4double zp,
				   G4double t,
				   G4double weight,
				   G4int    PDGType, 
				   G4int    nEvent, 
				   G4int    TurnsTaken)
{
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WritePrimary(samplerName,E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,TurnsTaken);
  }
}

void BDSOutputVector::WriteHistogram(BDSHistogram1D* histogramIn) {
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->WriteHistogram(histogramIn);
  }
}

void BDSOutputVector::Commit(){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->Commit();
  }
}

void BDSOutputVector::Write(){
  for (unsigned int i=0; i<output.size(); i++) {
    output[i]->Write();
  }
}
