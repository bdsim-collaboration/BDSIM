void evtRates() {
  TFile *f = new TFile("output.root");
  TTree *t = (TTree*)f->Get("Event");
  int elastic   = t->Draw("Trajectory.n","Trajectory.primaryHadronic()==111","goff");
  int inelastic = t->Draw("Trajectory.n","Trajectory.primaryHadronic()==121","goff");
  cout << "event-numbers: " << elastic << " " << inelastic << endl;
  gSystem->Exit(0);
}
