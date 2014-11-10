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
	 * Track properties
	 */
	virtual Double_t trackNHits( Int_t iHit ){ return 0; }
	virtual Double_t trackNHitsDedx( Int_t iHit ){ return 0; }
	virtual Double_t trackNHitsFit( Int_t iHit ){ return 0; }
	virtual Double_t trackNHitsPossible( Int_t iHit ){ return 0; }

	virtual Double_t trackPt( Int_t iHit ){ return 0; }
	virtual Double_t trackP( Int_t iHit ){ return 0; }
	virtual Double_t trackEta( Int_t iHit ){ return -100; }
	virtual Int_t trackCharge( Int_t iHit ){ return 0; }

	virtual Double_t trackDedx( Int_t iHit ){ return 0; }
	virtual Double_t trackPathLength( Int_t iHit ){ return 0; }
	virtual Double_t trackTof( Int_t iHit ){ return 0; }
	virtual Double_t trackBeta( Int_t iHit ){ return 0; }
	virtual Int_t trackTofMatch( Int_t iHit ){ return 0; }


	virtual Double_t eventVertexX(  ){ return -999; }
	virtual Double_t eventVertexY(  ){ return -999; }
	virtual Double_t eventVertexZ(  ){ return -999; }

	virtual Double_t trackDcaX( Int_t iHit ){ return -999; }
	virtual Double_t trackDcaY( Int_t iHit ){ return -999; }
	virtual Double_t trackDcaZ( Int_t iHit ){ return -999; }
	virtual Double_t trackDca( Int_t iHit ){ return -999; }

		// TOF
	virtual Double_t trackYLocal( Int_t iHit ){ return -999; }

	/**
	 * Event properties
	 */	
	virtual UInt_t eventRunId(){ return 0; }
	virtual UInt_t eventEventId(){ return 0; }
	virtual UInt_t eventDay(){ return 0; }
	virtual UInt_t eventYear(){ return 0; }

	virtual Int_t eventNTZero(  ){ return 0; }
	virtual Short_t eventTofMult(){ return 0; }
	virtual UShort_t eventRefMult(){ return 0; }
	virtual Int_t eventNumTracks(){ return 0; }
	virtual Short_t eventNumPrimary(){ return 0; }
	virtual Short_t eventNumGlobal(){ return 0; }

	virtual Float_t eventZDC(){ return 0; }
	virtual Float_t eventBBC(){ return 0; }

};


#endif