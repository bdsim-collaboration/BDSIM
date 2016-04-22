#ifndef __EVENTDISPLAY_H
#define __EVENTDISPLAY_H

#include "TROOT.h"

#include "DataLoader.hh"
#include "Event.hh"
#include "TChain.h"

class EventDisplay
{
public:
  static EventDisplay* Instance();
  EventDisplay(TString geoFileName);
  virtual ~EventDisplay();

  void ClearEvent();
  void DrawElossHits();
  void DrawTunnelHits();
  void DrawTrajectories();
  void LoadData(int iEvt);


private:
  void LoadGeometry();

  TString geoFileName    = "";

  DataLoader *dataLoader = nullptr;
  Event      *event      = nullptr;
  TChain     *eventTree  = nullptr;

  static EventDisplay *_instance;

  ClassDef(EventDisplay,1);
};

#endif
