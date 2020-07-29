//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#ifndef BDSBH4DBASE_HH
#define BDSBH4DBASE_HH

#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"
#include "TObject.h"

class BDSBH4DBase : public TH1D {

public:
    ~BDSBH4DBase() override = default;

    int GetNbinsX() const final;
    int GetNbinsY() const final;
    int GetNbinsZ() const final;
    int GetNbinsE() const;
    const char* GetName() const override;
    const char* GetTitle() const override;

    void SetName(const char*) override;
    void SetTitle(const char*) override;
    void SetEntries(double) override;

    virtual BDSBH4DBase& operator+=(const BDSBH4DBase& other) = 0;
    virtual void Reset() = 0;
    BDSBH4DBase* Clone(const char*) const override = 0;
    virtual void Fill(double, double, double, double) = 0;
    virtual void Set(int, int, int, int, double) = 0;
    virtual void SetError(int, int, int, int, double) = 0;
    virtual void Add(BDSBH4DBase*) = 0;
    virtual double At(int, int, int, int) = 0;
    virtual double AtError(int, int, int, int) = 0;
    virtual double LowBinEdgeAt(int, int, int, int) = 0;
    virtual double HighBinEdgeAt(int, int, int, int) = 0;
    virtual void Print() = 0;
    virtual void Print(int, int, int, int) = 0;

    unsigned int h_nxbins;
    unsigned int h_nybins;
    unsigned int h_nzbins;
    unsigned int h_nebins;
    double h_xmin;
    double h_xmax;
    double h_ymin;
    double h_ymax;
    double h_zmin;
    double h_zmax;
    double h_emin;
    double h_emax;
    std::string h_name;
    std::string h_title;
    std::string h_escale;
    unsigned long entries = 0;

ClassDef(BDSBH4DBase,1);

};

#endif //BDSBH4DBASE_HH
