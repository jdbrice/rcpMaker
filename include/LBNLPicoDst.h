#ifndef LBNL_PICO_DST_H
#define LBNL_PICO_DST_H

#include "PicoDataStore.h"
#include "DataSource.h"
#include "TMath.h"

class LBNLPicoDst : public PicoDataStore {

	DataSource * ds;

public:
	virtual TTree * getTree() { return NULL; };
	virtual Int_t    GetEntry(Long64_t entry) { return (Int_t)ds->getEntry( entry ) ;};

	LBNLPicoDst( DataSource *_ds ){
		ds = _ds;
	}
	virtual UInt_t triggerWord(){ return ds->getUInt( "Event.mTriggerWord" ); }
	virtual UInt_t runId(){ return ds->getUInt( "Event.mRunId" ); }
	virtual UInt_t eventId(){ return ds->getUInt( "Event.mEventId" );; }

	virtual Double_t vX(  ){ return ds->get("Event.mPrimaryVertex.mX1"); }
	virtual Double_t vY(  ){ return ds->get("Event.mPrimaryVertex.mX2"); }
	virtual Double_t vZ(  ){ return ds->get("Event.mPrimaryVertex.mX3"); }
	virtual Double_t vR(  ){ return TMath::Sqrt( vX()*vX() + vY()*vY() ); }

	virtual UShort_t refMult(){ return (ds->getInt("Event.mRefMultPos") + ds->getInt("Event.mRefMultNeg")); }
	virtual Int_t numTracks(){ return ds->getInt("Event.mNumberOfGlobalTracks"); }
	virtual Int_t numTofMatchedTracks(){ return ds->getInt("Event.mNBTOFMatch"); }
	

	virtual Double_t trackPt( Int_t iHit ){ 
		double px = ds->get("Tracks.mPMomentum.mX1", iHit);
		double py = ds->get("Tracks.mPMomentum.mX2", iHit);
		return TMath::Sqrt( px*px + py*py ); 
	}
	virtual Double_t trackP( Int_t iHit ){ 
		double px = ds->get("Tracks.mPMomentum.mX1", iHit);
		double py = ds->get("Tracks.mPMomentum.mX2", iHit);
		double pz = ds->get("Tracks.mPMomentum.mX3", iHit);
		return TMath::Sqrt( px*px + py*py + pz*pz ); 
	}
	virtual Double_t globalPt( Int_t iHit ){ 
		double px = ds->get("Tracks.mGMomentum.mX1", iHit);
		double py = ds->get("Tracks.mGMomentum.mX2", iHit);
		return TMath::Sqrt( px*px + py*py ); 
	}
	virtual Int_t trackCharge( Int_t iHit ){ 
		if ( ds->getInt("Tracks.mNHitsFit", iHit) > 0 )
			return 1;
		else if ( ds->getInt("Tracks.mNHitsFit", iHit) < 0 )
			return -1;
		else
			return 0; 
	}
	virtual Double_t trackEta( Int_t iHit ){ 
		double pz = ds->get("Tracks.mPMomentum.mX3", iHit);
		return TMath::ASinH( pz / trackPt( iHit ) ); 
	}

	// same as nHitsFit for primary track
	virtual Double_t trackNHits( Int_t iHit ){ return TMath::Abs( ds->get("Tracks.mNHitsFit", iHit) ); }
	virtual Double_t trackNHitsDedx( Int_t iHit ){ return ds->get("Tracks.mNHitsDedx", iHit); }
	virtual Double_t trackNHitsFit( Int_t iHit ){ return TMath::Abs( ds->get("Tracks.mNHitsFit", iHit) ); }
	virtual Double_t trackNHitsPossible( Int_t iHit ){ return ds->get("Tracks.mNHitsMax", iHit); }

	virtual Double_t trackDedx( Int_t iHit ){ return ds->get("Tracks.mDedx", iHit) / 1000.0; }
	
	virtual Double_t trackBeta( Int_t iHit ){ return (ds->get("Tracks.mBTofBeta", iHit) / 20000.0); }
	virtual Int_t trackTofMatch( Int_t iHit ){ return ds->get("Tracks.mBTofMatchFlag", iHit); }
	virtual Double_t trackYLocal( Int_t iHit ){ return (ds->get("Tracks.mBTofYLocal", iHit) / 1000.0); }
	virtual Double_t trackZLocal( Int_t iHit ){ return (ds->get("Tracks.mBTofZLocal", iHit) / 1000.0); }

	virtual Double_t trackDca( Int_t iHit ){ return ds->get("Tracks.mGDca", iHit) / 1000.0; }

};



#endif