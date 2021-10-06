#ifndef MADetectorConstruction_h
#define MADetectorConstruction_h 1

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
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
  void     SetGeometry(const G4String& name);
  void     ExportGeometry(const G4String& file);
  void     SetNeutronBiasFactor(G4double nf);
  void     SetMuonBiasFactor(G4double mf);

private:
  void DefineCommands();
  void DefineMaterials();

  G4VPhysicalVolume* SetupBaseline();
  G4VPhysicalVolume* SetupAlternative();
  G4VPhysicalVolume* SetupHallA();

  G4GenericMessenger*                 fDetectorMessenger = nullptr;
  G4GenericMessenger*                 fBiasMessenger     = nullptr;
  G4double                            fvertexZ           = -1.0;
  G4double                            fmaxrad            = -1.0;
  G4String                            fGeometryName      = "baseline";
  G4double                            fNeutronBias       = 1.0;
  G4double                            fMuonBias          = 1.0;
  G4Cache<MACrystalSD*>               fSD                = nullptr;
};

#endif
