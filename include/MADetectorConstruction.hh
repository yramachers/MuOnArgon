#ifndef MADetectorConstruction_h
#define MADetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class MACrystalSD;

class MADetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MADetectorConstruction();
  ~<ADetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void               ConstructSDandField();

  G4double GetWorldSizeZ() { return fvertexZ; }  // inline
  G4double GetWorldExtent() { return fmaxrad; }  // --"--

private:
  void DefineMaterials();

  G4VPhysicalVolume* SetupCryostat();

  G4double                            fvertexZ           = -1.0;
  G4double                            fmaxrad            = -1.0;
  G4Cache<MACrystalSD*>               fSD                = nullptr;
};

#endif
