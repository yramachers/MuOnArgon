#include "MALiquidSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MALiquidSD::MALiquidSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MALiquidSD::~MALiquidSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MALiquidSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new MALiquidHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MALiquidSD::ProcessHits(G4Step* aStep, 
                                     G4TouchableHistory*)
{  
  // energy deposit required
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep==0.) return false;

  // only Ion production of interest
  G4bool isGenIon = aStep->GetTrack()->GetDefinition()->IsGeneralIon();
  G4bool isTriton = aStep->GetTrack()->GetDefinition()->GetParticleName() == "triton";
  if (isGenIon || isTriton) {

     // particle info on Ions
     auto iZ = aStep->GetTrack()->GetDefinition()->GetAtomicNumber();
     auto iA = aStep->GetTrack()->GetDefinition()->GetAtomicMass();

     MALiquidHit* newHit = new MALiquidHit();

     newHit->SetTID(aStep->GetTrack()->GetTrackID());
     newHit->SetIonZ(iZ);
     newHit->SetIonA(iA);
     newHit->SetVName(aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName());
     newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
     newHit->SetEdep(edep);
     newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());

     fHitsCollection->insert( newHit );
     return true;
  }
  return false;
}

void MALiquidSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the liquid: " << G4endl;
  }
}
