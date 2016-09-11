#include "Compare.hh"
#include "Result.hh"
#include "ResultHistogram.hh"
#include "ResultHistogram2D.hh"
#include "ResultTree.hh"

#include <iostream>
#include <vector>

#include "TBranch.h"
#include "TFile.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TTree.h"


std::vector<Result*> Compare::Files(TFile *f1, TFile *f2)
{
  std::vector<Result*> res;
  
  // get list of keys for file 1 
  TList *f1k = f1->GetListOfKeys();
  
  // loop over keys 
  for(int i = 0; i < f1k->GetEntries(); ++i) {
    TObject *ko = f1k->At(i);
    TObject *f1o = f1->Get(ko->GetName());
    
    if(std::string(f1o->ClassName()) == "TH1D") { 
      //      std::cout << "Compare TH1D" << std::endl;
      TH1 *f1h = (TH1*)f1->Get(ko->GetName());
      TH1 *f2h = (TH1*)f2->Get(ko->GetName());
      if (!f2h)
	{
	  std::cout << "No matching histrogram named \"" << ko->GetName() << "\"" << std::endl;
	  continue;
	}
      ResultHistogram *hcr = Compare::Histograms(f1h,f2h);
      res.push_back(hcr);
    }
    else if(std::string(f1o->ClassName()) == "TTree")
      {
	TTree *f1t = (TTree*)f1->Get(ko->GetName());
	TTree *f2t = (TTree*)f2->Get(ko->GetName());
	if (!f2t)
	  {
	    std::cout << "No matching tree named \"" << ko->GetName() << "\"" << std::endl;
	    continue;
	  }
	ResultTree *tcr = Compare::Trees(f1t,f2t);
	res.push_back(tcr);
      }
  }
   
  return res;
}

ResultHistogram* Compare::Histograms(TH1* h1, TH1* h2)
{
  // Take difference between histograms
  ResultHistogram* c = new ResultHistogram();
  c->name      = h1->GetName();
  c->objtype   = "TH";
  c->h1Entries = h1->GetEntries();
  c->h2Entries = h2->GetEntries();
  c->h1NXBins  = h1->GetNbinsX();
  c->h2NXBins  = h2->GetNbinsX();
  c->h1XMean   = h1->GetMean();
  c->h2XMean   = h2->GetMean();
  c->h1XRms    = h1->GetRMS();
  c->h2XRms    = h2->GetRMS();
  c->h1Integral= h1->Integral();
  c->h2Integral= h2->Integral();

  c->chi2   = 0.0;
  int ndof = 0; 
  for(int i=0;i < h1->GetNbinsX(); i++) { 
    //    std::cout << h1->GetBinContent(i) << " " << h2->GetBinContent(i) << " " << h1->GetBinError(i) << std::endl;
    if(h1->GetBinError(i) > 0) {
      c->chi2 += std::pow(h1->GetBinContent(i)-h2->GetBinContent(i),2)/(std::pow(h1->GetBinError(i),2)+std::pow(h2->GetBinError(i),2));
      ndof++;
    }
  }
  // chi2 per dof
  c->chi2 /= ndof;

  c->iStatus = 0;
  if(c->chi2 > 40.0)
    {c->iStatus = 1;}

  return c;
}


ResultTree* Compare::Trees(TTree* t1, TTree* t2)
{ 
  ResultTree* c = new ResultTree();  
  c->name       = t1->GetName();
  c->objtype    = "TTree";
  c->t1NEntries = t1->GetEntries();
  c->t2NEntries = t2->GetEntries();

  TObjArray *oa1 = t1->GetListOfBranches(); 
  TObjArray *oa2 = t2->GetListOfBranches(); 
  
  for(int j = 0; j<oa1->GetSize(); ++j) {
    for(int i = 0; i<t1->GetEntries(); ++i) {

      double t1v = 0;
      double t2v = 0;

      TBranch *b1 = (TBranch*)((*oa1)[j]);
      TBranch *b2 = (TBranch*)((*oa2)[j]);

      b1->SetAddress(&t1v);
      b2->SetAddress(&t2v);

      t1->GetEntry(i);
      t2->GetEntry(i);

      if(std::abs((t1v - t2v )/t1v) > 0.01) {
	c->iStatus = 1;
      }
      else {
	c->iStatus = 0;
      }
    }
    
  }
  return c;
}

bool Compare::CheckResults(std::vector<Result*> results)
{
  for(auto result : results)
    {   
      if(result->iStatus != 0)
	{return true;}
    }
  return false;
}

void Compare::PrintResults(std::vector<Result*> results)
{
  for(auto result : results)
    {std::cout << *result << std::endl;}  
}

void Compare::PrintFailure(std::vector<Result*> results)
{
  for(auto result : results)
    { 
      if(result->iStatus != 0)
	{std::cout << *result << std::endl;}
    }
}  
