#include "Compare.hh"
#include "Result.hh"
#include "ResultHistogram.hh"
#include "ResultHistogram2D.hh"
#include "ResultTree.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "TBranch.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TTree.h"

#define CHI2TOLERANCE 40


std::vector<Result*> Compare::Files(TFile* f1, TFile* f2)
{
  std::vector<Result*> results;
  // A TFile inherits TDirectory, so we simply use the TDirectory function.
  Compare::Directories((TDirectory*)f1, (TDirectory*)f2, results);
  return results;
}

void Compare::Directories(TDirectory* d1,
			  TDirectory* d2,
			  std::vector<Result*>& results)
{
  // record original directory in file.
  TDirectory* originalDirectory = TDirectory::CurrentDirectory();
  
  d1->cd(); // change into the directory
  
  // get list of keys for file 1 & loop over
  TList* d1k = d1->GetListOfKeys();
  for(int i = 0; i < d1k->GetEntries(); ++i)
    {
      TObject* keyObject = d1k->At(i); // key object in list of keys
      TObject* d1o       = d1->Get(keyObject->GetName()); // object in file

      std::string objectName = std::string(keyObject->GetName());
      std::string className  = std::string(d1o->ClassName());

      if (className == "TDirectory" || className == "TDirectoryFile") // recursion!
	{
	  TDirectory* subD1 = (TDirectory*)d1->Get(objectName.c_str());
	  TDirectory* subD2 = (TDirectory*)d2->Get(objectName.c_str());
	  if (!subD2)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Directories(subD1, subD2, results);
	}
      else if(className == "TH1D")
	{
	  TH1* d1h = (TH1*)d1->Get(objectName.c_str());
	  TH1* d2h = (TH1*)d2->Get(objectName.c_str());
	  if (!d2h)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Histograms(d1h, d2h, results);
	}
      else if(className == "TTree")
	{
	  TTree* d1t = (TTree*)d1->Get(objectName.c_str());
	  TTree* d2t = (TTree*)d2->Get(objectName.c_str());
	  if (!d2t)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Trees(d1t, d2t, results);
	}
    }
  originalDirectory->cd();
}

void Compare::Histograms(TH1* h1, TH1* h2, std::vector<Result*>& results)
{
  // Take difference between histograms
  ResultHistogram* c = new ResultHistogram();
  c->name      = h1->GetName();
  c->tolerance = CHI2TOLERANCE;
  c->objtype   = "TH1";
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

  c->passed = true;
  if(c->chi2 > CHI2TOLERANCE)
    {c->passed = false;}

  results.push_back(c);
}

void Compare::Trees(TTree* t1, TTree* t2, std::vector<Result*>& results)
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

      if(std::abs((t1v - t2v )/t1v) > 0.01)
	{
	  c->passed = false;
	  c->offendingBranches.push_back(std::string(b2->GetName()));
	}
      else
	{c->passed = true;}
    }
  }
  results.push_back(c);
}

bool Compare::Summarise(std::vector<Result*> results)
{
  bool allPassed = true;
  const int titleWidth = 20;
  const int fullWidth  = titleWidth + 22;
  std::cout << "Comparison: " << std::setw(titleWidth) << "Object Name" << "   " << "Result" << std::endl;
  std::cout << std::setfill('-') << std::setw(fullWidth) << " " << std::endl;
  std::cout << std::setfill(' ');
  for (const auto& result : results)
    {
      if (!(result->passed))
	{
	  allPassed = false;
	  std::cout << *result << std::endl;
	}
      else
	{std::cout << "Comparison: " << std::setw(20) << result->name << " : Passed" << std::endl;}
    }
  return allPassed;
}

bool Compare::AnyFailed(std::vector<Result*> results)
{
  for(auto result : results)
    {   
      if(!(result->passed))
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
      if(!(result->passed))
	{std::cout << *result << std::endl;}
    }
}  

void Compare::PrintNoMatching(std::string className, std::string objectName)
{
  std::cout << "Comparison file has no " << className << " called " << objectName << ". Skipping" << std::endl;
}
