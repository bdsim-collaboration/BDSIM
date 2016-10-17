#include "BDSProgressBar.hh"

BDSProgressBar::BDSProgressBar(double maxCountIn):
  maxCount(maxCountIn),
  nTicMarksInBar(50),
  countsPerTicMark(maxCountIn)
{
  countsPerTicMark = maxCount / nTicMarksInBar;
  Reset();
}

void BDSProgressBar::Reset()
{
  ticMark          = 0;
  nextTicMarkCount = 0;  
  count            = 0;

  std::cout << std::endl;
  if (!maxCount)
    {maxCount = 1;}
}

