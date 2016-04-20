#ifndef __EVENTDISPLAY_H
#define __EVENTDISPLAY_H

#include "TROOT.h"

class EventDisplay
{
public:
  EventDisplay(TString geoFileName, TString datFileName);
  virtual ~EventDisplay();

private:
  void LoadGeometry();
  void LoadData();

  TString geoFileName = "";
  TString datFileName = "";

  ClassDef(EventDisplay,1);
};

#endif

#if 0
void eve(TString fileName)
{
  TEveManager::Create();
  TFile::SetCacheFileDir(".");
  gGeoManager = gEve->GetGeometry(fileName.Data());
  gGeoManager->DefaultColors();

  TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  gEve->AddGlobalElement(tn);
  gEve->FullRedraw3D(kTRUE);
}
#endif