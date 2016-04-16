#include <iostream>


#include "TString.h"

#include "BDSOutputROOTEventOptions.hh"
#include "parser/OptionsBase.h"

#include "Utility.H"

void DrawGaussTwissEllipse(TString fileName)
{
  BDSOutputROOTEventOptions *o = GetOptions(fileName);
}

void DrawHalo(TString fileName)
{
  BDSOutputROOTEventOptions *o = GetOptions(fileName);
  GMAD::OptionsBase *ob = (GMAD::OptionsBase*)o;

  std::cout << ob->envelopeX << " " << ob->envelopeXp << std::endl;
  std::cout << ob->envelopeY << " " << ob->envelopeYp << std::endl;
}