#include "BDSBeamlineEndPieceBuilder.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals

void BDS::BuildEndPieceBeamline()
{
  // the beamline of end pieces to be placed.
  BDSBeamline* endPieces = new BDSBeamline();

  // the main beam line
  auto beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  // references to first and last item for checking
  const BDSBeamlineElement* firstItem = beamline->GetFirstItem();
  const BDSBeamlineElement* lastItem  = beamline->GetLastItem();
  
  // loop over main beamline
  G4int currentIndex = 0;
  for (auto element : *beamline)
    {
      const auto accComponent   = element->GetAcceleratorComponent();
      const auto endPieceBefore = accComponent->EndPieceBefore();
      const auto endPieceAfter  = accComponent->EndPieceAfter();
      
      G4bool placeBefore = true;
      G4bool placeAfter  = true;
      if (!endPieceBefore && !endPieceAfter)
	{// no end piece on either side - skip this element
	  currentIndex++;
	  continue;
	}
      else if (!endPieceBefore)
	{placeBefore = false;}
      else if (!endPieceAfter)
	{placeAfter  = false;}

      G4double requiredBeforeLength = 0; // required length for previous end piece
      G4double requiredAfterLength  = 0; // required length for next end piece
      if (endPieceBefore)
	{requiredBeforeLength = endPieceBefore->GetChordLength();}
      if (endPieceAfter)
	{requiredAfterLength  = endPieceAfter->GetChordLength();}

      if (placeBefore)
	{
	  // search behind current element until we hit a previous component that's not a drift.
	  // ie work out the amount of drift space available. If we hit a previous element, check
	  // if it has an end piece and take that length out of the available space. if there's then
	  // available space for this end piece we place it.
	  G4bool keepGoing = true;
	  BDSBeamlineElement* inspectedElement = element; // start with current element
	  G4double             availableLength = 0;
	  G4double   previousNonDriftEndPieceL = 0;
	  G4bool              driftIsFirstItem = false;
	  while(keepGoing)
	    {
	      inspectedElement = beamline->GetPrevious(inspectedElement);
	      if (inspectedElement)
		{ // there is a previous element - inspect it
		  if (inspectedElement->GetType() == "drift") // leave keepGoing true here to keep going
		    {availableLength += inspectedElement->GetChordLength();}
		  else
		    {
		      keepGoing = false;
		      auto previousNonDriftAccCompEndPiece = inspectedElement->GetAcceleratorComponent()->EndPieceAfter();
		      if (previousNonDriftAccCompEndPiece)
			{previousNonDriftEndPieceL = previousNonDriftAccCompEndPiece->GetChordLength();}
		    } // not a drift - can't be used
		  if (availableLength > requiredBeforeLength)
		    {keepGoing = false;} // break out if we've got enough space
		}
	      else
		{keepGoing = false;} // quit the loop - no previous element
	    }
	  
	  // don't place if there isn't enough space for BOTH endpieces - the previous one and this one
	  if (requiredBeforeLength > (availableLength - previousNonDriftEndPieceL))
	    {placeBefore = false;}

	  // place the last item anyway even if there technically isn't space - either too short
	  // a drift or the magnet is first in the line
	  if ( (element == firstItem) || driftIsFirstItem )
	    {placeBefore = true;}
	  
	  if (placeBefore)
	    { // provide a BDSBeamlineElement for the end piece w.r.t. the original beam line
	      auto beforeEl = beamline->ProvideEndPieceElementBefore(endPieceBefore,
								     currentIndex);
	      endPieces->AddBeamlineElement(beforeEl); // append to end piece beam line
	    }
	}

      if (placeAfter)
	{
	  // search after current element until we hit another component that's not a drift.
	  // ie work out the amount of drift space available. If we hit another element, check
	  // if it has an end piece and take that length out of the available space. if
	  // there's then available space for this end piece we place it.
	  G4bool keepGoing = true;
	  BDSBeamlineElement* inspectedElement = element; // start with current element
	  G4double             availableLength = 0;
	  G4double       nextNonDriftEndPieceL = 0;
	  G4bool               driftIsLastItem = false;
	  while(keepGoing)
	    {
	      inspectedElement = beamline->GetNext(inspectedElement);
	      if (inspectedElement)
		{ // there is a previous element - inspect it
		  if (inspectedElement->GetType() == "drift") // leave keepGoing true here to keep going
		    {
		      availableLength += inspectedElement->GetChordLength();
		      driftIsLastItem = inspectedElement == lastItem;
		    }
		  else
		    {
		      keepGoing = false;
		      auto nextNonDriftAccCompEndPiece = inspectedElement->GetAcceleratorComponent()->EndPieceBefore();
		      if (nextNonDriftAccCompEndPiece)
			{nextNonDriftEndPieceL = nextNonDriftAccCompEndPiece->GetChordLength();}
		    } // not a drift - can't be used
		  if (availableLength > requiredAfterLength)
		    {keepGoing = false;} // break out if we've got enough space
		}
	      else
		{keepGoing = false;} // quit the loop - no previous element
	    }

	  // don't place if there isn't enough space for BOTH endpieces - this one and the upcoming one
	  if (requiredBeforeLength > (availableLength - nextNonDriftEndPieceL))
	    {placeAfter = false;}
	  
	  // place the last item anyway even if there technically isn't space - either too short
	  // a drift or the magnet is last in the line
	  if ( (element == lastItem) || driftIsLastItem )
	    {placeAfter = true;}

	  if (placeAfter)
	    { // provide a BDSBeamlineElement for the end piece w.r.t. the original beam line
	      auto afterEl = beamline->ProvideEndPieceElementAfter(endPieceAfter,
								   currentIndex);
	      endPieces->AddBeamlineElement(afterEl); // append to end piece beam line
	    }
	}
      currentIndex++; // increment iterator index on beamline
    }
  BDSAcceleratorModel::Instance()->RegisterEndPieceBeamline(endPieces);
}
