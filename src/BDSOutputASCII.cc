#include "BDSOutputASCII.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include <ctime>

BDSOutputASCII::BDSOutputASCII():BDSOutputBase()
{
  time_t tm = time(NULL);

  filename = BDSExecOptions::Instance()->GetOutputFilename()+".txt";
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << "Output format ASCII, filename: "<<filename<<G4endl;
#endif
  of.open(filename);
  of<<"### BDSIM output created "<<ctime(&tm)<<" ####"<<G4endl;
  of<<"# PT E[GeV] X[mum] Y[mum] Z[m] Xp[rad] Yp[rad] NEvent Weight ParentID TrackID Turn"<<G4endl;
  G4String filenameEloss = BDSExecOptions::Instance()->GetOutputFilename()+".eloss.txt";
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << "Eloss output format ASCII, filename: "<<filenameEloss<<G4endl;
#endif
  ofEloss.open(filenameEloss);
  ofEloss<<"### BDSIM eloss output created "<<ctime(&tm)<<" ####"<<G4endl;
  ofEloss<<"#Energy loss: Z[m] E[GeV] PartID Weight Turn"<<G4endl;

}

BDSOutputASCII::~BDSOutputASCII()
{
  if (of.is_open()) {
    of.flush();
    of.close();
  }
  if (ofEloss.is_open()) {
    ofEloss.flush();
    ofEloss.close();
  }
}

void BDSOutputASCII::WriteAsciiHit(G4int PDGType, G4double Mom, G4double X, G4double Y, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken){
  of<<PDGType
    <<" "
    <<Mom/CLHEP::GeV
    <<" "
    <<X/CLHEP::micrometer
    <<" "
    <<Y/CLHEP::micrometer
    <<" "
    <<S / CLHEP::m
    <<" "
    <<XPrime / CLHEP::radian
    <<" "
    <<YPrime / CLHEP::radian
    <<" "
    <<EventNo 
    <<" "
    <<Weight
    <<" "
    <<ParentID
    <<" "
    <<TrackID
    <<" "
    <<TurnsTaken
    <<G4endl;
}

void BDSOutputASCII::WritePrimary(G4String /*samplerName*/, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double /*zp*/,G4double /*t*/,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken){
  WriteAsciiHit(PDGType, E, x0, y0, z0, xp, yp, nEvent, weight, 0, 1, TurnsTaken);
}

void BDSOutputASCII::WriteHits(BDSSamplerHitsCollection *hc)
{
  int G4precision = G4cout.precision();
  G4cout.precision(15);
  
  for (G4int i=0; i<hc->entries(); i++)
    {
      WriteAsciiHit(
		    (*hc)[i]->GetPDGtype(),
		    (*hc)[i]->GetMom(),
		    (*hc)[i]->GetX(),
		    (*hc)[i]->GetY(),
		    (*hc)[i]->GetS(),
		    (*hc)[i]->GetXPrime(),
		    (*hc)[i]->GetYPrime(),
		    (*hc)[i]->GetEventNo(),
		    (*hc)[i]->GetWeight(),
		    (*hc)[i]->GetParentID(),
		    (*hc)[i]->GetTrackID(),
		    (*hc)[i]->GetTurnsTaken()
		    );
    }
  of.flush();
  // set precision back
  G4cout.precision(G4precision);
}

/// write a trajectory to a root/ascii file
// TODO: ASCII file not implemented - JS
void BDSOutputASCII::WriteTrajectory(std::vector<G4VTrajectory*> &/*TrajVec*/){
  G4cout << __METHOD_NAME__ << "WARNING trajectory writing not implemented for ASCII output" << G4endl;
}

// make energy loss histo
void BDSOutputASCII::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  G4int n_hit = hc->entries();
  
  for (G4int i=0;i<n_hit;i++)
    {
      G4double Energy     = (*hc)[i]->GetEnergy();
      G4double Zpos       = (*hc)[i]->GetZ();
      G4int    partID     = (*hc)[i]->GetPartID();
      G4double weight     = (*hc)[i]->GetWeight();
      G4int    turnnumber = (*hc)[i]->GetTurnsTaken();

      ofEloss << Zpos/CLHEP::m << "  " << Energy/CLHEP::GeV << "  " << partID << "  " << weight << turnnumber << G4endl;
    }
  ofEloss.flush();
}

void BDSOutputASCII::Commit()
{
  of.flush();
  ofEloss.flush();
  /// multiple file writing not implemented for ASCII
}

void BDSOutputASCII::Write()
{
  of.flush();
  of.close();
  ofEloss.flush();
  ofEloss.close();
}
