#include "TRKLineUtils.hh"

#include "TRKElement.hh"
#include "TRKLine.hh"


namespace trk
{
  double BeamlineLength(const TRKLine* line)
  {
    auto totalLength = 0.0;
    for (const auto & element: *line)
      totalLength += element->GetLength();
    return totalLength;
  }
}
