#include "MAEventAction.hh"
#include "MATrajectory.hh"
#include "g4root.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

#include "MALiquidSD.hh"

#include "Randomize.hh"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <vector>

MALiquidHitsCollection* MAEventAction::GetHitsCollection(G4int hcID,
                                              const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<MALiquidHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("MAEventAction::GetHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


void MAEventAction::makeMap()
{
  // make a lookup table to translate Logical Volume name
  // to a unique int for storage in the ntuple.
  lookup["Cavern_log"]   = 0;
  lookup["Hall_log"]     = 1;
  lookup["Tank_log"]     = 2;
  lookup["Pu_log"]       = 3;
  lookup["Membrane_log"] = 4;
  lookup["Lar_log"]      = 5;
  lookup["Cu_log"]       = 6;
  lookup["OB_log"]       = 7;
  lookup["Ac2_log"]      = 8;
  lookup["IB_log"]       = 9;
  lookup["Ac_log"]       = 10;
  lookup["TPC_log"]      = 11;
}

G4int MAEventAction::GeomID(G4String name)
{
  auto it = lookup.find(name);

  if(it == lookup.end())
  {
    G4ExceptionDescription msg;
    msg << "Name  " << name << " not in look up table";
    G4Exception("MAEventAction::GeomID()", "MyCode0005", FatalException, msg);
  }
  return it->second;
}

void MAEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{
  edep.clear();
  htrid.clear();
  thit.clear();
  hZ.clear();
  hA.clear();
  edep.clear();
  xloc.clear();
  yloc.clear();
  zloc.clear();
  // clear trajectory data
  trjpdg.clear();
  trjnpts.clear();
  nameid.clear();
  trjxvtx.clear();
  trjyvtx.clear();
  trjzvtx.clear();
  trjxpos.clear();
  trjypos.clear();
  trjzpos.clear();

  makeMap();
}

void MAEventAction::EndOfEventAction(const G4Event* event)
{
  // Get liquid hits collections IDs
  if(fHID < 0)
    fHID   = G4SDManager::GetSDMpointer()->GetCollectionID("LiquidHitsCollection");


  // Get entries from hits collections
  //
  auto CrysHC   = GetHitsCollection(fHID, event);

  if(CrysHC->entries() <= 0)
  {
    return;  // no action on no hit
  }

  // dummy storage
  std::vector<int> thid, tz, ta;
  std::vector<double> ttime, ted, tx, ty, tz;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int nofHits = CrysHC->entries();

  for ( G4int i=0; i<nofHits; i++ ) 
  {
    auto hh = (*CrysHC)[i];

    thid.push_back(hh->GetTID());
    tz.push_back(hh->GetIonZ());
    ta.push_back(hh->GetIonA());
    ttime.push_back(hh->GetTime()   / G4Analysis::GetUnitValue("ns"));
    ted.push_back(hh->GetEdep()     / G4Analysis::GetUnitValue("MeV"));
    tx.push_back((hh->GetPos()).x() / G4Analysis::GetUnitValue("m"));
    ty.push_back((hh->GetPos()).y() / G4Analysis::GetUnitValue("m"));
    tz.push_back((hh->GetPos()).z() / G4Analysis::GetUnitValue("m"));
  }

  // fill the ntuple
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<ted.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(1, thid.at(i));
    analysisManager->FillNtupleIColumn(2, tz.at(i));
    analysisManager->FillNtupleIColumn(2, ta.at(i));
    analysisManager->FillNtupleDColumn(4, ted.at(i));
    analysisManager->FillNtupleDColumn(5, ttime.at(i));
    analysisManager->FillNtupleDColumn(6, tx.at(i));
    analysisManager->FillNtupleDColumn(7, ty.at(i));
    analysisManager->FillNtupleDColumn(8, tz.at(i)); // same size
  }

  // fill trajectory data

  // fill trajectory data if available
  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int                  n_trajectories =
    (trajectoryContainer == nullptr) ? 0 : trajectoryContainer->entries();

  if(n_trajectories > 0)
  {
    // temporary full storage
    std::vector<G4int>    temptid, temppid, temppdg;
    std::vector<G4String> tempname;
    std::vector<G4double> tempxvtx, tempyvtx, tempzvtx;


    for(G4int i = 0; i < n_trajectories; i++)
    {
      MATrajectory* trj = (MATrajectory*) ((*(event->GetTrajectoryContainer()))[i]);
      temptid.push_back(trj->GetTrackID());
      temppid.push_back(trj->GetParentID());
      temppdg.push_back(trj->GetPDGEncoding());
      tempname.push_back(trj->GetVertexName());
      tempxvtx.push_back((trj->GetVertex()).x());
      tempyvtx.push_back((trj->GetVertex()).y());
      tempzvtx.push_back((trj->GetVertex()).z());
    }

    // store filtered trajectories only
    for(const int& item : thid)
    {
      std::vector<int> res = FilterTrajectories(item, temptid, temppid);
      for(int& idx : res)
      {
	analysisManager->FillNtupleIColumn(9, temppdg.at(idx));
	analysisManager->FillNtupleIColumn(10, GeomID(tempname.at(idx)));
	analysisManager->FillNtupleDColumn(11, tempxvtx.at(idx));
	analysisManager->FillNtupleDColumn(12, tempyvtx.at(idx));
	analysisManager->FillNtupleDColumn(13, tempzvtx.at(idx));
      }
    }
  }
  // fill the ntuple
  analysisManager->AddNtupleRow();

  // printing
  G4int eventID = event->GetEventID();
  G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << ted.size() << " hits stored in this event." << G4endl;
  G4cout << "    " << temptid.size() << " trajectories stored in this event." << G4endl;
}
