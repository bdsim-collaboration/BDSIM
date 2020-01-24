#ifndef LINKOPAQUEBOX_H
#define LINKOPAQUEBOX_H

#include "G4ThreeVector.hh"
#include "G4types.hh"

#include "BDSExtent.hh"

class BDSAcceleratorComponent;

class LinkOpaqueBox : BDSGeometryComponent
{
public:
  LinkOpaqueBox(BDSAcceleratorComponent*);

  BDSExtent getExtent();

  //! Default constructor
  LinkOpaqueBox() = delete;

  //! Copy constructor
  LinkOpaqueBox(const LinkOpaqueBox &other) = default;

  //! Move constructor
  LinkOpaqueBox(LinkOpaqueBox &&other) noexcept = default;

  //! Destructor
  virtual ~LinkOpaqueBox() noexcept = default;

  //! Copy assignment operator
  LinkOpaqueBox& operator=(const LinkOpaqueBox &other) = default;

  //! Move assignment operator
  LinkOpaqueBox& operator=(LinkOpaqueBox &&other) noexcept = default;


private:
  BDSAcceleratorComponent* component;
  G4int index;
  G4ThreeVector toStart;
};

#endif /* LINKOPAQUEBOX_H */
