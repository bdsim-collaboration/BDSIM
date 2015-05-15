#include "BDSOutputASCII.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include <cmath>
#include <ctime>
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
  
  // generate filenames
  G4String basefilename = BDSExecOptions::Instance()->GetOutputFilename();
  filename = basefilename + ".txt"; //main output filename - for samplers
  G4String filenamePrimaries = basefilename + ".primaries.txt"; // for primaries
  G4String filenameELoss     = basefilename + ".eloss.txt"; //for eloss hits
  G4String filenameHistogram = basefilename + ".elosshist.txt"; // eloss histogram

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Output format ASCII" << G4endl;
  G4cout << "Output filename          : " << filename  << G4endl;
  G4cout << "Primaries filename       : " << filenamePrimaries << G4endl;
  G4cout << "Histogram filename       : " << filenameHistogram << G4endl;
  G4cout << "EnergyLoss Hits filename : " << filenameELoss     << G4endl;
#endif
  std::stringstream headerstream;
  headerstream << std::left 
	       << std::setw(6)  << "PDGID"    << " "
	       << std::setw(17) << "E[GeV]"   << " "
	       << std::setw(17) << "X[mum]"   << " "
	       << std::setw(17) << "Y[mum]"   << " "
	       << std::setw(17) << "Z[mum]"   << " "
	       << std::setw(17) << "XRel[mum]"   << " "
	       << std::setw(17) << "YRel[mum]"   << " "
	       << std::setw(17) << "ZRel[mum]"   << " "
	       << std::setw(17) << "tRel[ns]"     << " "
	       << std::setw(17) << "XGlob[mum]"   << " "
	       << std::setw(17) << "YGlob[mum]"   << " "
	       << std::setw(17) << "ZGlob[mum]"   << " "
	       << std::setw(17) << "tGlobal[ns]"     << " "
	       << std::setw(17) << "S[m]"     << " "
	       << std::setw(17) << "SReference[m]"     << " "
	       << std::setw(17) << "Xp[rad]"  << " "
	       << std::setw(17) << "Yp[rad]"  << " "
	       << std::setw(17) << "XpGlob[rad]"  << " "
	       << std::setw(17) << "YpGlob[rad]"  << " "
	       << std::setw(6)  << "NEvent"   << " "
	       << std::setw(17) << "Weight"   << " "
	       << std::setw(9)  << "ParentID" << " "
	       << std::setw(8)  << "TrackID"  << " "
	       << std::setw(5)  << "Turn"
	       << G4endl;
  G4String headerstring = headerstream.str();

  std::stringstream primaryheaderstream;
  primaryheaderstream << std::left 
	       << std::setw(6)  << "PDGID"    << " "
	       << std::setw(17) << "E[GeV]"   << " "
	       << std::setw(17) << "X[mum]"   << " "
	       << std::setw(17) << "Y[mum]"   << " "
	       << std::setw(17) << "Z[mum]"   << " "
	       << std::setw(17) << "S[m]"     << " "
	       << std::setw(17) << "tLocal[ns]"     << " "
	       << std::setw(17) << "Xp[rad]"  << " "
	       << std::setw(17) << "Yp[rad]"  << " "
	       << std::setw(6)  << "NEvent"   << " "
	       << std::setw(17) << "Weight"   << " "
	       << std::setw(9)  << "ParentID" << " "
	       << std::setw(8)  << "TrackID"  << " "
	       << std::setw(5)  << "Turn"
	       << G4endl;
  G4String primaryheaderstring = primaryheaderstream.str();

  // main output file initialisation
  ofMain.open(filename.c_str());
  ofMain       << "### BDSIM output - created "<< timestring << G4endl;
  ofMain       << headerstring;

  // primaries output file initialisation
  ofPrimaries.open(filenamePrimaries.c_str());
  ofPrimaries  << "### BDSIM primaries output - created "<< timestring << G4endl;
  ofPrimaries  << primaryheaderstring;

  // energy loss hits output file initialisation
  ofELoss.open(filenameELoss.c_str());
  ofELoss      << "### BDSIM energy loss hits output - created " << timestring <<G4endl;
  ofELoss      << primaryheaderstring;
  
  // energy loss histogram and output file initialisation
  // construct histogram
  G4double xmin, xmax, binwidth;
  G4int nbins;
  xmin     = 0.0; // need to include possible global offset in globals
  xmax     = BDSGlobalConstants::Instance()->GetSMax()/CLHEP::m;
  binwidth = BDSGlobalConstants::Instance()->GetElossHistoBinWidth();///CLHEP::m;
  nbins    = (int) ceil((xmax-xmin)/binwidth);
  xmax     = xmin + (nbins*binwidth);
  hist     = new BDSHistogram1D(xmin,xmax,nbins); //naturally in metres
  // write header info
  ofELossHistogram.open(filenameHistogram.c_str());
  ofELossHistogram << hist << " - created " << timestring << G4endl;
  ofELossHistogram << std::left << std::setprecision(20) << std::fixed
		   << std::setw(17) << "S[m]"   << " "
		   << std::setw(17) << "E[GeV]" << " "
		   << G4endl;
}

