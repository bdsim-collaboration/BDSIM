#include "Sampler.h"
#include <iostream>
#include <string>
#include "TList.h"
#include "TFile.h"
#include "TTree.h"
//#include "TIter.h"
#include "TObject.h"
#include "TCollection.h"
//#include "TListIter.h"
#include "TKey.h"
#include <vector>

using namespace std;

void SamplerLoop();

int main()
{
  SamplerLoop();
  return 0;
}

void SamplerLoop()
{
  //  TFile f("track3_0.root");
  TFile f("st_100_0.root");
  Int_t nsamps = f.GetNkeys() - 2; // allow for h1, h1tuple
  
  cout << "Number of Samplers: " << nsamps << std::endl;
 
  TList* list = f.GetListOfKeys();
  //TIter next(f.GetListOfKeys());
 
  vector<string> names;

  TObject *obj = list->First();
  names.push_back(obj->GetName());

  TTree * tree;

  //Sampler* testsampler;

  //TObject *objn = 0;
  while ((obj = (TObject *)list->After(obj)))
    {
      names.push_back(obj->GetName());
      f.GetObject(obj->GetName(),tree);
      cout << obj->GetName() << endl;
      //Sampler* thesampler = new Sampler(tree);
      //thesampler->CalculateOpticalFunctions();
    }

  for (vector<string>::iterator i = names.begin(); i!= names.end(); ++i)
    {
      //cout << *i << endl;
    }
  
  cout << "end loop" << endl;
}

//  TFree* it = *list;
// while(it) {
