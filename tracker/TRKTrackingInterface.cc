#include "TRKTrackingInterface.hh"

TRKTrackingInterface::TRKTrackingInterface() {
  trackingSteps = DEFAULT_TRACKING_STEPS;
  type = thin;
}

TRKTrackingInterface::TRKTrackingInterface(TRKType typeIn, int trackingStepsIn) : 
  type(typeIn), trackingSteps(trackingStepsIn) {
}

TRKTrackingInterface::~TRKTrackingInterface() { 
}

