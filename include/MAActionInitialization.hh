#ifndef MAActionInitialization_h
#define MAActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "MADetectorConstruction.hh"

/// Action initialization class.

class MAActionInitialization : public G4VUserActionInitialization
{
public:
  MAActionInitialization(MADetectorConstruction* det, G4String name);
  virtual ~MAActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  MADetectorConstruction*   fDet;
  G4String                  foutname;
};

#endif
