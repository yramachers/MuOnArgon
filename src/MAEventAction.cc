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
  std::vector<double> ttime, ted, tx, ty, tzloc;
  std::vector<G4String> tname;

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
    tname.push_back(hh->GetVName());
    ttime.push_back(hh->GetTime()   / G4Analysis::GetUnitValue("ns"));
    ted.push_back(hh->GetEdep()     / G4Analysis::GetUnitValue("MeV"));
    tx.push_back((hh->GetPos()).x() / G4Analysis::GetUnitValue("m"));
    ty.push_back((hh->GetPos()).y() / G4Analysis::GetUnitValue("m"));
    tzloc.push_back((hh->GetPos()).z() / G4Analysis::GetUnitValue("m"));
  }

  // fill the ntuple
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<ted.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(0, 1, thid.at(i));
    analysisManager->FillNtupleIColumn(0, 2, tz.at(i));
    analysisManager->FillNtupleIColumn(0, 3, ta.at(i));
    analysisManager->FillNtupleIColumn(0, 4, GeomID(tname.at(i)));
    analysisManager->FillNtupleDColumn(0, 5, ted.at(i));
    analysisManager->FillNtupleDColumn(0, 6, ttime.at(i));
    analysisManager->FillNtupleDColumn(0, 7, tx.at(i));
    analysisManager->FillNtupleDColumn(0, 8, ty.at(i));
    analysisManager->FillNtupleDColumn(0, 9, tzloc.at(i)); // same size
    analysisManager->AddNtupleRow(0);
  }

  // fill trajectory data

  // fill trajectory data if available
  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int                  n_trajectories =
    (trajectoryContainer == nullptr) ? 0 : trajectoryContainer->entries();

  std::vector<int> res;
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
      res = FilterTrajectories(item, temptid, temppid);
      for(int& idx : res)
      {
	analysisManager->FillNtupleIColumn(1, 0, eventID); // repeat all rows
	analysisManager->FillNtupleIColumn(1, 1, temppdg.at(idx));
	analysisManager->FillNtupleIColumn(1, 2, GeomID(tempname.at(idx)));
	analysisManager->FillNtupleDColumn(1, 3, tempxvtx.at(idx));
	analysisManager->FillNtupleDColumn(1, 4, tempyvtx.at(idx));
	analysisManager->FillNtupleDColumn(1, 5, tempzvtx.at(idx));
        analysisManager->AddNtupleRow(1);
      }
    }
  }

  // printing
  G4cout << ">>> Event: " << eventID << G4endl;
  G4cout << "    " << nofHits << " hits stored in this event." << G4endl;
  G4cout << "    " << res.size() << " trajectories after filter." << G4endl;
  res.clear();
}