BDSOutputASCII::~BDSOutputASCII()
{
  if (ofMain.is_open()) {
    ofMain.flush();
    ofMain.close();
  }
  if (ofPrimaries.is_open()) {
    ofPrimaries.flush();
    ofPrimaries.close();
  }
  if (ofELoss.is_open()) {
    ofELoss.flush();
    ofELoss.close();
  }
  if (ofELossHistogram.is_open()) {
    ofELossHistogram.flush();
    ofELossHistogram.close();
  }
}

void BDSOutputASCII::WriteAsciiHit(std::ofstream* outfile, G4int PDGType, G4double Mom, G4double X, G4double Y, G4double Z, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken)
{
  *outfile << std::left << std::setprecision(15) 
	   << std::setw(6)  << PDGType              << " "
	   << std::setw(17) << Mom/CLHEP::GeV       << " "
	   << std::setw(17) << X/CLHEP::micrometer  << " "
	   << std::setw(17) << Y/CLHEP::micrometer  << " "
	   << std::setw(17) << Z/CLHEP::micrometer  << " "
	   << std::setw(17) << S/CLHEP::m           << " "
		   << std::setw(17) << XPrime/CLHEP::radian << " "
	   << std::setw(17) << YPrime/CLHEP::radian << " "
	   << std::setw(6)  << EventNo              << " "
	   << std::setw(17) << Weight               << " "
	   << std::setw(9)  << ParentID             << " "
	   << std::setw(8)  << TrackID              << " "
	   << std::setw(5)  << TurnsTaken
	   << G4endl;
}

void BDSOutputASCII::WriteAsciiHit(std::ofstream* outfile, G4int PDGType, G4double Mom, G4double X, G4double Y, G4double Z,G4double S, G4double tLocal, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken)
{
  *outfile << std::left << std::setprecision(15) 
	   << std::setw(6)  << PDGType              << " "
	   << std::setw(17) << Mom/CLHEP::GeV       << " "
	   << std::setw(17) << X/CLHEP::micrometer  << " "
	   << std::setw(17) << Y/CLHEP::micrometer  << " "
	   << std::setw(17) << Z/CLHEP::micrometer  << " "
	   << std::setw(17) << S/CLHEP::m           << " "
	   << std::setw(17) << tLocal/CLHEP::ns     << " "
	   << std::setw(17) << XPrime/CLHEP::radian << " "
	   << std::setw(17) << YPrime/CLHEP::radian << " "
	   << std::setw(6)  << EventNo              << " "
	   << std::setw(17) << Weight               << " "
	   << std::setw(9)  << ParentID             << " "
	   << std::setw(8)  << TrackID              << " "
	   << std::setw(5)  << TurnsTaken
	   << G4endl;
}

void BDSOutputASCII::WriteAsciiHit(std::ofstream* outfile, G4int PDGType, G4double Mom, G4double X, G4double Y, G4double Z, 
				   G4double XRel, G4double YRel, G4double ZRel, G4double tRel, 
				   G4double XGlobal,G4double YGlobal, G4double ZGlobal,G4double tGlobal,G4double S, G4double SReference,G4double XPrime, G4double YPrime, 
				   G4double XPrimeGlobal, G4double YPrimeGlobal, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken)
{
  *outfile << std::left 
	   << std::setprecision(15) 
	   << std::setw(6)  << PDGType              << " "
	   << std::setw(17) << Mom/CLHEP::GeV       << " "
	   << std::setw(17) << X/CLHEP::micrometer  << " "
	   << std::setw(17) << Y/CLHEP::micrometer  << " "
	   << std::setw(17) << Z/CLHEP::micrometer  << " "
	   << std::setw(17) << XRel/CLHEP::micrometer  << " "
	   << std::setw(17) << YRel/CLHEP::micrometer  << " "
	   << std::setw(17) << ZRel/CLHEP::micrometer  << " "
	   << std::setw(17) << tRel/CLHEP::ns           << " "
	   << std::setw(17) << XGlobal/CLHEP::micrometer  << " "
	   << std::setw(17) << YGlobal/CLHEP::micrometer  << " "
	   << std::setw(17) << ZGlobal/CLHEP::micrometer  << " "
	   << std::setw(17) << tGlobal/CLHEP::ns           << " "
	   << std::setw(17) << S/CLHEP::m           << " "
	   << std::setw(17) << SReference/CLHEP::m           << " "
	   << std::setw(17) << XPrime/CLHEP::radian << " "
	   << std::setw(17) << YPrime/CLHEP::radian << " "
	   << std::setw(17) << XPrimeGlobal/CLHEP::radian << " "
	   << std::setw(17) << YPrimeGlobal/CLHEP::radian << " "
	   << std::setw(6)  << EventNo              << " "
	   << std::setw(17) << Weight               << " "
	   << std::setw(9)  << ParentID             << " "
	   << std::setw(8)  << TrackID              << " "
	   << std::setw(5)  << TurnsTaken
	   << G4endl;
}

