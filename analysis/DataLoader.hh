
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Config.hh"
#include "Event.hh"

class DataLoader
{
  public :
    DataLoader();
    DataLoader(Config &config);
    virtual ~DataLoader();

  private:
    BDSOutputROOTEventOptions *opt;
    BDSOutputROOTEventModel   *mod;
    Event                     *evt;

  ClassDef(DataLoader,1);
};