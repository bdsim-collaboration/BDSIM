#include "EventDisplay.hh"

#include <iostream>

#include "TFile.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveGeoNode.h"
#include "TEvePointSet.h"
#include "TEveBoxSet.h"
#include "TEveLine.h"
#include "TEveScene.h"
#include "TEveEventManager.h"

ClassImp(EventDisplay)

EventDisplay* EventDisplay::_instance = nullptr;


EventDisplay* EventDisplay::Instance()
{
  return _instance;
}

EventDisplay::EventDisplay(TString geoFileNameIn) :
  geoFileName(geoFileNameIn)
{
  TEveManager::Create();
  dataLoader = new DataLoader();
  event      = dataLoader->GetEvent();
  eventTree  = dataLoader->GetEventTree();

  this->LoadGeometry();
  this->LoadData(0);
  this->DrawElossHits();
  this->DrawTunnelHits();
  this->DrawTrajectories();

  _instance = this;
}

EventDisplay::~EventDisplay()
{
  delete dataLoader;
}

void EventDisplay::LoadGeometry()
{
  TFile::SetCacheFileDir(".");
  std::cout << this->geoFileName.Data() << std::endl;
  gGeoManager = gEve->GetGeometry(this->geoFileName.Data());
  gGeoManager->DefaultColors();

  TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  gEve->AddGlobalElement(tn);
  gEve->FullRedraw3D(kTRUE);
}

void EventDisplay::LoadData(int i)
{
  eventTree->GetEntry(i);
  this->DrawElossHits();
  this->DrawTunnelHits();
  this->DrawTrajectories();
}

void EventDisplay::ClearEvent()
{
  gEve->GetCurrentEvent()->DestroyElements();
}

void EventDisplay::DrawElossHits()
{
  TEvePointSet *ps = new TEvePointSet();
  for(int i=0;i<(int)event->eloss->X.size();++i)
  {
    ps->SetNextPoint(event->eloss->X[i]*100.0,
                     event->eloss->Y[i]*100.0,
                     event->eloss->Z[i]*100.0);
  }
  ps->SetMainColor(kWhite);
  gEve->AddElement(ps);
  gEve->Redraw3D();
}

void EventDisplay::DrawTunnelHits()
{
#if 0
  TEvePointSet *ps = new TEvePointSet();
  for(int i=0;i<(int)event->tunnelHit->X.size();++i)
  {
    ps->SetNextPoint(event->tunnelHit->X[i]*100.0,
                     event->tunnelHit->Y[i]*100.0,
                     event->tunnelHit->Z[i]*100.0);
  }
  ps->SetMainColor(kWhite);
  gEve->AddElement(ps);
  gEve->Redraw3D();
#endif

  TEveBoxSet *bs = new TEveBoxSet("boxset");
  bs->Reset(TEveBoxSet::kBT_AABox, kFALSE,64);

  for(int i=0;i<(int)event->tunnelHit->X.size();++i)
  {
    bs->AddBox(event->tunnelHit->X[i]*100.0,
               event->tunnelHit->Y[i]*100.0,
               event->tunnelHit->Z[i]*100.0,
               -event->tunnelHit->energy[i]*1000.0,
               -event->tunnelHit->energy[i]*1000.0,
               -event->tunnelHit->energy[i]*1000.0);
  }
  bs->RefitPlex();

  bs->SetMainColor(kWhite);
  gEve->AddElement(bs);
  gEve->Redraw3D();
}

void EventDisplay::DrawTrajectories()
{
  std::cout << "EventDisplay::DrawTrajectories>" << std::endl;
  std::cout << "EventDisplay::DrawTrajectories> ntraj=" << event->trajectory->trajectories.size() << std::endl;
  for(auto t : event->trajectory->trajectories) {
    TEveLine *et = new TEveLine();
//    std::cout << "EventDisplay::DrawTrajectories> ntrajPoint=" << t.size() << std::endl;
    for(auto tp : t) {
      //std::cout << tp.x() << " " << tp.y() << " " << tp.z() << std::endl;
      et->SetNextPoint(tp.x()*100,
                       tp.y()*100,
                       tp.z()*100);
    }
    et->SetMainColor(kWhite);
    gEve->AddElement(et);
    gEve->Redraw3D();
  }
}