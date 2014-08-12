#include "BDSOutputASCII.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
//#include <ctime>
#include "time.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

BDSOutputASCII::BDSOutputASCII():BDSOutputBase()
{
  time_t currenttime;
  time(&currenttime);
  std::string timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);

  filename = BDSExecOptions::Instance()->GetOutputFilename()+".txt";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Output format ASCII, filename: " << filename << G4endl;
#endif
  of.open(filename);
  of << "### BDSIM output - created "<< timestring << G4endl;
  of << std::left << std::setprecision(10) << std::fixed
     << std::setw(6)  << "PDGID"    << " "
     << std::setw(15) << "E[GeV]"   << " "
     << std::setw(15) << "X[mum]"   << " "
     << std::setw(15) << "Y[mum]"   << " "
     << std::setw(20) << "S[m]"     << " "
     << std::setw(15) << "Xp[rad]"  << " "
     << std::setw(15) << "Yp[rad]"  << " "
     << std::setw(6)  << "NEvent"   << " "
     << std::setw(15) << "Weight"   << " "
     << std::setw(9)  << "ParentID" << " "
     << std::setw(8)  << "TrackID"  << " "
     << std::setw(5)  << "Turn"
     << G4endl;  
  
  G4String filenameEloss = BDSExecOptions::Instance()->GetOutputFilename()+".eloss.txt";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Eloss output format ASCII, filename: " << filenameEloss << G4endl;
#endif
  ofEloss.open(filenameEloss);
  ofEloss << "### BDSIM energy loss output - created "<< timestring <<G4endl;
  ofEloss << std::left << std::setprecision(5) << std::fixed
    //<< std::setw(12) << "Z[m]"     << " "
	  << std::setw(12) << "S[m]"     << " "
	  << std::setw(12) << "E[GeV]"   << " "
	  << std::setw(6)  << "PDGID"    << " "
	  << std::setw(7)  << "Weight"   << " "
	  << std::setw(6)  << "Turn"
	  << G4endl;
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

void BDSOutputASCII::WriteAsciiHit(G4int PDGType, G4double Mom, G4double X, G4double Y, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken)
{
  of << std::left << std::setprecision(10) << std::fixed
     << std::setw(6)  << PDGType              << " "
     << std::setw(15) << Mom/CLHEP::GeV       << " "
     << std::setw(15) << X/CLHEP::micrometer  << " "
     << std::setw(15) << Y/CLHEP::micrometer  << " "
     << std::setw(20) << S/CLHEP::m           << " "
     << std::setw(15) << XPrime/CLHEP::radian << " "
     << std::setw(15) << YPrime/CLHEP::radian << " "
     << std::setw(6)  << EventNo              << " "
     << std::setw(15) << Weight               << " "
     << std::setw(9)  << ParentID             << " "
     << std::setw(8)  << TrackID              << " "
     << std::setw(5)  << TurnsTaken
     << G4endl;
}

void BDSOutputASCII::WritePrimary(G4String /*samplerName*/, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double /*zp*/,G4double /*t*/,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken){
  WriteAsciiHit(PDGType, E, x0, y0, z0, xp, yp, nEvent, weight, 0, 1, TurnsTaken);
}

void BDSOutputASCII::WriteHits(BDSSamplerHitsCollection *hc)
{  
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
}

/// write a trajectory to a root/ascii file
// TODO: ASCII file not implemented - JS
void BDSOutputASCII::WriteTrajectory(std::vector<BDSTrajectory*> &/*TrajVec*/){
  G4cout << __METHOD_NAME__ << "WARNING trajectory writing not implemented for ASCII output" << G4endl;
}

// make energy loss histo
void BDSOutputASCII::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  G4int n_hit = hc->entries();
  
  for (G4int i=0;i<n_hit;i++)
    {
      G4double Energy     = (*hc)[i]->GetEnergy();
      G4double Spos       = (*hc)[i]->GetS();
      //G4double Zpos       = (*hc)[i]->GetZ();
      G4int    partID     = (*hc)[i]->GetPartID();
      G4double weight     = (*hc)[i]->GetWeight();
      G4int    turnnumber = (*hc)[i]->GetTurnsTaken();

      ofEloss << std::left << std::setprecision(5) << std::fixed
	      << std::setw(12) << Spos/CLHEP::m     << " "
	      << std::setw(12) << Energy/CLHEP::GeV << " "
	      << std::setw(6)  << partID            << " "
	      << std::setw(7)  << weight            << " "
	      << std::setw(6)  << turnnumber
	      << G4endl;
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
