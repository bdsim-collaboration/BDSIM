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

  void Draw();
  void DrawModel();
  void DrawElossHits();
  void DrawTunnelHits();
  void DrawSamplers();
  void DrawTrajectories();

  void LoadOptions(int iOpt);
  void LoadModel(int iMod);
  void LoadData(int iEvt);


private:
  void LoadGeometry();

  TString geoFileName    = "";

  DataLoader *dataLoader             = nullptr;
  Event      *event                  = nullptr;
  TChain     *eventTree              = nullptr;
  BDSOutputROOTEventOptions *options = nullptr;
  TChain     *optionsTree            = nullptr;
  BDSOutputROOTEventModel *model     = nullptr;
  TChain     *modelTree              = nullptr;

  static EventDisplay *_instance;

  ClassDef(EventDisplay,1);
};

#endif
