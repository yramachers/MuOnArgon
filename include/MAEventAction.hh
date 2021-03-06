#ifndef MAEventAction_h
#define MAEventAction_h 1

#include <algorithm>
#include <numeric>
#include <vector>

#include "MALiquidHit.hh"

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

private:
  // methods
  MALiquidHitsCollection*    GetHitsCollection(G4int hcID,
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
  G4int                     fHID    = -1;
  std::map<G4String, G4int> lookup;
};

#endif
