#include <iostream>

void compareseeds(int eventNumber)
{
  TFile* f  = new TFile("./output.root");
  TTree* et = (TTree*)f->Get("Event");
  BDSOutputROOTEventInfo* origInfo = new BDSOutputROOTEventInfo();
  BDSOutputROOTEventSampler<float>* prims = new BDSOutputROOTEventSampler<float>;

  et->SetBranchAddress("Info.", &origInfo);
  et->SetBranchAddress("Primary.", &prims);
  et->GetEntry(eventNumber);

  double originalX = prims->x[0];

  std::ofstream originalSeed;
  originalSeed.open("originalSeed.dat");
  originalSeed << origInfo->seedStateAtStart;
  originalSeed.close();
  
  f->Close();
  delete f;
  delete origInfo;

  TFile* f2 = new TFile("recreation1.root");
  TTree* et2 = (TTree*)f2->Get("Event");
  BDSOutputROOTEventInfo* newInfo = new BDSOutputROOTEventInfo();
  BDSOutputROOTEventSampler<float>* prims2 = new BDSOutputROOTEventSampler<float>;
  
  et2->SetBranchAddress("Info.", &newInfo);
  et2->SetBranchAddress("Primary.", &prims2);
  et2->GetEntry(0); // should be repeat of event '3'

  std::ofstream recreateSeed;
  recreateSeed.open("recreateSeed.dat");
  recreateSeed << newInfo->seedStateAtStart;
  recreateSeed.close();

  std::cout << "Old Primary.x " << originalX << std::endl;
  std::cout << "New Primary.x " << prims2->x[0] << std::endl;

  f2->Close();
  delete f2;
  delete newInfo;
  delete prims2;
}
