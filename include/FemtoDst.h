//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar 30 20:07:20 2015 by ROOT version 5.34/25
// from TTree FemtoDst/A subset of the PicoDst
// found on file: femtoDst.root
//////////////////////////////////////////////////////////

#ifndef FEMTO_DST_h
#define FEMTO_DST_h

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "PicoDataStore.h"
#include "TMath.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

const int kMaxFDSTTracks = 1000;
class FemtoDst : public PicoDataStore
{
public:
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain



   // Declaration of leaf types
   UInt_t          Event_mTriggerWord;
   virtual UInt_t triggerWord(){ return Event_mTriggerWord; }
   Int_t           Event_mRunId;
   virtual Int_t runId(){ return Event_mRunId; }
   Float_t         Event_mPrimaryVertex_mX1;
   Float_t         Event_mPrimaryVertex_mX2;
   Float_t         Event_mPrimaryVertex_mX3;
   virtual Double_t vX(  ){ return Event_mPrimaryVertex_mX1; }
   virtual Double_t vY(  ){ return Event_mPrimaryVertex_mX2; }
   virtual Double_t vZ(  ){ return Event_mPrimaryVertex_mX3; }
   virtual Double_t vR(  ){ return TMath::Sqrt( vX()*vX() + vY()*vY() ); }

   Int_t           Event_mRefMult;
   virtual UShort_t refMult(){ return Event_mRefMult; }
   Int_t           Event_mNumberOfPrimaryTracks;
   virtual Int_t numTracks(){ return Event_mNumberOfPrimaryTracks; }
   Int_t           Event_mNBTOFMatchedTracks;
   virtual Int_t numTofMatchedTracks(){ return Event_mNBTOFMatchedTracks; }
   Float_t         Tracks_mEta[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackEta( Int_t iHit ){ return Tracks_mEta[ iHit ]; }
   Float_t         Tracks_mPt[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackPt( Int_t iHit ){ return Tracks_mPt[ iHit ]; }
   Float_t         Tracks_mDedx[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackDedx( Int_t iHit ){ return Tracks_mDedx[ iHit ]; }
   Float_t         Tracks_mBTofBeta[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackBeta( Int_t iHit ){ return (Doublt_t)Tracks_mBTofBeta[ iHit ]; }
   Float_t         Tracks_mBTofYLocal[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   Float_t         Tracks_mBTofZLocal[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackYLocal( Int_t iHit ){ return Tracks_mBTofYLocal[ iHit ]; }
   virtual Double_t trackZLocal( Int_t iHit ){ return Tracks_mBTofZLocal[ iHit ]; }
   Float_t         Tracks_mGDca[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackDca( Int_t iHit ){ return Tracks_mGDca[ iHit ]; }
   Char_t          Tracks_mCharge[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Int_t trackCharge( Int_t iHit ){ return Tracks_mCharge[ iHit ]; }
   Int_t           Tracks_mNHitsFit[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   Int_t           Tracks_mNHitsMax[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   Int_t           Tracks_mNHitsDedx[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Int_t trackNHitsFit( Int_t iHit ){ return Tracks_mNHitsFit[ iHit ]; }
   virtual Int_t trackNHitsPossible( Int_t iHit ){ return Tracks_mNHitsMax[ iHit ]; }
   virtual Int_t trackNHitsDedx( Int_t iHit ){ return Tracks_mNHitsDedx[ iHit ]; }
   Float_t         Tracks_mPMomentum_mX1[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   Float_t         Tracks_mPMomentum_mX2[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   Float_t         Tracks_mPMomentum_mX3[kMaxFDSTTracks];   //[Event.mNumberOfPrimaryTracks]
   virtual Double_t trackPx( Int_t iHit ){ return Tracks_mPMomentum_mX1[ iHit ]; }
   virtual Double_t trackPy( Int_t iHit ){ return Tracks_mPMomentum_mX2[ iHit ]; }
   virtual Double_t trackPz( Int_t iHit ){ return Tracks_mPMomentum_mX3[ iHit ]; }
   virtual Double_t trackP( Int_t iHit ){ 
      double pX = trackPx( iHit );
      double pY = trackPy( iHit );
      double pZ = trackPz( iHit );
      return TMath::Sqrt( pX*pX + pY*pY + pZ*pZ );
    }
   // List of branches
   TBranch        *b_Event_mTriggerWord;   //!
   TBranch        *b_Event_mRunId;   //!
   TBranch        *b_Event_mPrimaryVertex_mX1;   //!
   TBranch        *b_Event_mPrimaryVertex_mX2;   //!
   TBranch        *b_Event_mPrimaryVertex_mX3;   //!
   TBranch        *b_Event_mRefMult;   //!
   TBranch        *b_Event_mNumberOfPrimaryTracks;   //!
   TBranch        *b_Event_mNBTOFMatchedTracks;   //!
   TBranch        *b_Tracks_mEta;   //!
   TBranch        *b_Tracks_mPt;   //!
   TBranch        *b_Tracks_mDedx;   //!
   TBranch        *b_Tracks_mBTofBeta;   //!
   TBranch        *b_Tracks_mBTofYLocal;   //!
   TBranch        *b_Tracks_mBTofZLocal;   //!
   TBranch        *b_Tracks_mGDca;   //!
   TBranch        *b_Tracks_mCharge;   //!
   TBranch        *b_Tracks_mNHitsFit;   //!
   TBranch        *b_Tracks_mNHitsMax;   //!
   TBranch        *b_Tracks_mNHitsDedx;   //!
   TBranch        *b_Tracks_mPMomentum_mX1;   //!
   TBranch        *b_Tracks_mPMomentum_mX2;   //!
   TBranch        *b_Tracks_mPMomentum_mX3;   //!

   FemtoDst(TTree *tree=0);
   virtual ~FemtoDst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(){};
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual TTree * getTree() { return fChain; }
};

#endif


