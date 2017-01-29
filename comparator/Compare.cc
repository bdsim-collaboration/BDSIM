#include "Compare.hh"
#include "Result.hh"
#include "ResultEvent.hh"
#include "ResultEventTree.hh"
#include "ResultHistogram.hh"
#include "ResultHistogram2D.hh"
#include "ResultSampler.hh"
#include "ResultTree.hh"

#include "analysis/Event.hh"
#include "analysis/Model.hh"

#include "BDSOutputROOTEventSampler.hh"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "TBranch.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TTree.h"

#define CHI2TOLERANCE 40
#define TREETOLERANCE 0.05
#define OPTICSSIGMATOLERANCE 10
#define EVENTTREETOLERANCE 1e-10

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
      else
	{
	  std::cout << "No comparison written for object named " << objectName
		    << " of type " << className << std::endl;
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

  // check for a bad comparison
  if (c->h1NXBins != c->h2NXBins)
    {
      c->passed = false;
      results.push_back(c);
      return;
    }

  c->chi2   = 0.0;
  int ndof = 0; 
  for(int i=0;i < h1->GetNbinsX(); i++)
    { 
      //    std::cout << h1->GetBinContent(i) << " " << h2->GetBinContent(i) << " " << h1->GetBinError(i) << std::endl;
      if(h1->GetBinError(i) > 0)
	{
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
 if (!strcmp(t1->GetName() , "optics"))
    {
      Compare::Optics(t1, t2, results);
      return;
    }
 else if (!strcmp(t1->GetName(), "Event"))
   {
     // We need the sampler names which are in the Model tree. If we have an
     // event tree, we must have a Model tree too!
     TDirectory* dir = t1->GetDirectory();
     TTree* modTree = dynamic_cast<TTree*>(dir->Get("Model"));
     if (!modTree)
       {return;} // shouldnt' really happen, but we can't compare the samplers

     Model* mod = new Model();
     mod->SetBranchAddress(modTree);
     modTree->GetEntry(0);
     std::vector<std::string> names = mod->SamplerNames();
     delete mod;
     
     Compare::EventTree(t1, t2, results, names);
     return;
   }
 else if (!strcmp(t1->GetName(), "Options"))  // ignore an Options tree
   {return;}
 else if (!strcmp(t1->GetName(), "Model"))    // ignore a Model tree
   {return;}
 else if (!strcmp(t1->GetName(), "Run"))      // ignore a Run tree
   {return;}
  
  ResultTree* c = new ResultTree();
  c->name       = t1->GetName();
  c->objtype    = "TTree";
  c->t1NEntries = (int)t1->GetEntries();
  c->t2NEntries = (int)t2->GetEntries();

  TObjArray *oa1 = t1->GetListOfBranches(); 
  TObjArray *oa2 = t2->GetListOfBranches();
  
  for(int j = 0; j<oa1->GetSize(); ++j)
    {// loop over branches
      bool branchFailed = false;
      TBranch* b1 = (TBranch*)((*oa1)[j]);
      TBranch* b2 = (TBranch*)((*oa2)[j]);
      double  t1v = 0;
      double  t2v = 0;
      b1->SetAddress(&t1v);
      b2->SetAddress(&t2v);
        for(int i = 0; i<t1->GetEntries(); ++i)
	{// loop over entries	  
	  t1->GetEntry(i);
	  t2->GetEntry(i);
	  
	  if(std::abs((t1v - t2v )/t1v) > TREETOLERANCE)
	    {
	      c->passed    = false;
	      branchFailed = true;
	    }
	  else
	    {c->passed = true;}
	}
      if (branchFailed)
	{c->offendingBranches.push_back(std::string(b2->GetName()));}
  }
  results.push_back(c);
}

void Compare::Optics(TTree* t1, TTree* t2, std::vector<Result*>& results)
{
  ResultTree* c = new ResultTree();
  c->name       = t1->GetName();
  c->passed     = true; // set default to pass
  c->objtype    = "TTree(optics)";
  c->t1NEntries = (int)t1->GetEntries();
  c->t2NEntries = (int)t2->GetEntries();

  TObjArray *oa1 = t1->GetListOfBranches(); 
  
  for(int j = 0; j<oa1->GetSize(); ++j)
    {// loop over branches
      TBranch* b1 = (TBranch*)(*oa1)[j];
      std::string branchName = std::string(b1->GetName());
      if (Compare::StringStartsWith(branchName, "Sig_"))
	{continue;} // skip this branch

      bool branchFailed = false;
      std::string errBranchName = "Sig_" + branchName;
      
      TBranch* b1err = t1->GetBranch(errBranchName.c_str());
      TBranch* b2err = t2->GetBranch(errBranchName.c_str());     
      if (!b1err || !b2err)
	{continue;} // There's no appropriate error branch - don't compare
      
      TBranch* b2 = t2->GetBranch(branchName.c_str());
      double t1v = 0;
      double t1e = 0;
      double t2v = 0;
      double t2e = 0;
      b1->SetAddress(&t1v);
      b1err->SetAddress(&t1e);
      b2->SetAddress(&t2v);
      b2err->SetAddress(&t2e);
      for(int i = 0; i<t1->GetEntries(); ++i)
	{// loop over entries
	  t1->GetEntry(i);
	  t2->GetEntry(i);
	  
	  if (!std::isnormal(t1e) || !std::isnormal(t2e))
	    {break;} // skip test when errors are 0
	  
	  // Here only one entry so ndof = 1
	  double chi2 = std::pow(t1v - t2v, 2) / (std::pow(t1e,2) + std::pow(t2e,2));
	  
	  branchFailed = chi2 > OPTICSSIGMATOLERANCE;
	  
	  if (branchFailed)
	    {
	      std::cout << t1v << " " << t2v << " " << t1e << " "
			<< t2e << " " << chi2 << std::endl;
	      break; // skip testing rest of branch entries
	    }
	}
      if (branchFailed)
	{
	  std::cout << "Branch was " << branchName << std::endl << std::endl;
	  c->offendingBranches.push_back(branchName);
	  c->passed = false;
	}
    }
  results.push_back(c);
}

void Compare::EventTree(TTree* t1, TTree* t2, std::vector<Result*>& results,
			std::vector<std::string> samplerNames)
{
  ResultEventTree* ret = new ResultEventTree();
  ret->name            = t1->GetName();
  ret->passed          = true; // set deafault to pass
  ret->objtype         = "TTree(Event)";
  ret->t1NEntries      = (int)t1->GetEntries();
  ret->t2NEntries      = (int)t2->GetEntries();

  // Don't proceed if uneven number of entries of the even tree
  // ie different number of events
  if (ret->t1NEntries != ret->t2NEntries)
    {
      ret->passed = false;
      results.push_back(ret);
      return;
    }

  Event* evtLocal1 = new Event();
  Event* evtLocal2 = new Event();
  evtLocal1->SetBranchAddress(t1, &samplerNames);
  evtLocal2->SetBranchAddress(t2, &samplerNames);

  for (auto i = 0; i < t1->GetEntries(); i++)
    {
      ResultEvent* re = new ResultEvent();
      re->name    = std::to_string(i);
      re->passed  = true; // default true
      re->objtype = "Event of Event Tree";
      
      t1->GetEntry(i);
      t2->GetEntry(i);

      Compare::Sampler(evtLocal1->GetPrimaries(), evtLocal2->GetPrimaries(), re);
      for (auto j = 0; j < (int)evtLocal1->samplers.size(); j++)
	{
	  Compare::Sampler(evtLocal1->samplers[j], evtLocal2->samplers[j], re);
	}

      ret->eventResults.push_back(*re);
      if (!re->passed)
	{
	  ret->passed = false;
	  break;
	}
    }
  results.push_back(ret);

  delete evtLocal1;
  delete evtLocal2;
}

#ifdef __ROOTDOUBLE__
void Compare::Sampler(BDSOutputROOTEventSampler<double>* e1,
		      BDSOutputROOTEventSampler<double>* e2,
		      ResultEvent* re)
#else
void Compare::Sampler(BDSOutputROOTEventSampler<float>* e1,
		      BDSOutputROOTEventSampler<float>* e2,
		      ResultEvent* re)
#endif
{
  ResultSampler rs(e1->samplerName);
  
  if (e1->n != e2->n)
    {
      rs.passed = false;
      rs.offendingLeaves.push_back("n");
    }
  else
    {
      for (int i = 0; i < e1->n; i++)
	{
	  if (Diff(e1->x, e2->x, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("x");}
	  if (Diff(e1->y, e2->y, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("y");}
	  if (Diff(e1->z, e2->z))
	    {rs.passed = false; rs.offendingLeaves.push_back("z");}
	  if (Diff(e1->xp, e2->xp, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("xp");}
	  if (Diff(e1->yp, e2->yp, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("yp");}
	  if (Diff(e1->zp, e2->zp, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("zp");}
	  if (Diff(e1->t, e2->t, i))
	    {rs.passed = false; rs.offendingLeaves.push_back("t");}
	  if (Diff(e1->S, e2->S))
	    {rs.passed = false; rs.offendingLeaves.push_back("S");}
	}
    }

  // update parent result status
  if (!rs.passed)
    {re->passed = false;}
  re->samplerResults.push_back(rs);
}

#ifdef __ROOTDOUBLE__
bool Compare::Diff(const std::vector<double>& v1, const std::vector<double>& v2, int i)
#else
bool Compare::Diff(const std::vector<float>& v1, const std::vector<float>& v2, int i)
#endif
{
  return std::abs(v1[i] - v2[i]) > EVENTTREETOLERANCE;
}

#ifdef __ROOTDOUBLE__
bool Compare::Diff(const double& v1, const double& v2)
#else
bool Compare::Diff(const float& v1, const float& v2)
#endif
{
  return std::abs(v1 - v2) > EVENTTREETOLERANCE;
}

bool Compare::Summarise(std::vector<Result*> results)
{
  bool allPassed = true;
  const int titleWidth = 20;
  const int fullWidth  = titleWidth + 22;
  std::cout << std::endl;
  std::cout << "Comparison: " << std::setw(titleWidth) << "Object Name" << "   "
	    << "Result" << std::endl;
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

void Compare::PrintNoMatching(std::string className, std::string objectName)
{
  std::cout << "Comparison file has no " << className << " called " << objectName << ". Skipping" << std::endl;
}

bool Compare::StringStartsWith(std::string aString, std::string prefix)
{
  try
    {
      if (aString.compare(0, prefix.length(), prefix) == 0)
	{return true;}
      else
	{return false;}
    }
  catch(const std::out_of_range&)
    {return false;} // if string isn't as long as prefix
  return false; // for static analysis warning
}
