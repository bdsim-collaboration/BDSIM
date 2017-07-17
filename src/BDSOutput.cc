#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSSamplerHit.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "parser/OptionsBase.h"

#include <ostream>
#include <set>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "Eloss",
  "PrimaryFirstHit", "PrimaryLastHit", "TunnelHit",
  "Trajectory"
};

BDSOutput::BDSOutput(G4String fileName,
		     G4int    fileNumberOffset):
  BDSOutputStructures(BDSGlobalConstants::Instance()),
  filename(fileName),
  outputFileNumber(fileNumberOffset)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  writePrimaries     = g->WritePrimaries();
}

void BDSOutput::InitialiseGeometryDependent()
{
  BDSOutputStructures::InitialiseGeometryDependent();
}

void BDSOutput::FillOptions(const GMAD::OptionsBase* options)
{
  *optionsOutput = BDSOutputROOTEventOptions(options);
  WriteOptions();
  ClearStructuresOptions();
}

void BDSOutput::FillModel()
{
  modelOutput->Fill();
  WriteModel();
  ClearStructuresModel();
}

void BDSOutput::FillPrimary(const G4PrimaryVertex* vertex,
			    const G4int            eventNumber,
			    const G4int            turnsTaken)
{
  const G4PrimaryParticle* primaryParticle = vertex->GetPrimary();
  G4ThreeVector momDir  = primaryParticle->GetMomentumDirection();
  G4double      E       = primaryParticle->GetTotalEnergy();
  G4double      xp      = momDir.x();
  G4double      yp      = momDir.y();
  G4double      zp      = momDir.z();
  G4double      x0      = vertex->GetX0();
  G4double      y0      = vertex->GetY0();
  G4double      z0      = vertex->GetZ0();
  G4double      t       = vertex->GetT0();
  G4double      weight  = primaryParticle->GetWeight();
  G4int         PDGType = primaryParticle->GetPDGcode();

  FillPrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, eventNumber, turnsTaken);
}

void BDSOutput::FillEventPrimaryOnly(G4double E,
				     G4double x0,
				     G4double y0,
				     G4double z0,
				     G4double xp,
				     G4double yp,
				     G4double zp,
				     G4double t,
				     G4double weight,
				     G4int    PDGType,
				     G4int    eventNumber,
				     G4int    turnsTaken)
{
  FillPrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, eventNumber, turnsTaken);
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::FillEvent(const BDSEventInfo*                   info,
			  const G4PrimaryVertex*                vertex,
			  const BDSSamplerHitsCollection*       samplerHitsPlane,
			  const BDSSamplerHitsCollection*       samplerHitsCylinder,
			  const BDSEnergyCounterHitsCollection* energyLoss,
			  const BDSEnergyCounterHitsCollection* tunnelLoss,
			  const BDSTrajectoryPoint*             primaryHit,
			  const BDSTrajectoryPoint*             primaryLoss,
			  const std::vector<BDSTrajectory*>&    trajectories,
			  const G4int                           turnsTaken)
{
  if (info)
    {FillEventInfo(info);}
  if (vertex)
    {FillPrimary(vertex, info->GetInfo()->index, turnsTaken);}
  if (samplerHitsPlane)
    {FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::plane);}
  if (samplerHitsCylinder)
    {FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::cylinder);}
  if (energyLoss)
    {FillEnergyLoss(energyLoss, BDSOutput::LossType::energy);}
  if (tunnelLoss)
    {FillEnergyLoss(tunnelLoss, BDSOutput::LossType::tunnel);}
  if (primaryHit)
    {FillPrimaryHit(primaryHit);}
  if (primaryLoss)
    {FillPrimaryLoss(primaryLoss);}
  FillTrajectories(trajectories);
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::CloseAndOpenNewFile()
{
  CloseFile();
  NewFile();
  InitialiseGeometryDependent();
}

void BDSOutput::FillRun(const BDSEventInfo* info)
{
  FillRunInfo(info);
  WriteFileRunLevel();
  ClearStructuresRunLevel();
}
 
