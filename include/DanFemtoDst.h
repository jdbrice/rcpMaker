#ifndef FEMTO_DST_H
#define FEMTO_DST_H

#include "PicoDataStore.h"
#include "DataSource.h"
#include "TMath.h"

class FemtoDst : public PicoDataStore {

	DataSource * ds;

public:
	virtual TTree * getTree() { return NULL; };
	virtual Int_t    GetEntry(Long64_t entry) { return (Int_t)ds->getEntry( entry ) ;};

	FemtoDst( DataSource *_ds ){
		ds = _ds;
	}
	virtual UInt_t triggerWord(){ 
		cout << "TRIGGERWORD" << endl;
		return ds->get<UInt_t>( "Event.mTriggerWord" ); }
	virtual Int_t runId(){ return ds->get<Int_t>( "Event.mRunId" ); }

	virtual Double_t vX(  ){ return ds->get<Float_t>("Event.mPrimaryVertex.mX1"); }
	virtual Double_t vY(  ){ return ds->get<Float_t>("Event.mPrimaryVertex.mX2"); }
	virtual Double_t vZ(  ){ return ds->get<Float_t>("Event.mPrimaryVertex.mX3"); }
	virtual Double_t vR(  ){ return TMath::Sqrt( vX()*vX() + vY()*vY() ); }

	virtual UShort_t refMult(){ return ds->get<Int_t>("Event.mRefMult"); }
	virtual Int_t numTracks(){ return ds->get<Int_t>("Event.mNumberOfPrimaryTracks"); }
	virtual Int_t numTofMatchedTracks(){ 
		return ds->get<Int_t>( "Event.mNBTOFMatchedTracks" );
	}
	

	virtual Double_t trackPt( Int_t iHit ){ 
		double px = ds->get<Float_t>("Tracks.mPMomentum.mX1", iHit);
		double py = ds->get<Float_t>("Tracks.mPMomentum.mX2", iHit);
		return TMath::Sqrt( px*px + py*py ); 
	}
	virtual Double_t trackP( Int_t iHit ){ 
		double px = ds->get<Float_t>("Tracks.mPMomentum.mX1", iHit);
		double py = ds->get<Float_t>("Tracks.mPMomentum.mX2", iHit);
		double pz = ds->get<Float_t>("Tracks.mPMomentum.mX3", iHit);
		return TMath::Sqrt( px*px + py*py + pz*pz ); 
	}
	virtual Int_t trackCharge( Int_t iHit ){ 
		return ds->get<Char_t>("Tracks.mCharge", iHit);
	}
	virtual Double_t trackEta( Int_t iHit ){ 
		return ds->get<Float_t>( "Tracks.mEta", iHit );
	}

	// same as nHitsFit for primary track
	virtual Int_t trackNHits( Int_t iHit ){ return TMath::Abs( ds->get<Int_t>("Tracks.mNHitsFit", iHit) ); }
	virtual Int_t trackNHitsDedx( Int_t iHit ){ return ds->get<Int_t>("Tracks.mNHitsDedx", iHit); }
	virtual Int_t trackNHitsFit( Int_t iHit ){ return TMath::Abs( ds->get<Int_t>("Tracks.mNHitsFit", iHit) ); }
	virtual Int_t trackNHitsPossible( Int_t iHit ){ return ds->get<Int_t>("Tracks.mNHitsMax", iHit); }

	virtual Double_t trackDedx( Int_t iHit ){ return ds->get<Float_t>("Tracks.mDedx", iHit); }
	
	virtual Double_t trackBeta( Int_t iHit ){ return ds->get<Float_t>("Tracks.mBTofBeta", iHit); }
	virtual Double_t trackYLocal( Int_t iHit ){ return ds->get<Float_t>("Tracks.mBTofYLocal", iHit); }
	virtual Double_t trackZLocal( Int_t iHit ){ return ds->get<Float_t>("Tracks.mBTofZLocal", iHit); }

	virtual Double_t trackDca( Int_t iHit ){ return ds->get<Float_t>("Tracks.mGDca", iHit); }

};



#endif