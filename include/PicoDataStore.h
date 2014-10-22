#ifndef PICO_DATA_STORE_H
#define PICO_DATA_STORE_H

#include "TTree.h"
#include <vector>

class PicoDataStore
{
public:
	
	virtual TTree * getTree() = 0;
	virtual Int_t    GetEntry(Long64_t entry) = 0;

	
	/**
	 * Triggers
	 */
	virtual Int_t numEventTriggers() = 0;
	virtual std::vector<UInt_t> eventTriggerIds() = 0;

	/**
	 * Kinematics
	 * @param  iHit The hit index in the current Event
	 * @return      the kinematic var for the current event and given hit
	 */
	virtual Double_t trackPt( Int_t iHit ) = 0;
	virtual Double_t trackP( Int_t iHit ) = 0;
	virtual Double_t trackEta( Int_t iHit ) = 0;
	virtual Int_t trackCharge( Int_t iHit ) = 0;

	/**
	 * Pid 
	 * @param  iHit The hit index in the current Event
	 * @return      Returns the given var
	 */
	virtual Double_t trackDedx( Int_t iHit ) = 0;
	virtual Double_t trackPathLength( Int_t iHit ) = 0;
	virtual Double_t trackTof( Int_t iHit ) = 0;
	virtual Double_t trackBeta( Int_t iHit ) = 0;
	virtual Int_t trackTofMatch( Int_t iHit ) = 0;

	/**
	 * Analysis Cuts
	 */
	virtual Double_t eventVertexX(  ) = 0;
	virtual Double_t eventVertexY(  ) = 0;
	virtual Double_t eventVertexZ(  ) = 0;

	

	virtual Double_t trackDcaX( Int_t iHit ) = 0;
	virtual Double_t trackDcaY( Int_t iHit ) = 0;
	virtual Double_t trackDcaZ( Int_t iHit ) = 0;
	virtual Double_t trackDca( Int_t iHit ) = 0;

	virtual Double_t trackYLocal( Int_t iHit ) = 0;

	virtual Double_t trackNHits( Int_t iHit ) = 0;
	virtual Double_t trackNHitsDedx( Int_t iHit ) = 0;
	virtual Double_t trackNHitsFit( Int_t iHit ) = 0;
	virtual Double_t trackNHitsPossible( Int_t iHit ) = 0;

	
	virtual UInt_t eventRunId() = 0;
	virtual UInt_t eventEventId() = 0;
	virtual UInt_t eventDay() = 0;
	virtual UInt_t eventYear() = 0;

	virtual Int_t eventNTZero(  ) = 0;
	virtual Short_t eventTofMult() = 0;
	virtual UShort_t eventRefMult() = 0;
	virtual Int_t eventNumTracks() = 0;
	virtual Short_t eventNumPrimary() = 0;
	virtual Short_t eventNumGlobal() = 0;

	virtual Float_t eventZDC() = 0;
	virtual Float_t eventBBC() = 0;

};


#endif