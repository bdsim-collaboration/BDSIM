#ifndef HISTOANALYSIS_H
#define HISTOANALYSIS_H

#include "BDSOutputROOTEventHistograms.hh"

class HistoAnalysis
{
public:
  HistoAnalysis();
  ~HistoAnalysis();


protected:

  BDSOutputROOTEventHistograms *sumHistos;


  ClassDef(HistoAnalysis,1);


};


#endif