G4String BDSOutput::GetNextFileName()
{
  outputFileNumber++;
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  // Base root file name 
  G4String basefilename = filename;
  basefilename = basefilename+std::string("_event");

  // if more than one file add number (starting at 0)
  if (numberEventPerFile>0 && globalConstants->NGenerate()>numberEventPerFile)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + std::string(".root");

  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch())
    {
      // check if file exists
      int nTimeAppended = 1;
      while (BDS::FileExists(filename))
	{
	  // if exists remove trailing .root
	  filename = basefilename + std::string("-") + std::to_string(nTimeAppended);
	  filename += ".root";
	  nTimeAppended +=1;
	}
    }
  G4cout << __METHOD_NAME__ << "Setting up new file: "<<filename<<G4endl;

  return filename;
}

bool BDSOutput::InvalidSamplerName(const G4String& samplerName)
{
  return protectedNames.find(samplerName) != protectedNames.end();
}

void BDSOutput::PrintProtectedNames(std::ostream& out)
{
  out << "Protected names for output " << G4endl;
  for (auto key : protectedNames)
    {out << "\"" << key << "\"" << G4endl; }
}

void BDSOutput::FillPrimary(G4double E,
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
			    G4int    turnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  if (WritePrimaries())
    {primary->Fill(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,turnsTaken,0 /* always first element */);}
}

void BDSOutput::FillEventInfo(const BDSEventInfo *info)
{
  if (info)
    {*evtInfo = *(info->GetInfo());}
}

void BDSOutput::FillSamplerHits(const BDSSamplerHitsCollection *hits,
				const BDSOutput::HitsType)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hc->entries() << std::endl;
#endif
  for (int i=0; i<hits->entries(); i++)
    {
      G4int samplerID = (*hits)[i]->GetSamplerID();
      if (WritePrimaries())
        {samplerID += 1;} // offset index by one
      samplerTrees[samplerID]->Fill((*hits)[i]);
    }
}

void BDSOutput::FillEnergyLoss(const BDSEnergyCounterHitsCollection *hits,
			       const LossType lType)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4int n_hit = hits->entries();
  for(G4int i=0;i<n_hit;i++)
  {
    BDSEnergyCounterHit* hit = (*hits)[i];
    switch (lType)
      {
      case BDSOutput::LossType::energy:
	{eLoss->Fill(hit); break;}
      case BDSOutput::LossType::tunnel:
	{tHit->Fill(hit); break;}
      default:
	{eLoss->Fill(hit); break;}
      }
    G4double sHit = hit->GetSHit()/CLHEP::m;
    G4double eW   = hit->GetEnergyWeighted()/CLHEP::GeV;
    runHistos->Fill1DHistogram(2, sHit, eW);
    evtHistos->Fill1DHistogram(2, sHit, eW);
    runHistos->Fill1DHistogram(5, sHit, eW);
    evtHistos->Fill1DHistogram(5, sHit, eW);
    if (useScoringMap)
    {
      G4double x = hit->Getx()/CLHEP::m;
      G4double y = hit->Gety()/CLHEP::m;
      evtHistos->Fill3DHistogram(0, x, y, sHit, eW);
    }
  }
}

void BDSOutput::FillPrimaryHit(const BDSTrajectoryPoint *phit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pFirstHit->Fill(phit);
  const G4double preStepSPosition = phit->GetPreS() / CLHEP::m;
  runHistos->Fill1DHistogram(0, preStepSPosition);
  evtHistos->Fill1DHistogram(0, preStepSPosition);
  runHistos->Fill1DHistogram(3, preStepSPosition);
  evtHistos->Fill1DHistogram(3, preStepSPosition);
}

void BDSOutput::FillPrimaryLoss(const BDSTrajectoryPoint *ploss)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pLastHit->Fill(ploss);
  const G4double postStepSPosition = ploss->GetPostS() / CLHEP::m;
  runHistos->Fill1DHistogram(1, postStepSPosition);
  evtHistos->Fill1DHistogram(1, postStepSPosition);
  runHistos->Fill1DHistogram(4, postStepSPosition);
  evtHistos->Fill1DHistogram(4, postStepSPosition);
}

void BDSOutput::FillTrajectories(const std::vector<BDSTrajectory*>& trajectories)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " ntrajectory=" << trajVec.size() << G4endl;
#endif
  traj->Fill(trajectories);
}

void BDSOutput::FillRunInfo(const BDSEventInfo *info)
{
  if (info)
    {*runInfo = BDSOutputROOTEventRunInfo(info->GetInfo());}
}
