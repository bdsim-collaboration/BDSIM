
#include "TH1Set.hh"

#include "TMultiGraph.h"
#include "TGraph.h"
#include "TClass.h"
#include "TList.h"
#include "TMath.h"
#include <cassert>

ClassImp(TH1Set);


TH1Set::TH1Set()
{
  Initialize(0., 0., 0., 0., 25, 25);
  SetName("NoName");
  SetTitle("NoTitle");
}

TH1Set::TH1Set(const char* name,
	       const char* title,
	       Double_t xlow,
	       Double_t xup)
{
  Initialize(xlow, xup, ylow, yup, 25, 25);
  SetName(name);
  SetTitle(title);
}

TH1Set::~TH1Set()
{
   delete[] fCells;
   delete[] fIsEmpty;
   delete[] fCompletelyInside;
   // delete at the end the bin List since it owns the objects
   delete fBins;
}

////////////////////////////////////////////////////////////////////////////////
/// Adds a new bin to the histogram. It can be any object having the method
/// IsInside(). It returns the bin number in the histogram. It returns 0 if
/// it failed to add. To allow the histogram limits to expand when a bin
/// outside the limits is added, call SetFloat() before adding the bin.

Int_t TH1Set::AddBin(TObject *poly)
{
   auto *bin = CreateBin(poly);
   Int_t ibin = fNcells-kNOverflow;
   if(!bin) return 0;

   // If the bin lies outside histogram boundaries, then extends the boundaries.
   // Also changes the partition information accordingly
   Bool_t flag = kFALSE;
   if (fFloat) {
      if (fXaxis.GetXmin() > bin->GetXMin()) {
         fXaxis.Set(100, bin->GetXMin(), fXaxis.GetXmax());
         flag = kTRUE;
      }
      if (fXaxis.GetXmax() < bin->GetXMax()) {
         fXaxis.Set(100, fXaxis.GetXmin(), bin->GetXMax());
         flag = kTRUE;
      }
      if (fYaxis.GetXmin() > bin->GetYMin()) {
         fYaxis.Set(100, bin->GetYMin(), fYaxis.GetXmax());
         flag = kTRUE;
      }
      if (fYaxis.GetXmax() < bin->GetYMax()) {
         fYaxis.Set(100, fYaxis.GetXmin(), bin->GetYMax());
         flag = kTRUE;
      }
      if (flag) ChangePartition(fCellX, fCellY);
   } else {
      /*Implement polygon clipping code here*/
   }

   fBins->Add((TObject*) bin);
   SetNewBinAdded(kTRUE);

   // Adds the bin to the partition matrix
   AddBinToPartition(bin);

   return ibin;
}

////////////////////////////////////////////////////////////////////////////////
/// Adds a new bin to the histogram. The number of vertices and their (x,y)
/// coordinates are required as input. It returns the bin number in the
/// histogram.

Int_t TH1Set::AddBin(Int_t n, const Double_t *x, const Double_t *y)
{
   TGraph *g = new TGraph(n, x, y);
   Int_t bin = AddBin(g);
   return bin;
}

////////////////////////////////////////////////////////////////////////////////
/// Add a new bin to the histogram. The bin shape is a rectangle.
/// It returns the bin number of the bin in the histogram.

Int_t TH1Set::AddBin(Double_t x1, Double_t y1, Double_t x2, Double_t  y2)
{
   Double_t x[] = {x1, x1, x2, x2, x1};
   Double_t y[] = {y1, y2, y2, y1, y1};
   TGraph *g = new TGraph(5, x, y);
   Int_t bin = AddBin(g);
   return bin;
}

////////////////////////////////////////////////////////////////////////////////
/// Performs the operation: this = this + c1*h1.

