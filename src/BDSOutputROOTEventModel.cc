#include "BDSOutputROOTEventModel.hh"

ClassImp(BDSOutputROOTEventModel);

BDSOutputROOTEventModel::BDSOutputROOTEventModel()
{
}

BDSOutputROOTEventModel::~BDSOutputROOTEventModel()
{
}

void BDSOutputROOTEventModel::Fill()
{
  // get accelerator model
  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  // iterate over flat beamline
  for(auto i = beamline->begin(); i != beamline->end(); ++i)
  {
    this->name.push_back((*i)->GetName());
    this->length.push_back((float &&) (*i)->GetAcceleratorComponent()->GetArcLength());
    auto p = (*i)->GetPositionStart();
    this->staPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetPositionMiddle();
    this->midPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetPositionEnd();
    this->endPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));

    G4RotationMatrix *gr;
    gr = (*i)->GetRotationStart();
    TRotation rr = TRotation();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->staRot.push_back(rr);
    gr = (*i)->GetRotationMiddle();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->midRot.push_back(rr);
    gr = (*i)->GetRotationEnd();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->endRot.push_back(rr);
  }
}
