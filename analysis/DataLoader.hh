
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Config.hh"
#include "Event.hh"

class DataLoader
{
  public :
    DataLoader();
    DataLoader(Config &config);
    void CommonCtor();
    virtual ~DataLoader();
    BDSOutputROOTEventOptions*  GetOptions();
    BDSOutputROOTEventModel*    GetModel();
    Event*                      GetEvent();

  private:
    BDSOutputROOTEventOptions *opt;
    BDSOutputROOTEventModel   *mod;
    Event                     *evt;

  ClassDef(DataLoader,1);
};