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
  G4String filenamePrimaries  = basefilename + ".primaries.txt"; // primaries
  G4String filenameELoss      = basefilename + ".eloss.txt";     // energy loss hits
  G4String filenameHistogram  = basefilename + ".elosshist.txt"; // eneryg loss histogram
  G4String filenamePLoss      = basefilename + ".ploss.txt";     // primary loss hits
  G4String filenamePHistogram = basefilename + ".plosshist.txt"; // primary loss histogram

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Output format ASCII - filenames:" << G4endl;
  G4cout << "Output                 : " << filename           << G4endl;
  G4cout << "Primaries              : " << filenamePrimaries  << G4endl;
  G4cout << "Energy Loss Histogram  : " << filenameHistogram  << G4endl;
  G4cout << "Energy Loss Hits       : " << filenameELoss      << G4endl;
  G4cout << "Primary Loss Histogram : " << filenamePHistogram << G4endl;
  G4cout << "Primary Loss Hits      : " << filenamePLoss      << G4endl;
#endif
  std::stringstream headerstream;
  headerstream << std::left << std::setprecision(10) << std::fixed
	       << std::setw(6)  << "PDGID"    << " "
	       << std::setw(15) << "E[GeV]"   << " "
	       << std::setw(15) << "X[mum]"   << " "
	       << std::setw(15) << "Y[mum]"   << " "
	       << std::setw(15) << "Z[mum]"   << " "
	       << std::setw(20) << "S[m]"     << " "
	       << std::setw(15) << "Xp[rad]"  << " "
	       << std::setw(15) << "Yp[rad]"  << " "
	       << std::setw(6)  << "NEvent"   << " "
	       << std::setw(15) << "Weight"   << " "
	       << std::setw(9)  << "ParentID" << " "
	       << std::setw(8)  << "TrackID"  << " "
	       << std::setw(5)  << "Turn"
	       << G4endl;
  G4String headerstring = headerstream.str();

  // main output file initialisation
  ofMain.open(filename.c_str());
  ofMain       << "### BDSIM output - created "<< timestring << G4endl;
  ofMain       << headerstring;

  // primaries output file initialisation
  ofPrimaries.open(filenamePrimaries.c_str());
  ofPrimaries  << "### BDSIM primaries output - created "<< timestring << G4endl;
  ofPrimaries  << headerstring;

  // energy loss hits output file initialisation
  ofELoss.open(filenameELoss.c_str());
  ofELoss      << "### BDSIM energy loss hits output - created " << timestring <<G4endl;
  ofELoss      << headerstring;

  // primary loss hits output file initialisation
  ofPLoss.open(filenamePLoss.c_str());
  ofPLoss      << "### BDSIM primary loss hits output - created " << timestring <<G4endl;
  ofPLoss      << headerstring;
  
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
  ofELossHistogram << *hist << " - created " << timestring << G4endl;
  ofELossHistogram << std::left << std::setprecision(10) << std::fixed
		   << std::setw(20) << "S[m]"   << " "
		   << std::setw(15) << "E[GeV]" << " "
		   << G4endl;
  
  // primary loss histogram and output file initialisation
  // construct histogram
  // binning is the same as eloss
  phist    = new BDSHistogram1D(xmin,xmax,nbins); //naturally in metres
  // write header info
  ofPLossHistogram.open(filenamePHistogram.c_str());
  ofPLossHistogram << *hist << " - created " << timestring << G4endl;
  ofPLossHistogram << std::left << std::setprecision(10) << std::fixed
		   << std::setw(20) << "S[m]"   << " "
		   << std::setw(15) << "Number" << " "
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
  if (ofPLoss.is_open()) {
    ofPLoss.flush();
    ofPLoss.close();
  }
  if (ofELossHistogram.is_open()) {
    ofELossHistogram.flush();
    ofELossHistogram.close();
  }
  if (ofPLossHistogram.is_open()) {
    ofPLossHistogram.flush();
    ofPLossHistogram.close();
  }
  delete hist;
  delete phist;
}

void BDSOutputASCII::WriteAsciiHit(std::ofstream* outfile, G4int PDGType, G4double Mom, G4double X, G4double Y, G4double Z, G4double S, G4double XPrime, G4double YPrime, G4int EventNo, G4double Weight, G4int ParentID, G4int TrackID, G4int TurnsTaken)
{
  *outfile << std::left << std::setprecision(10) << std::fixed
	   << std::setw(6)  << PDGType              << " "
	   << std::setw(15) << Mom/CLHEP::GeV       << " "
	   << std::setw(15) << X/CLHEP::micrometer  << " "
	   << std::setw(15) << Y/CLHEP::micrometer  << " "
	   << std::setw(15) << Z/CLHEP::micrometer  << " "
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
  WriteAsciiHit(&ofPrimaries, PDGType, E, x0, y0, z0, /*s=*/0.0, xp, yp, nEvent, weight, 0, 1, TurnsTaken);
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

void BDSOutputASCII::WritePrimaryLoss(BDSEnergyCounterHit* hit)
{
  phist->Fill(hit->GetS()/CLHEP::m); //no weighting by energy - done in external analysis

  WriteAsciiHit(
		&ofPLoss,
		hit->GetPartID(),
		hit->GetEnergy(),
		hit->GetX(),
		hit->GetY(),
		hit->GetZ(),
		hit->GetS(),
		0.0,//hit->GetXPrime(),
		0.0,//hit->GetYPrime(),
		0,//hit->GetEventNo(),
		hit->GetWeight(),
		0,//hit->GetParentID(),
		0,//hit->GetTrackID(),
		hit->GetTurnsTaken()
		);
  ofPLoss.flush();
}

void BDSOutputASCII::WritePrimaryHit(BDSEnergyCounterHit* /*hit*/)
{}

void BDSOutputASCII::Commit()
{
  ofMain.flush();
  ofPrimaries.flush();
  ofELoss.flush();
  ofPLoss.flush();
  WriteHistogram(hist,&ofELossHistogram);
  WriteHistogram(phist,&ofPLossHistogram);
  ofELossHistogram.flush();
  ofPLossHistogram.flush();
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
  ofPLoss.flush();
  ofPLoss.close();
  WriteHistogram(hist,&ofELossHistogram);
  WriteHistogram(phist,&ofPLossHistogram);
  ofELossHistogram.close();
  ofPLossHistogram.close();
}

void BDSOutputASCII::WriteHistogram(BDSHistogram1D* aHist, std::ofstream* anOf)
{
  std::vector<BDSBin*> thebins = aHist->GetBins();
  std::vector<BDSBin*>::iterator i = thebins.begin();
  *anOf << std::left << std::setprecision(10) << std::fixed
	<< std::setw(12) << "underflow" << thebins.front()->GetValue() << G4endl
	<< std::setw(12) << "overflow"  << thebins.back()->GetValue()  << G4endl;
  //remember .begin() is before first item, so +2 to get 2nd bin
  for (i = thebins.begin()+1; i != --thebins.end(); ++i)
    {
      std::pair<G4double, G4double> binvalues = (*i)->GetXMeanAndTotal();
#ifdef BDSDEBUG
      G4cout << "writing bin " << binvalues.first << " " << binvalues.second << G4endl;
#endif
      *anOf << std::left << std::setprecision(10) << std::fixed
	    << std::setw(12) << binvalues.first  << " " << std::scientific
	    << std::setw(12) << binvalues.second << " "
	    << G4endl;
    }
  anOf->flush();
}
