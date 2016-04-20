#include "EventDisplay.hh"

#include <iostream>

#include "TFile.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveGeoNode.h"

ClassImp(EventDisplay)

EventDisplay::EventDisplay(TString geoFileNameIn, TString datFileNameIn) :
  geoFileName(geoFileNameIn), datFileName(datFileNameIn)
{
  TEveManager::Create();

  this->LoadGeometry();
  this->LoadData();
}

EventDisplay::~EventDisplay()
{

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

void EventDisplay::LoadData()
{

}