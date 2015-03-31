#ifndef FEMTO_TRACK_H
#define FEMTO_TRACK_H


const int kMaxTracks = 1000;
class FemtoTrack
{
public:
	
	
	Float_t mPMomentumX1[kMaxTracks],
			mPMomentumX2[kMaxTracks],
			mPMomentumX3[kMaxTracks];
	Int_t 	mNHitsFit[kMaxTracks],
			mNHitsMax[kMaxTracks],
			mNHitsDedx[kMaxTracks];
	Float_t mEta[kMaxTracks];
	Float_t mPt[kMaxTracks];
	Float_t mDedx[kMaxTracks], mBTofBeta[kMaxTracks];
	Float_t mBTofYLocal[kMaxTracks], mBTofZLocal[kMaxTracks];
	Float_t mGDca[kMaxTracks];
	Char_t mCharge[kMaxTracks];
};


#endif