#include "Sampler.h"
#include "SamplerLoop.h"

#include "TChain.h"
#include "TFile.h"
#include "TIterator.h"
#include "TList.h"
#include "TObject.h"
#include "TSystemDirectory.h"
#include "TTree.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>

int main(int argc, char* argv[])
{
  std::string ofilename;
  std::string ifilename;
  //iterate through arguments to get input and output filename
  //skip argc = 0 as this is the program name
  for (int i=1; i<argc; i+=2)
    {
      if (std::string(argv[i]) == "-i")
	{ifilename = std::string(argv[i+1]);}
      else if (std::string(argv[i]) == "-o")
	{ofilename = std::string(argv[i+1]);}
      else
	{
	  std::cout << "Incorrect input arguments" << std::endl;
	  exit(0);
	}
    }
  //currently doesn't check for right number of arguments
 
  SamplerLoop(ofilename);
  return 0;
}

void SamplerLoop(std::string outputfilename)
{
  //Get list of files
  std::vector<std::string> rootfiles = ListFiles(".",".root");
  std::cout << "Identified " << rootfiles.size() << " root files." << std::endl;

  //Open a single file to get list of trees
  std::cout << "Using " << rootfiles[0] << " as a template of Trees." << std::endl;
  TFile f(rootfiles[0].c_str());
  
  Int_t nsamps = f.GetNkeys() - 2; // allow for h1, h1tuple
  std::cout << "Number of Samplers: " << nsamps << std::endl; 

  TList* list  = f.GetListOfKeys();
  TObject *obj = list->First();
  TTree * tree;
  
  //counter for percentage feedback on number of samplers
  Int_t i = 0;
  double percentage = 0.;

  //open outputfile
  std::ofstream ofs;
  ofs.open(outputfilename.c_str());
  
  //output header
  ofs << "s\tbeta_x\tbeta_y\talph_x\talph_y\tdisp_x\t";
  ofs << "disp_xp\tdisp_y\tdisp_yp\temitt_x\temitt_y\n";
  
  // loop over the trees and for each one build a chain and do stuff
  for (int j=0; j<nsamps; j++)
    {
      //some nice feedback
      if (i%10 == 0) //slowly though so not to slow down
	{
	  percentage = i / (float)nsamps * 100.0;
	  std::cout << "\r" << std::setprecision(1) << std::fixed << percentage << "%";
	  std::cout.flush();
	}
      obj = (TObject *)list->After(obj);
      //Only do analysis on Sampler Trees - they start with 'Sampler'
      if (strncmp(obj->GetName(),"Sampler",7) == 0)
	{
	  //it's a sampler - do analysis
	  //chain all the files together
	  TChain treechain(obj->GetName());
	  treechain.Add("*.root");
	  tree = &treechain;
	  //std::cout << "Number of entries: " << tree->GetEntries() << std::endl;
	  Sampler* thesampler = new Sampler(tree);
	  thesampler->CalculateOpticalFunctions();
	  thesampler->AppendOpticalData(&ofs);
	  delete thesampler; 
	}
      i++;
    }
  std::cout << std::endl;
  ofs.close();
  std::cout << "Data written to " << outputfilename << std::endl;
}

std::vector<std::string> ListFiles(const char *dirname="C:/root/folder/", const char *ext=".root")
{
   TSystemDirectory dir(dirname, dirname);
   TList* files = dir.GetListOfFiles();
   std::vector<std::string> matchingfiles;
   if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
         fname = file->GetName();
         if (!file->IsDirectory() && fname.EndsWith(ext)) 
	   {
	     matchingfiles.push_back((std::string)fname.Data());
	     //std::cout << fname.Data() << std::endl;
	   }
      }
   }
   return matchingfiles;
}