Bool_t TH1Set::Add(const TH1 *h1, Double_t c1)
{
   Int_t bin;

   TH1Set *h1p = (TH1Set *)h1;

   // Check if number of bins is the same.
   if (h1p->GetNumberOfBins() != GetNumberOfBins()) {
      Error("Add", "Attempt to add histograms with different number of bins");
      return kFALSE;
   }

   // Check if the bins are the same.
   TList *h1pBins = h1p->GetBins();
   TH1SetBin *thisBin, *h1pBin;
   for (bin = 1; bin <= GetNumberOfBins(); bin++) {
      thisBin = (TH1SetBin *)fBins->At(bin - 1);
      h1pBin  = (TH1SetBin *)h1pBins->At(bin - 1);
      if (thisBin->GetXMin() != h1pBin->GetXMin() ||
            thisBin->GetXMax() != h1pBin->GetXMax() ||
            thisBin->GetYMin() != h1pBin->GetYMin() ||
            thisBin->GetYMax() != h1pBin->GetYMax()) {
         Error("Add", "Attempt to add histograms with different bin limits");
         return kFALSE;
      }
   }


   // Create Sumw2 if h1p has Sumw2 set
   if (fSumw2.fN == 0 && h1p->GetSumw2N() != 0) Sumw2();

   // statistics can be preserved only in case of positive coefficients
   // otherwise with negative c1 (histogram subtraction) one risks to get negative variances
   Bool_t resetStats = (c1 < 0);
   Double_t s1[kNstat] = {0};
   Double_t s2[kNstat] = {0};
   if (!resetStats) {
      // need to initialize to zero s1 and s2 since
      // GetStats fills only used elements depending on dimension and type
      GetStats(s1);
      h1->GetStats(s2);
   }
   //   get number of entries now because afterwards UpdateBinContent will change it 
   Double_t entries = TMath::Abs( GetEntries() + c1 * h1->GetEntries() );


   // Perform the Add.
   Double_t factor = 1;
   if (h1p->GetNormFactor() != 0)
      factor = h1p->GetNormFactor() / h1p->GetSumOfWeights();
   for (bin = 0; bin < fNcells; bin++) {
      Double_t y = h1p->RetrieveBinContent(bin) + c1 * h1p->RetrieveBinContent(bin);
      UpdateBinContent(bin, y);
      if (fSumw2.fN) {
         Double_t esq = factor * factor * h1p->GetBinErrorSqUnchecked(bin);
         fSumw2.fArray[bin] += c1 * c1 * factor * factor * esq;
      }
   }
   // for (bin = 1; bin <= GetNumberOfBins(); bin++) {
   //    thisBin = (TH1SetBin *)fBins->At(bin - 1);
   //    h1pBin  = (TH1SetBin *)h1pBins->At(bin - 1);
   //    thisBin->SetContent(thisBin->GetContent() + c1 * h1pBin->GetContent());
   //    if (fSumw2.fN) {
   //       Double_t e1 = factor * h1p->GetBinError(bin);
   //       fSumw2.fArray[bin] += c1 * c1 * e1 * e1;
   //    }
   // }

   // update statistics (do here to avoid changes by SetBinContent)
   if (resetStats)  {
      // statistics need to be reset in case coefficient are negative
      ResetStats();
   } else {
      for (Int_t i = 0; i < kNstat; i++) {
         if (i == 1) s1[i] += c1 * c1 * s2[i];
         else        s1[i] += c1 * s2[i];
      }
      PutStats(s1);
      SetEntries(entries);
   }
   return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// Performs the operation: this = this + c1*f1.

Bool_t TH1Set::Add(TF1 *, Double_t, Option_t *)
{
   Warning("Add","Not implement for TH1Set");
   return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// Replace contents of this histogram by the addition of h1 and h2.

Bool_t TH1Set::Add(const TH1 *, const TH1 *, Double_t, Double_t)
{
   Warning("Add","Not implement for TH1Set");
   return kFALSE;
}


////////////////////////////////////////////////////////////////////////////////
/// Make a complete copy of the underlying object.  If 'newname' is set,
/// the copy's name will be set to that name.

TObject* TH1Set::Clone(const char* newname) const
{
   // TH1::Clone relies on ::Copy to implemented by the derived class.
   // Until this is implemented, revert to the much slower default version
   // (and possibly non-thread safe).

   return TNamed::Clone(newname);
}

////////////////////////////////////////////////////////////////////////////////
/// Clears the contents of all bins in the histogram.

void TH1Set::ClearBinContents()
{
   TIter next(fBins);
   TObject *obj;
   TH1SetBin *bin;

   // Clears the bin contents
   while ((obj = next())) {
      bin = (TH1SetBin*) obj;
      bin->ClearContent();
   }

   // Clears the statistics
   fTsumw   = 0;
   fTsumw2  = 0;
   fTsumwx  = 0;
   fTsumwx2 = 0;
   fTsumwy  = 0;
   fTsumwy2 = 0;
   fEntries = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Reset this histogram: contents, errors, etc.

void TH1Set::Reset(Option_t *opt)
{
   TIter next(fBins);
   TObject *obj;
   TH1SetBin *bin;

   // Clears the bin contents
   while ((obj = next())) {
      bin = (TH1SetBin*) obj;
      bin->ClearContent();
   }

   TH2::Reset(opt);
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the bin number of the bin at the given coordinate. -1 to -9 are
/// the overflow and underflow bins.  overflow bin -5 is the unbinned areas in
/// the histogram (also called "the sea"). The third parameter can be left
/// blank.
/// The overflow/underflow bins are:
///~~~ {.cpp}
/// -1 | -2 | -3
/// -------------
/// -4 | -5 | -6
/// -------------
/// -7 | -8 | -9
///~~~
/// where -5 means is the "sea" bin (i.e. unbinned areas)

Int_t TH1Set::FindBin(Double_t x, Double_t y, Double_t)
{

   // Checks for overflow/underflow
   Int_t overflow = 0;
   if      (y > fYaxis.GetXmax()) overflow += -1;
   else if (y > fYaxis.GetXmin()) overflow += -4;
   else                           overflow += -7;
   if      (x > fXaxis.GetXmax()) overflow += -2;
   else if (x > fXaxis.GetXmin()) overflow += -1;
   if (overflow != -5) return overflow;

   // Finds the cell (x,y) coordinates belong to
   Int_t n = (Int_t)(floor((x-fXaxis.GetXmin())/fStepX));
   Int_t m = (Int_t)(floor((y-fYaxis.GetXmin())/fStepY));

   // Make sure the array indices are correct.
   if (n>=fCellX) n = fCellX-1;
   if (m>=fCellY) m = fCellY-1;
   if (n<0)       n = 0;
   if (m<0)       m = 0;

   if (fIsEmpty[n+fCellX*m]) return -5;

   TH1SetBin *bin;

   TIter next(&fCells[n+fCellX*m]);
   TObject *obj;

   // Search for the bin in the cell
   while ((obj=next())) {
      bin  = (TH1SetBin*)obj;
      if (bin->IsInside(x,y)) return bin->GetBinNumber();
   }

   // If the search has not returned a bin, the point must be on "the sea"
   return -5;
}

////////////////////////////////////////////////////////////////////////////////
/// Increment the bin containing (x,y) by 1.
/// Uses the partitioning algorithm.

Int_t TH1Set::Fill(Double_t x, Double_t y)
{
   return Fill(x, y, 1.0);
}

////////////////////////////////////////////////////////////////////////////////
/// Increment the bin containing (x,y) by w.
/// Uses the partitioning algorithm.

Int_t TH1Set::Fill(Double_t x, Double_t y, Double_t w)
{
   // see GetBinCOntent for definition of overflow bins
   // in case of weighted events store weight square in fSumw2.fArray
   // but with this indexing:
   // fSumw2.fArray[0:kNOverflow-1] : sum of weight squares for the overflow bins 
   // fSumw2.fArray[kNOverflow:fNcells] : sum of weight squares for the standard bins
   // where fNcells = kNOverflow + Number of bins. kNOverflow=9

   if (fNcells <= kNOverflow) return 0;

   // create sum of weight square array if weights are different than 1
   if (!fSumw2.fN && w != 1.0 && !TestBit(TH1::kIsNotW) )  Sumw2();   

   Int_t overflow = 0;
   if      (y > fYaxis.GetXmax()) overflow += -1;
   else if (y > fYaxis.GetXmin()) overflow += -4;
   else                           overflow += -7;
   if      (x > fXaxis.GetXmax()) overflow += -2;
   else if(x > fXaxis.GetXmin())  overflow += -1;
   if (overflow != -5) {
      fOverflow[-overflow - 1]+= w;
      if (fSumw2.fN) fSumw2.fArray[-overflow - 1] += w*w;
      return overflow;
   }

   // Finds the cell (x,y) coordinates belong to
   Int_t n = (Int_t)(floor((x-fXaxis.GetXmin())/fStepX));
   Int_t m = (Int_t)(floor((y-fYaxis.GetXmin())/fStepY));

   // Make sure the array indices are correct.
   if (n>=fCellX) n = fCellX-1;
   if (m>=fCellY) m = fCellY-1;
   if (n<0)       n = 0;
   if (m<0)       m = 0;

   if (fIsEmpty[n+fCellX*m]) {
      fOverflow[4]+= w;
      if (fSumw2.fN) fSumw2.fArray[4] += w*w;
      return -5;
   }

   TH1SetBin *bin;
   Int_t bi;

   TIter next(&fCells[n+fCellX*m]);
   TObject *obj;

   while ((obj=next())) {
      bin  = (TH1SetBin*)obj;
      // needs to account offset in array for overflow bins
      bi = bin->GetBinNumber()-1+kNOverflow;
      if (bin->IsInside(x,y)) {
         bin->Fill(w);

         // Statistics
         fTsumw   = fTsumw + w;
         fTsumw2  = fTsumw2 + w*w;
         fTsumwx  = fTsumwx + w*x;
         fTsumwx2 = fTsumwx2 + w*x*x;
         fTsumwy  = fTsumwy + w*y;
         fTsumwy2 = fTsumwy2 + w*y*y;
         if (fSumw2.fN) {
            assert(bi < fSumw2.fN);
            fSumw2.fArray[bi] += w*w;
         }
         fEntries++;

         SetBinContentChanged(kTRUE);

         return bin->GetBinNumber();
      }
   }

   fOverflow[4]+= w;
   if (fSumw2.fN) fSumw2.fArray[4] += w*w;
   return -5;
}

////////////////////////////////////////////////////////////////////////////////
/// Increment the bin named "name" by w.

Int_t TH1Set::Fill(const char* name, Double_t w)
{
   TString sname(name);

   TIter    next(fBins);
   TObject  *obj;
   TH1SetBin *bin;

   while ((obj = next())) {
      bin = (TH1SetBin*) obj;
      if (!sname.CompareTo(bin->GetPolygon()->GetName())) {
         bin->Fill(w);
         fEntries++;
         SetBinContentChanged(kTRUE);
         return bin->GetBinNumber();
      }
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Fills a 2-D histogram with an array of values and weights.
///
/// \param [in] ntimes:  number of entries in arrays x and w
///                      (array size must be ntimes*stride)
/// \param [in] x:       array of x values to be histogrammed
/// \param [in] y:       array of y values to be histogrammed
/// \param [in] w:       array of weights
/// \param [in] stride:  step size through arrays x, y and w

void TH1Set::FillN(Int_t ntimes, const Double_t* x, const Double_t* y,
                               const Double_t* w, Int_t stride)
{
   for (int i = 0; i < ntimes; i += stride) {
      Fill(x[i], y[i], w[i]);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the integral of bin contents.
/// By default the integral is computed as the sum of bin contents.
/// If option "width" or "area" is specified, the integral is the sum of
/// the bin contents multiplied by the area of the bin.

Double_t TH1Set::Integral(Option_t* option) const
{
   TString opt = option;
   opt.ToLower();

   if ((opt.Contains("width")) || (opt.Contains("area"))) {
      Double_t w;
      Double_t integral = 0.;

      TIter    next(fBins);
      TObject *obj;
      TH1SetBin *bin;
      while ((obj=next())) {
         bin       = (TH1SetBin*) obj;
         w         = bin->GetArea();
         integral += w*(bin->GetContent());
      }

      return integral;
   } else {
      return fTsumw;
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the content of the input bin
/// For the overflow/underflow/sea bins:
///~~~ {.cpp}
/// -1 | -2 | -3
/// ---+----+----
/// -4 | -5 | -6
/// ---+----+----
/// -7 | -8 | -9
///~~~
/// where -5 is the "sea" bin (i.e. unbinned areas)

Double_t TH1Set::GetBinContent(Int_t bin) const
{
   if (bin > GetNumberOfBins() || bin == 0 || bin < -kNOverflow) return 0;
   if (bin<0) return fOverflow[-bin - 1];
   return ((TH1SetBin*) fBins->At(bin-1))->GetContent();
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the value of error associated to bin number bin.
/// If the sum of squares of weights has been defined (via Sumw2),
/// this function returns the sqrt(sum of w2).
/// otherwise it returns the sqrt(contents) for this bin.
/// Bins are in range [1:nbins] and for bin < 0 in range [-9:-1] it returns errors for overflow bins.
/// See also TH1Set::GetBinContent

Double_t TH1Set::GetBinError(Int_t bin) const
{
   if (bin == 0 || bin > GetNumberOfBins() || bin < - kNOverflow) return 0;
   if (fBuffer) ((TH1*)this)->BufferEmpty();
   // in case of weighted events the sum of the weights are stored in a different way than
   // a normal histogram
   // fSumw2.fArray[0:kNOverflow-1] : sum of weight squares for the overflow bins (
   // fSumw2.fArray[kNOverflow:fNcells] : sum of weight squares for the standard bins
   //  fNcells = kNOverflow (9) + Number of bins
   if (fSumw2.fN) {
      Int_t binIndex = (bin > 0) ? bin+kNOverflow-1 : -(bin+1);
      Double_t err2 = fSumw2.fArray[binIndex];
      return TMath::Sqrt(err2);
   }
   Double_t error2 = TMath::Abs(GetBinContent(bin));
   return TMath::Sqrt(error2);
}

////////////////////////////////////////////////////////////////////////////////
/// Set the bin Error.
/// Re-implementation for TH1Set given the different bin indexing in the
/// stored squared error array.
/// See also notes in TH1::SetBinError
///
/// Bins are in range [1:nbins] and for bin < 0 in the range [-9:-1] the  errors is set for the overflow bins


void TH1Set::SetBinError(Int_t bin, Double_t error) 
{
   if (bin == 0 || bin > GetNumberOfBins() || bin < - kNOverflow) return;
   if (!fSumw2.fN) Sumw2();
   SetBinErrorOption(kNormal);
   // see comment in GetBinError for special convention of bin index in fSumw2 array
   Int_t binIndex = (bin > 0) ? bin+kNOverflow-1 : -(bin+1);
   fSumw2.fArray[binIndex] = error * error; 
}



////////////////////////////////////////////////////////////////////////////////
/// Returns the bin name.

const char *TH1Set::GetBinName(Int_t bin) const
{
   if (bin > GetNumberOfBins())  return "";
   if (bin < 0)          return "";
   return ((TH1SetBin*) fBins->At(bin-1))->GetPolygon()->GetName();
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the bin title.

const char *TH1Set::GetBinTitle(Int_t bin) const
{
   if (bin > GetNumberOfBins())  return "";
   if (bin < 0)          return "";
   return ((TH1SetBin*) fBins->At(bin-1))->GetPolygon()->GetTitle();
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the maximum value of the histogram.

Double_t TH1Set::GetMaximum() const
{
   if (fNcells <= kNOverflow) return 0;
   if (fMaximum != -1111) return fMaximum;

   TH1SetBin  *b;

   TIter next(fBins);
   TObject *obj;
   Double_t max,c;

   max = ((TH1SetBin*) next())->GetContent();

   while ((obj=next())) {
      b = (TH1SetBin*)obj;
      c = b->GetContent();
      if (c>max) max = c;
   }
   return max;
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the maximum value of the histogram that is less than maxval.

Double_t TH1Set::GetMaximum(Double_t maxval) const
{
   if (fNcells <= kNOverflow) return 0;
   if (fMaximum != -1111) return fMaximum;

   TH1SetBin  *b;

   TIter next(fBins);
   TObject *obj;
   Double_t max,c;

   max = ((TH1SetBin*) next())->GetContent();

   while ((obj=next())) {
      b = (TH1SetBin*)obj;
      c = b->GetContent();
      if (c>max && c<maxval) max=c;
   }
   return max;
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the minimum value of the histogram.

Double_t TH1Set::GetMinimum() const
{
   if (fNcells <= kNOverflow) return 0;
   if (fMinimum != -1111) return fMinimum;

   TH1SetBin  *b;

   TIter next(fBins);
   TObject *obj;
   Double_t min,c;

   min = ((TH1SetBin*) next())->GetContent();

   while ((obj=next())) {
      b = (TH1SetBin*)obj;
      c = b->GetContent();
      if (c<min) min=c;
   }
   return min;
}

////////////////////////////////////////////////////////////////////////////////
/// Returns the minimum value of the histogram that is greater than minval.

Double_t TH1Set::GetMinimum(Double_t minval) const
{
   if (fNcells <= kNOverflow) return 0;
   if (fMinimum != -1111) return fMinimum;

   TH1SetBin  *b;

   TIter next(fBins);
   TObject *obj;
   Double_t min,c;

   min = ((TH1SetBin*) next())->GetContent();

   while ((obj=next())) {
      b = (TH1SetBin*)obj;
      c = b->GetContent();
      if (c<min && c>minval) min=c;
   }
   return min;
}

////////////////////////////////////////////////////////////////////////////////
/// Initializes the TH1Set object.  This method is called by the constructor.

void TH1Set::Initialize(Double_t xlow, Double_t xup,
                      Double_t ylow, Double_t yup, Int_t n, Int_t m)
{
   Int_t i;
   fDimension = 2;  //The dimension of the histogram

   fBins   = 0;
   fNcells = kNOverflow;

   // Sets the boundaries of the histogram
   fXaxis.Set(100, xlow, xup);
   fYaxis.Set(100, ylow, yup);

   for (i=0; i<9; i++) fOverflow[i] = 0.;

   // Statistics
   fEntries = 0;   // The total number of entries
   fTsumw   = 0.;  // Total amount of content in the histogram
   fTsumw2  = 0.;  // Sum square of the weights
   fTsumwx  = 0.;  // Weighted sum of x coordinates
   fTsumwx2 = 0.;  // Weighted sum of the squares of x coordinates
   fTsumwy2 = 0.;  // Weighted sum of the squares of y coordinates
   fTsumwy  = 0.;  // Weighted sum of y coordinates

   fCellX = n; // Set the number of cells to default
   fCellY = m; // Set the number of cells to default

   // number of cells in the grid
   //N.B. not to be confused with fNcells (the number of bins) !
   fNCells = fCellX*fCellY;
   fCells  = new TList [fNCells];  // Sets an empty partition
   fStepX  = (fXaxis.GetXmax() - fXaxis.GetXmin())/fCellX; // Cell width
   fStepY  = (fYaxis.GetXmax() - fYaxis.GetXmin())/fCellY; // Cell height

   fIsEmpty = new Bool_t [fNCells]; // Empty partition
   fCompletelyInside = new Bool_t [fNCells]; // Cell is completely inside bin

   for (i = 0; i<fNCells; i++) {   // Initializes the flags
      fIsEmpty[i] = kTRUE;
      fCompletelyInside[i] = kFALSE;
   }
}





////////////////////////////////////////////////////////////////////////////////
/// Merge TH1Sets
/// Given the special nature of the TH1Set, the merge is implemented in
/// terms of subsequent TH1Set::Add calls.
Long64_t TH1Set::Merge(TCollection *coll)
{
   for (auto h2pAsObj : *coll) {
      if (!Add((TH1*)h2pAsObj, 1.)) {
         Warning("Merge", "An issue was encountered during the merge operation.");
         return 0L;
      }
   }
   return GetEntries();
}

////////////////////////////////////////////////////////////////////////////////
/// Save primitive as a C++ statement(s) on output stream out

void TH1Set::SavePrimitive(std::ostream &out, Option_t *option)
{
   out <<"   "<<std::endl;
   out <<"   "<< ClassName() <<" *";

   //histogram pointer has by default the histogram name.
   //however, in case histogram has no directory, it is safer to add a
   //incremental suffix
   static Int_t hcounter = 0;
   TString histName = GetName();
   if (!fDirectory && !histName.Contains("Graph")) {
      hcounter++;
      histName += "__";
      histName += hcounter;
   }
   const char *hname = histName.Data();

   //Construct the class initialization
   out << hname << " = new " << ClassName() << "(\"" << hname << "\", \""
       << GetTitle() << "\", " << fCellX << ", " << fXaxis.GetXmin()
       << ", " << fXaxis.GetXmax()
       << ", " << fCellY << ", " << fYaxis.GetXmin() << ", "
       << fYaxis.GetXmax() << ");" << std::endl;

   // Save Bins
   TIter       next(fBins);
   TObject    *obj;
   TH1SetBin *th2pBin;

   while((obj = next())){
      th2pBin = (TH1SetBin*) obj;
      th2pBin->GetPolygon()->SavePrimitive(out,
                                           TString::Format("th2poly%s",histName.Data()));
   }

   // save bin contents
   out<<"   "<<std::endl;
   Int_t bin;
   for (bin=1;bin<=GetNumberOfBins();bin++) {
      Double_t bc = GetBinContent(bin);
      if (bc) {
         out<<"   "<<hname<<"->SetBinContent("<<bin<<","<<bc<<");"<<std::endl;
      }
   }

   // save bin errors
   if (fSumw2.fN) {
      for (bin=1;bin<=GetNumberOfBins();bin++) {
         Double_t be = GetBinError(bin);
         if (be) {
            out<<"   "<<hname<<"->SetBinError("<<bin<<","<<be<<");"<<std::endl;
         }
      }
   }
   TH1::SavePrimitiveHelp(out, hname, option);
}

////////////////////////////////////////////////////////////////////////////////
/// Multiply this histogram by a constant c1.

void TH1Set::Scale(Double_t c1, Option_t*)
{
   for( int i = 0; i < this->GetNumberOfBins(); i++ ) {
      this->SetBinContent(i+1, c1*this->GetBinContent(i+1));
   }
   for( int i = 0; i < kNOverflow; i++ ) {
      this->SetBinContent(-i-1, c1*this->GetBinContent(-i-1) );
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Sets the contents of the input bin to the input content
/// Negative values between -1 and -9 are for the overflows and the sea

void TH1Set::SetBinContent(Int_t bin, Double_t content)
{
   if (bin > GetNumberOfBins() || bin == 0 || bin < -9 ) return;
   if (bin > 0) {
      ((TH1SetBin*) fBins->At(bin-1))->SetContent(content);
   }
   else
      fOverflow[-bin - 1] = content;

   SetBinContentChanged(kTRUE);
   fEntries++;
}

////////////////////////////////////////////////////////////////////////////////
/// When set to kTRUE, allows the histogram to expand if a bin outside the
/// limits is added.

void TH1Set::SetFloat(Bool_t flag)
{
   fFloat = flag;
}

////////////////////////////////////////////////////////////////////////////////
/// Return "true" if the point (x,y) is inside the bin of binnr.

Bool_t TH1Set::IsInsideBin(Int_t binnr, Double_t x, Double_t y)
{
   if (!fBins) return false;
   TH1SetBin* bin = (TH1SetBin*)fBins->At(binnr);
   if (!bin) return false;
   return bin->IsInside(x,y);
}

void TH1Set::GetStats(Double_t *stats) const
{
   stats[0] = fTsumw;
   stats[1] = fTsumw2;
   stats[2] = fTsumwx;
   stats[3] = fTsumwx2;
   stats[4] = fTsumwy;
   stats[5] = fTsumwy2;
   stats[6] = fTsumwxy;
}
