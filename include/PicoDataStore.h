#ifndef PICO_DATA_STORE_H
#define PICO_DATA_STORE_H

#include "TTree.h"

class PicoDataStore
{
public:
	
	virtual TTree * getTree() = 0;
	virtual Int_t    GetEntry(Long64_t entry) = 0;


	virtual Int_t numTofTracks() = 0;
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

	/**
	 * Analysis Cuts
	 */

};


#endif