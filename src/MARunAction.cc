#include "MARunAction.hh"
#include "g4root.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

MARunAction::MARunAction(G4String name)
: G4UserRunAction()
, fout(std::move(name))
{
  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);

  // Creating ntuple with value entries
  // since vector entries don't work anymore with 10.7
  //
  analysisManager->CreateNtuple("Score", "Hits");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("HitID");
  analysisManager->CreateNtupleIColumn("IonZ");
  analysisManager->CreateNtupleIColumn("IonA");
  analysisManager->CreateNtupleIColumn("VCode");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("Time");
  analysisManager->CreateNtupleDColumn("Hitxloc");
  analysisManager->CreateNtupleDColumn("Hityloc");
  analysisManager->CreateNtupleDColumn("Hitzloc");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("Traj", "Trajectories");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("HitID");
  analysisManager->CreateNtupleIColumn("ParentID");
  analysisManager->CreateNtupleIColumn("Trjpdg");
  analysisManager->CreateNtupleIColumn("VtxName");
  analysisManager->CreateNtupleDColumn("TrjXVtx");
  analysisManager->CreateNtupleDColumn("TrjYVtx");
  analysisManager->CreateNtupleDColumn("TrjZVtx");
  analysisManager->FinishNtuple();
}

MARunAction::~MARunAction() { delete G4AnalysisManager::Instance(); }

void MARunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  analysisManager->OpenFile(fout);
}

void MARunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}
