#include <iostream>


#include "TCanvas.h"
#include "TString.h"
#include "TLine.h"

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

  TChain *c = new TChain("Event");
  Event  *e = new Event();

  GetEvent(fileName,c,e);

  TCanvas *can = new TCanvas("c");
  c->Draw("Primary.xp:Primary.x");
  std::cout << ob->envelopeX << " " << ob->envelopeXp << std::endl;
  std::cout << ob->envelopeY << " " << ob->envelopeYp << std::endl;
  std::cout << ob->haloEnvelopeCollMinX << " " << ob->haloEnvelopeCollMaxX << std::endl;
  std::cout << ob->haloEnvelopeCollMinY << " " << ob->haloEnvelopeCollMaxY << std::endl;
  std::cout << ob->haloEnvelopeCollMinXp << " " << ob->haloEnvelopeCollMaxXp << std::endl;
  std::cout << ob->haloEnvelopeCollMinYp << " " << ob->haloEnvelopeCollMaxYp << std::endl;

  TLine *xPos = new TLine(ob->haloEnvelopeCollMaxX,-ob->envelopeXp,
                          ob->haloEnvelopeCollMaxX, ob->envelopeXp);
  xPos->Draw();

  TLine *xNeg = new TLine(ob->haloEnvelopeCollMinX,-ob->envelopeXp,
                          ob->haloEnvelopeCollMinX, ob->envelopeXp);
  xNeg->Draw();


  TLine *yPos = new TLine(-ob->envelopeX,ob->haloEnvelopeCollMaxXp,
                           ob->envelopeX,ob->haloEnvelopeCollMaxXp);
  yPos->Draw();

  TLine *yNeg = new TLine(-ob->envelopeX,ob->haloEnvelopeCollMinXp,
                           ob->envelopeX,ob->haloEnvelopeCollMinXp);
  yNeg->Draw();
}