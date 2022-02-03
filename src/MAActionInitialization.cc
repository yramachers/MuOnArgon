#include "MAActionInitialization.hh"
#include "MAEventAction.hh"
#include "MAPrimaryGeneratorAction.hh"
#include "MARunAction.hh"
#include "MAStackingAction.hh"
#include "MATrackingAction.hh"

MAActionInitialization::MAActionInitialization(MADetectorConstruction* det,
                                                   G4String                  name)
: G4VUserActionInitialization()
, fDet(det)
, foutname(std::move(name))
{}

MAActionInitialization::~MAActionInitialization() = default;

void MAActionInitialization::BuildForMaster() const
{
  SetUserAction(new MARunAction(foutname));
}

void MAActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new MAPrimaryGeneratorAction(fDet));
  SetUserAction(new MAEventAction);
  SetUserAction(new MARunAction(foutname));
  SetUserAction(new MAStackingAction);
  SetUserAction(new MATrackingAction);
}