void BDSOutputASCII::WritePrimary(G4String /*samplerName*/, G4double E,G4double x0,G4double y0,G4double z0,G4double xp,G4double yp,G4double /*zp*/,G4double tLocal,G4double weight,G4int PDGType, G4int nEvent, G4int TurnsTaken){
  WriteAsciiHit(&ofPrimaries, PDGType, E, x0, y0, z0, /*s=*/0.0, tLocal, xp, yp, nEvent, weight, 0, 1, TurnsTaken);
  ofPrimaries.flush();
}


void BDSOutputASCII::WriteHits(BDSSamplerHitsCollection *hc)
{
  for (G4int i=0; i<hc->entries(); i++)
    {
      WriteAsciiHit(
		    &ofMain,
		    (*hc)[i]->GetPDGtype(),
		    (*hc)[i]->GetMom(),
		    (*hc)[i]->GetX(),
		    (*hc)[i]->GetY(),
		    (*hc)[i]->GetZ(),
		    (*hc)[i]->GetRelX(),
		    (*hc)[i]->GetRelY(),
		    (*hc)[i]->GetRelZ(),
		    (*hc)[i]->GetRelT(),
                    (*hc)[i]->GetGlobalX(),
		    (*hc)[i]->GetGlobalY(),
		    (*hc)[i]->GetGlobalZ(),
		    (*hc)[i]->GetTGlobal(),
		    (*hc)[i]->GetS(),
		    (*hc)[i]->GetSReference(),
		    (*hc)[i]->GetXPrime(),
		    (*hc)[i]->GetYPrime(),
		    (*hc)[i]->GetGlobalXPrime(),
		    (*hc)[i]->GetGlobalYPrime(),
		    (*hc)[i]->GetEventNo(),
		    (*hc)[i]->GetWeight(),
		    (*hc)[i]->GetParentID(),
		    (*hc)[i]->GetTrackID(),
		    (*hc)[i]->GetTurnsTaken()
		    );
    }
  ofMain.flush();
}

// write a trajectory to a root/ascii file
// TODO: ASCII file not implemented - JS
void BDSOutputASCII::WriteTrajectory(std::vector<BDSTrajectory*> &/*TrajVec*/){
  G4cout << __METHOD_NAME__ << "WARNING trajectory writing not implemented for ASCII output" << G4endl;
}

// make energy loss histo
void BDSOutputASCII::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  // In the case of ASCII output, we use our own histogram class
  // and buffer the data manually - only filling it here
  // and actually writing the data with special WriteHistogramCommand
  for (G4int i = 0; i < hc->entries(); i++)
    {
      //Fill(sposition,weight aka energy)
      hist->Fill((*hc)[i]->GetS()/CLHEP::m,(*hc)[i]->GetEnergy()/CLHEP::GeV);
      // write the hits to the eloss file
      WriteAsciiHit(
		    &ofELoss,
		    (*hc)[i]->GetPartID(),
		    (*hc)[i]->GetEnergy(),
		    (*hc)[i]->GetX(),
		    (*hc)[i]->GetY(),
		    (*hc)[i]->GetZ(),
		    (*hc)[i]->GetS(),
		    0.0,//(*hc)[i]->GetXPrime(),
		    0.0,//(*hc)[i]->GetYPrime(),
		    0,//(*hc)[i]->GetEventNo(),
		    (*hc)[i]->GetWeight(),
		    0,//(*hc)[i]->GetParentID(),
		    0,//(*hc)[i]->GetTrackID(),
		    (*hc)[i]->GetTurnsTaken()
		    );
    }
  ofELoss.flush();
}

void BDSOutputASCII::Commit()
{
  ofMain.flush();
  ofPrimaries.flush();
  ofELoss.flush();
  WriteHistogram();
  ofELossHistogram.flush();
  /// multiple file writing not implemented for ASCII
}

void BDSOutputASCII::Write()
{
  ofMain.flush();
  ofMain.close();
  ofPrimaries.flush();
  ofPrimaries.close();
  ofELoss.flush();
  ofELoss.close();
  WriteHistogram();
  ofELossHistogram.flush();
  ofELossHistogram.close();
}

void BDSOutputASCII::WriteHistogram()
{
  std::vector<BDSBin*> thebins = hist->GetBins();
  std::vector<BDSBin*>::iterator i = thebins.begin();
  ofELossHistogram << std::left << std::setprecision(15)
		   << std::setw(12) << "underflow" << thebins.front()->GetValue() << G4endl
		   << std::setw(12) << "overflow"  << thebins.back()->GetValue()  << G4endl;
  //remember .begin() is before first item, so +2 to get 2nd bin
  for (i = thebins.begin()+1; i != --thebins.end(); ++i)
    {
  //only write non zero value bins
  //    if ((*i) != 0)
  //	{
      std::pair<G4double, G4double> binvalues = (*i)->GetXMeanAndTotal();
#ifdef BDSDEBUG
      G4cout << "writing bin " << binvalues.first << " " << binvalues.second << G4endl;
#endif
      ofELossHistogram << std::left << std::setprecision(15) 
		       << std::setw(12) << binvalues.first  << " " << std::scientific
		       << std::setw(12) << binvalues.second << " "
		       << G4endl;
	  //	}
    }
  ofELossHistogram.flush();
}
