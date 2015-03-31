#ifndef FEMTO_EVENT_H
#define FEMTO_EVENT_H

class FemtoEvent 
{
public:
	FemtoEvent(){}
	~FemtoEvent(){}

	UInt_t mTriggerWord;
	Int_t mRunId;
	Float_t mPrimaryVertexX1, 
			mPrimaryVertexX2, 
			mPrimaryVertexX3;
	Int_t mRefMult;
	Int_t mNumberOfPrimaryTracks;
	Int_t mNBTOFMatchedTracks;
};



#endif