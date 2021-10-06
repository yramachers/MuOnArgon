#ifndef MACrystalSD_h
#define MACrystalSD_h 1

#include "G4VSensitiveDetector.hh"

#include "MACrystalHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// MACrystal sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class MACrystalSD : public G4VSensitiveDetector
{
  public:
    MACrystalSD(const G4String& name, 
                const G4String& hitsCollectionName);
    virtual ~MACrystalSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    MACrystalHitsCollection* fHitsCollection;
};

#endif

