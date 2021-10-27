#include "MACrystalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MACrystalSD::MACrystalSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MACrystalSD::~MACrystalSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MACrystalSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new MACrystalHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MACrystalSD::ProcessHits(G4Step* aStep, 
                                     G4TouchableHistory*)
{  
  // energy deposit required
  G4double edep = aStep->GetTotalEnergyDeposit();

  if (edep==0.) return false;

  // only Ion production of interest
  if (!aStep->GetTrack()->GetDefinition()->IsGeneralIon()) return false;

  // particle info on Ions
  auto iZ = aStep->GetTrack()->GetDefinition()->GetAtomicNumber();
  auto iA = aStep->GetTrack()->GetDefinition()->GetAtomicMass();

  MACrystalHit* newHit = new MACrystalHit();

  newHit->SetTID(aStep->GetTrack()->GetTrackID());
  newHit->SetIonZ(iZ);
  newHit->SetIonA(iA);
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
  newHit->SetEdep(edep);
  newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());

  fHitsCollection->insert( newHit );

  return true;
}

void MACrystalSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the crystal: " << G4endl;
  }
}
