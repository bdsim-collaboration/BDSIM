#include "BDSOutputVector.hh"
#include "BDSDebug.hh"

BDSOutputVector::BDSOutputVector()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

BDSOutputVector::~BDSOutputVector()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {delete output[i];}
}

void BDSOutputVector::Add(BDSOutputBase* derived)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "registering new output" << G4endl;
#endif
  output.push_back(derived);
}

void BDSOutputVector::WriteHits(BDSSamplerHitsCollection* a)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteHits(a);}
}

void BDSOutputVector::WriteEnergyLoss(BDSEnergyCounterHitsCollection* a)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteEnergyLoss(a);}
}

void BDSOutputVector::WritePrimaryLoss(BDSTrajectoryPoint* ploss)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WritePrimaryLoss(ploss);}
}

void BDSOutputVector::WritePrimaryHit(BDSTrajectoryPoint* phit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WritePrimaryHit(phit);}
}

void BDSOutputVector::WriteTunnelHits(BDSEnergyCounterHitsCollection* a)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteTunnelHits(a);}
}

void BDSOutputVector::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteTrajectory(TrajVec);}
}

void BDSOutputVector::WritePrimary(G4double E,
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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WritePrimary(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,TurnsTaken);}
}

void BDSOutputVector::WriteTrackerBunch(G4String samplerName, TRKBunch* bunch, bool primary)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteTrackerBunch(samplerName,bunch,primary);}
}

void BDSOutputVector::WriteTrackerPrimaryLoss(TRKBunch* lostBunch, int turnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteTrackerPrimaryLoss(lostBunch, turnsTaken);}
}

void BDSOutputVector::WriteHistogram(BDSHistogram1D* histogramIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteHistogram(histogramIn);}
}

void BDSOutputVector::WriteEventInfo(const time_t&  startTime,
				     const time_t&  stopTime,
				     const G4float& duration,
                                     const std::string& seedStateAtStart)
{
  for (auto out : output)
    { out->WriteEventInfo(startTime, stopTime, duration, seedStateAtStart);}
}

void BDSOutputVector::Initialise()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->Initialise();}
}

void BDSOutputVector::Write(const time_t&  startTime,
			    const time_t&  stopTime,
			    const G4float& duration,
                            const std::string& seedStateAtStart)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->Write(startTime, stopTime, duration, seedStateAtStart);}
}

void BDSOutputVector::WriteEventInfo(const BDSOutputROOTEventInfo* info)
{
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->WriteEventInfo(info);}
}  

void BDSOutputVector::Close()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (unsigned int i=0; i<output.size(); i++)
    {output[i]->Close();}
}
