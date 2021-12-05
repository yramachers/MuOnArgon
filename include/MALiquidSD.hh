#ifndef MALiquidSD_h
#define MALiquidSD_h 1

#include "G4VSensitiveDetector.hh"

#include "MALiquidHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// MALiquid sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class MALiquidSD : public G4VSensitiveDetector
{
  public:
    MALiquidSD(const G4String& name, 
                const G4String& hitsCollectionName);
    virtual ~MALiquidSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    MALiquidHitsCollection* fHitsCollection;
};

#endif

