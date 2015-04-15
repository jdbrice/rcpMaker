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
	virtual Int_t trackNHits( Int_t iHit ){ return 0; }
	virtual Int_t trackNHitsDedx( Int_t iHit ){ return 0; }
	virtual Int_t trackNHitsFit( Int_t iHit ){ return 0; }
	virtual Int_t trackNHitsPossible( Int_t iHit ){ return 0; }

	virtual Double_t globalPt( Int_t iHit ){ return 0; }
	virtual Double_t trackPt( Int_t iHit ){ return 0; }
	virtual Double_t trackP( Int_t iHit ){ return 0; }
	virtual Double_t trackPx( Int_t iHit ){ return 0; }
	virtual Double_t trackPy( Int_t iHit ){ return 0; }
	virtual Double_t trackPz( Int_t iHit ){ return 0; }
	virtual Double_t trackEta( Int_t iHit ){ return -100; }
	virtual Int_t trackCharge( Int_t iHit ){ return 0; }

	virtual Double_t trackDedx( Int_t iHit ){ return 0; }
	virtual Double_t trackPathLength( Int_t iHit ){ return 0; }
	virtual Double_t trackTof( Int_t iHit ){ return 0; }
	virtual Double_t trackBeta( Int_t iHit ){ return 0; }
	virtual Int_t trackTofMatch( Int_t iHit ){ return 0; }


	virtual Double_t vX(  ){ return -999; }
	virtual Double_t vY(  ){ return -999; }
	virtual Double_t vZ(  ){ return -999; }
	virtual Double_t vR(  ){ return -999; }

	virtual Double_t trackDcaX( Int_t iHit ){ return -999; }
	virtual Double_t trackDcaY( Int_t iHit ){ return -999; }
	virtual Double_t trackDcaZ( Int_t iHit ){ return -999; }
	virtual Double_t trackDca( Int_t iHit ){ return -999; }

		// TOF
	virtual Double_t trackYLocal( Int_t iHit ){ return -999; }
	virtual Double_t trackZLocal( Int_t iHit ){ return -999; }

	/**
	 * Event properties
	 */	
	virtual Int_t runId(){ return 0; }
	virtual Int_t eventId(){ return 0; }
	virtual Float_t eventWeight(){ return 0; }
	virtual UInt_t eventDay(){ return 0; }
	virtual UInt_t eventYear(){ return 0; }

	virtual UInt_t triggerWord(){ return 0; }

	virtual Int_t nTZero(  ){ return 0; }
	virtual Short_t tofMult(){ return 0; }
	virtual UShort_t refMult(){ return 0; }
	virtual Int_t numTracks(){ return 0; }
	virtual Short_t numPrimary(){ return 0; }
	virtual Short_t numGlobal(){ return 0; }
	virtual Int_t numTofMatchedTracks(){ return 0; }

	virtual Float_t zdc(){ return 0; }
	virtual Float_t bbc(){ return 0; }

};


#endif