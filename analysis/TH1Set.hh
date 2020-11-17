#ifndef TH1SET_H
#define TH1SET_H

#include "HistSparse1D.hh"

#include "TH1.h"


class TList;
class TGraph;
class TMultiGraph;
class TPad;

class TH1Set: public TH1
{
public:
  TH1Set();
  TH1Set(const char* name, const char* title, Double_t xlow, Double_t xup);
  virtual ~TH1Set();
  
  virtual Int_t AddBin(TObject *poly);
  Int_t         AddBin(Int_t n, const Double_t *x, const Double_t *y);
  Int_t         AddBin(Double_t x1, Double_t y1, Double_t x2, Double_t  y2);
  virtual Bool_t Add(const TH1 *h1, Double_t c1);
  virtual Bool_t Add(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1);
  virtual Bool_t Add(TF1 *h1, Double_t c1=1, Option_t *option="");
  TObject* Clone(const char* newname = "") const;
  virtual Int_t Fill(Double_t x);
  virtual Int_t Fill(Double_t x,Double_t w);
  virtual Int_t Fill(const char* name, Double_t w);
  virtual void  FillN(Int_t ntimes, const Double_t* x, const Double_t* y, const Double_t* w, Int_t stride = 1);
  virtual Int_t FindBin(Double_t x, Double_t y = 0, Double_t z = 0);
  virtual Double_t GetBinContent(Int_t bin) const;
  virtual Double_t GetBinError(Int_t bin) const;
  //Double_t GetMaximum() const;
  virtual Double_t GetMaximum(Double_t maxval) const;
  //  Double_t     GetMinimum() const;
  virtual Double_t GetMinimum(Double_t minval) const;

  virtual Double_t     Integral(Option_t* option = "") const;
  virtual Long64_t     Merge(TCollection *);

  virtual void SavePrimitive(std::ostream& out, Option_t* option = "");
  virtual void Reset(Option_t *option);
  virtual void Scale(Double_t c1 = 1, Option_t* option = "");
  virtual void SetBinContent(Int_t bin, Double_t content);
  virtual void SetBinError(Int_t bin, Double_t error);
  virtual void GetStats(Double_t *stats) const;

  //TList       *GetBins(){return fBins;}// Returns the TList of all bins in the histogram
  
protected:
  
  HistSparse1D<long long int> data;
  
  // functions not to be used
  //Int_t        Fill(Double_t){return -1;}                              //MayNotUse
  Int_t        Fill(Double_t , const char *, Double_t){return -1;}     //MayNotUse
  Int_t        Fill(const char *, Double_t , Double_t ){return -1;}    //MayNotUse
  Int_t        Fill(const char *, const char *, Double_t ){return -1;} //MayNotUse
  void         FillN(Int_t, const Double_t*, const Double_t*, Int_t){return;}  //MayNotUse
  
  Double_t     Integral(Int_t, Int_t, const Option_t*) const{return 0;}                             //MayNotUse
  Double_t     Integral(Int_t, Int_t, Int_t, Int_t, const Option_t*) const{return 0;}               //MayNotUse
  Double_t     Integral(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, const Option_t*) const{return 0;} //MayNotUse
  
  virtual Double_t     GetBinContent(Int_t, Int_t) const {return 0;}           //MayNotUse
  virtual Double_t     GetBinContent(Int_t, Int_t, Int_t) const {return 0;}    //MayNotUse
  
  virtual Double_t GetBinError(Int_t , Int_t) const {return 0;}            //MayNotUse
  virtual Double_t GetBinError(Int_t , Int_t , Int_t) const {return 0;}    //MayNotUse
  
  virtual void         SetBinContent(Int_t, Int_t, Double_t){}           //MayNotUse
  virtual void         SetBinContent(Int_t, Int_t, Int_t, Double_t){}    //MayNotUse
  virtual void         SetBinError(Int_t, Int_t, Double_t) {}
  virtual void         SetBinError(Int_t, Int_t, Int_t, Double_t) {}  
  
    enum {
      kNOverflow       = 9  //  number of overflows bins
   };
   TList   *fBins;              //List of bins. The list owns the contained objects
   Double_t fOverflow[kNOverflow];       //Overflow bins
   Int_t    fCellX;             //Number of partition cells in the x-direction of the histogram
   Int_t    fCellY;             //Number of partition cells in the y-direction of the histogram
   Int_t    fNCells;            //Number of partition cells: fCellX*fCellY
   TList   *fCells;             //[fNCells] The array of TLists that store the bins that intersect with each cell. List do not own the contained objects
   Double_t fStepX, fStepY;     //Dimensions of a partition cell
   Bool_t  *fIsEmpty;           //[fNCells] The array that returns true if the cell at the given coordinate is empty
   Bool_t  *fCompletelyInside;  //[fNCells] The array that returns true if the cell at the given coordinate is completely inside a bin
   Bool_t   fFloat;             //When set to kTRUE, allows the histogram to expand if a bin outside the limits is added.
   Bool_t   fNewBinAdded;       //!For the 3D Painter
   Bool_t   fBinContentChanged; //!For the 3D Painter

   void   Initialize(Double_t xlow, Double_t xup, Double_t ylow, Double_t yup, Int_t n, Int_t m);
   
   // needed by TH1 - no need to have a separate implementation , but internal ibin=0 is first bin. 
   virtual Double_t RetrieveBinContent(Int_t bin) const {
      return (bin>=kNOverflow) ? GetBinContent(bin-kNOverflow+1) : GetBinContent(-bin-1);
   }
   virtual void     UpdateBinContent(Int_t bin, Double_t content) {
      return (bin>=kNOverflow) ? SetBinContent(bin-kNOverflow+1,content) : SetBinContent(-bin-1,content);
   }

   ClassDef(TH1Set,1)
 };

#endif
