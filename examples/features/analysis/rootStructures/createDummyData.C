{
  DataDummy* d = new DataDummy();
  TFile* f = d->CreateEmptyRebdsimFile("dummyTestFile.root", 2);
  d->CreateDirectories(f,"Event.");
}
