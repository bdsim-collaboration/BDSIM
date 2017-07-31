#include <vector>

BDSOutputROOTEventSampler* test() {
  TFile *f = new TFile("./output.root");
  TTree *mt =  (TTree*)f->Get("Model");
  TTree *et =  (TTree*)f->Get("Event");
  BDSOutputROOTEventModel   *me = new BDSOutputROOTEventModel();
  BDSOutputROOTEventSampler *ps = new BDSOutputROOTEventSampler();
  cout << me << endl;
  mt->SetBranchAddress("Model.",&me);
  et->SetBranchAddress("Primary.",&ps);

  mt->GetEntry(0);
  et->GetEntry(0);

  return ps;
  //  cout << me->componentName->size() << endl;
}
