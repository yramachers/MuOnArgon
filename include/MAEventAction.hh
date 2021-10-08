#ifndef MAEventAction_h
#define MAEventAction_h 1

#include <algorithm>
#include <numeric>
#include <vector>

#include "MACrystalHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class MAEventAction : public G4UserEventAction
{
public:
  MAEventAction()          = default;
  virtual ~MAEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  // to create columns for Ntuple
  std::vector<G4int>&    GetHitTID()  { return htrid; }
  std::vector<G4int>&    GetHitIonZ() { return hZ; }
  std::vector<G4int>&    GetHitIonA() { return hA; }
  std::vector<G4double>& GetHitEdep() { return edep; }
  std::vector<G4double>& GetHitTime() { return thit; }
  std::vector<G4double>& GetHitxLoc() { return xloc; }
  std::vector<G4double>& GetHityLoc() { return yloc; }
  std::vector<G4double>& GetHitzLoc() { return zloc; }

  // tajectory methods
  std::vector<G4int>&    GetTrjPDG() { return trjpdg; }
  std::vector<G4int>&    GetTrjEntries() { return trjnpts; }
  std::vector<G4int>&    GetNameID() { return nameid; }
  std::vector<G4double>& GetTrjXVtx() { return trjxvtx; }
  std::vector<G4double>& GetTrjYVtx() { return trjyvtx; }
  std::vector<G4double>& GetTrjZVtx() { return trjzvtx; }
  std::vector<G4double>& GetTrjXPos() { return trjxpos; }
  std::vector<G4double>& GetTrjYPos() { return trjypos; }
  std::vector<G4double>& GetTrjZPos() { return trjzpos; }

private:
  // methods
  MACrystalHitsCollection*   GetHitsCollection(G4int hcID,
                                               const G4Event* event) const;
  G4int                      GeomID(G4String name);
  void                       makeMap();

  //! Brief description
  /*!
   * This filter method retrieves the track history of a specific hit.
   * Starting with the hit id, the identical track id is found in
   * the trajectory container with vectors extracted separately,
   * here the track id and the corresponding track parent id. The parent
   * id then serves as the new id to look for, all the way back to the
   * primary particle. That way, only the relevant track history from
   * start to finish is stored.
   *
   * \param[in] item hit id to look for as starting point of history
   * \param[in] tid vector of all track id's stored in event
   * \param[in] pid corresponding vector of all parent id's for all tracks in event.
   */
  std::vector<int> FilterTrajectories(int item, const std::vector<G4int>& tid,
                                      const std::vector<G4int>& pid)
  {
    int              idx  = 0;
    int              pidx = 0;
    std::vector<int> result;
    auto             it = std::find(tid.begin(), tid.end(), item);

    while(it != tid.end())  // find all links in the chain
    {
      idx = (it - tid.begin());  // location of id
      result.push_back(idx);

      pidx = pid.at(idx);  // next to look for
      it   = std::find(tid.begin(), tid.end(), pidx);
    }

    return result;
  }

  // data members
  // hit data
  G4int                 fHID    = -1;

  std::vector<G4int>    htrid;
  std::vector<G4int>    hZ;
  std::vector<G4int>    hA;
  std::vector<G4double> edep;
  std::vector<G4double> thit;
  std::vector<G4double> xloc;
  std::vector<G4double> yloc;
  std::vector<G4double> zloc;

  // trajectory data
  std::vector<G4int>        trjpdg;
  std::vector<G4int>        trjnpts;
  std::vector<G4int>        nameid;
  std::vector<G4double>     trjxvtx;
  std::vector<G4double>     trjyvtx;
  std::vector<G4double>     trjzvtx;
  std::vector<G4double>     trjxpos;
  std::vector<G4double>     trjypos;
  std::vector<G4double>     trjzpos;
  std::map<G4String, G4int> lookup;
};

#endif
