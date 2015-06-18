//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Apr  2 15:14:39 2015 by ROOT version 5.34/25
// from TTree PicoDst/StPicoDst
// found on file: st_physics_adc_15067048_raw_4500005.picoDst.root
//////////////////////////////////////////////////////////

#ifndef ProdPicoDst_h
#define ProdPicoDst_h

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TMath.h"

#include "PicoDataStore.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.
const Int_t kMaxEvent = 1;
const Int_t kMaxTracks = 5000;
const Int_t kMaxTrigger = 1;
const Int_t kMaxBTOWHit = 1;
const Int_t kMaxBTofHit = 5000;
const Int_t kMaxV0Ks = 1;
const Int_t kMaxV0L = 1;
const Int_t kMaxV0Lbar = 1;

class ProdPicoDst : public PicoDataStore {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Event_;
   Int_t           Event_mRunId[kMaxEvent];   //[Event_]
   Int_t           Event_mEventId[kMaxEvent];   //[Event_]
   UShort_t        Event_mFillId[kMaxEvent];   //[Event_]
   Float_t         Event_mBField[kMaxEvent];   //[Event_]
   Float_t         Event_mPrimaryVertex_mX1[kMaxEvent];   //[Event_]
   Float_t         Event_mPrimaryVertex_mX2[kMaxEvent];   //[Event_]
   Float_t         Event_mPrimaryVertex_mX3[kMaxEvent];   //[Event_]
   Float_t         Event_mSecondVertex_mX1[kMaxEvent];   //[Event_]
   Float_t         Event_mSecondVertex_mX2[kMaxEvent];   //[Event_]
   Float_t         Event_mSecondVertex_mX3[kMaxEvent];   //[Event_]
   UShort_t        Event_mTriggerWord[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultFtpcEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultFtpcWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultNeg[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultPos[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMult2NegEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMult2PosEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMult2NegWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMult2PosWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultHalfNegEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultHalfPosEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultHalfNegWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mRefMultHalfPosWest[kMaxEvent];   //[Event_]
   UChar_t         Event_mNVpdHitsEast[kMaxEvent];   //[Event_]
   UChar_t         Event_mNVpdHitsWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mNT0[kMaxEvent];   //[Event_]
   Short_t         Event_mVzVpd[kMaxEvent];   //[Event_]
   UShort_t        Event_mZDCx[kMaxEvent];   //[Event_]
   UShort_t        Event_mBBCx[kMaxEvent];   //[Event_]
   Float_t         Event_mBackgroundRate[kMaxEvent];   //[Event_]
   Float_t         Event_mBbcBlueBackgroundRate[kMaxEvent];   //[Event_]
   Float_t         Event_mBbcYellowBackgroundRate[kMaxEvent];   //[Event_]
   Float_t         Event_mBbcEastRate[kMaxEvent];   //[Event_]
   Float_t         Event_mBbcWestRate[kMaxEvent];   //[Event_]
   Float_t         Event_mZdcEastRate[kMaxEvent];   //[Event_]
   Float_t         Event_mZdcWestRate[kMaxEvent];   //[Event_]
   UShort_t        Event_mVpd[kMaxEvent][64];   //[Event_]
   UShort_t        Event_mZdcSumAdcEast[kMaxEvent];   //[Event_]
   UShort_t        Event_mZdcSumAdcWest[kMaxEvent];   //[Event_]
   UShort_t        Event_mZdcSmdEastHorizontal[kMaxEvent][8];   //[Event_]
   UShort_t        Event_mZdcSmdEastVertical[kMaxEvent][8];   //[Event_]
   UShort_t        Event_mZdcSmdWestHorizontal[kMaxEvent][8];   //[Event_]
   UShort_t        Event_mZdcSmdWestVertical[kMaxEvent][8];   //[Event_]
   Float_t         Event_mSpaceCharge[kMaxEvent];   //[Event_]
   UShort_t        Event_mbTofTrayMultiplicity[kMaxEvent];   //[Event_]
   UShort_t        Event_mNumberOfGlobalTracks[kMaxEvent];   //[Event_]
   Float_t         Event_mRanking[kMaxEvent];   //[Event_]
   UShort_t        Event_mNBEMCMatch[kMaxEvent];   //[Event_]
   UShort_t        Event_mNBTOFMatch[kMaxEvent];   //[Event_]
   UShort_t        Event_mBbcAdcEast[kMaxEvent][24];   //[Event_]
   UShort_t        Event_mBbcAdcWest[kMaxEvent][24];   //[Event_]
   UChar_t         Event_mHT_Th[kMaxEvent][3];   //[Event_]
   UChar_t         Event_mJP_Th[kMaxEvent][3];   //[Event_]
   Int_t           Tracks_;
   UShort_t        Tracks_mId[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mChi2[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mChi2Prob[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mGMomentum_mX1[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mGMomentum_mX2[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mGMomentum_mX3[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mPMomentum_mX1[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mPMomentum_mX2[kMaxTracks];   //[Tracks_]
   Float_t         Tracks_mPMomentum_mX3[kMaxTracks];   //[Tracks_]
   UChar_t         Tracks_mFlowFlag[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mOriginX[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mOriginY[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mOriginZ[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mGDca[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mDedx[kMaxTracks];   //[Tracks_]
   Char_t          Tracks_mNHitsFit[kMaxTracks];   //[Tracks_]
   UChar_t         Tracks_mNHitsMax[kMaxTracks];   //[Tracks_]
   UChar_t         Tracks_mNHitsDedx[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mNSigmaPion[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mNSigmaKaon[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mNSigmaProton[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mNSigmaElectron[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofCellId[kMaxTracks];   //[Tracks_]
   UChar_t         Tracks_mBTofMatchFlag[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mBTof[kMaxTracks];   //[Tracks_]
   UShort_t        Tracks_mBTofBeta[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofYLocal[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofZLocal[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofHitPosX[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofHitPosY[kMaxTracks];   //[Tracks_]
   Short_t         Tracks_mBTofHitPosZ[kMaxTracks];   //[Tracks_]
   Int_t           Trigger_;
   UInt_t          Trigger_fUniqueID[kMaxTrigger];   //[Trigger_]
   UInt_t          Trigger_fBits[kMaxTrigger];   //[Trigger_]
   UChar_t         Trigger_mFlag[kMaxTrigger];   //[Trigger_]
   UShort_t        Trigger_mId[kMaxTrigger];   //[Trigger_]
   UShort_t        Trigger_mAdc[kMaxTrigger];   //[Trigger_]
   Int_t           BTOWHit_;
   UInt_t          BTOWHit_fUniqueID[kMaxBTOWHit];   //[BTOWHit_]
   UInt_t          BTOWHit_fBits[kMaxBTOWHit];   //[BTOWHit_]
   UShort_t        BTOWHit_mId[kMaxBTOWHit];   //[BTOWHit_]
   UShort_t        BTOWHit_mAdc[kMaxBTOWHit];   //[BTOWHit_]
   Short_t         BTOWHit_mE[kMaxBTOWHit];   //[BTOWHit_]
   Int_t           BTofHit_;
   UInt_t          BTofHit_fUniqueID[kMaxBTofHit];   //[BTofHit_]
   UInt_t          BTofHit_fBits[kMaxBTofHit];   //[BTofHit_]
   Short_t         BTofHit_mId[kMaxBTofHit];   //[BTofHit_]
   Int_t           V0Ks_;
   Short_t         V0Ks_mIndex2Track[kMaxV0Ks][2];   //[V0Ks_]
 //StThreeVector<float> V0Ks_mMomentum[2][kMaxV0Ks];
   Float_t         V0Ks_mV0Pos_mX1[kMaxV0Ks];   //[V0Ks_]
   Float_t         V0Ks_mV0Pos_mX2[kMaxV0Ks];   //[V0Ks_]
   Float_t         V0Ks_mV0Pos_mX3[kMaxV0Ks];   //[V0Ks_]
   UShort_t        V0Ks_mDcaDaughters[kMaxV0Ks];   //[V0Ks_]
   UShort_t        V0Ks_mDca2Vtx[kMaxV0Ks];   //[V0Ks_]
   Float_t         V0Ks_mM[kMaxV0Ks];   //[V0Ks_]
   Int_t           V0L_;
   Short_t         V0L_mIndex2Track[kMaxV0L][2];   //[V0L_]
 //StThreeVector<float> V0L_mMomentum[2][kMaxV0L];
   Float_t         V0L_mV0Pos_mX1[kMaxV0L];   //[V0L_]
   Float_t         V0L_mV0Pos_mX2[kMaxV0L];   //[V0L_]
   Float_t         V0L_mV0Pos_mX3[kMaxV0L];   //[V0L_]
   UShort_t        V0L_mDcaDaughters[kMaxV0L];   //[V0L_]
   UShort_t        V0L_mDca2Vtx[kMaxV0L];   //[V0L_]
   Float_t         V0L_mM[kMaxV0L];   //[V0L_]
   Int_t           V0Lbar_;
   Short_t         V0Lbar_mIndex2Track[kMaxV0Lbar][2];   //[V0Lbar_]
 //StThreeVector<float> V0Lbar_mMomentum[2][kMaxV0Lbar];
   Float_t         V0Lbar_mV0Pos_mX1[kMaxV0Lbar];   //[V0Lbar_]
   Float_t         V0Lbar_mV0Pos_mX2[kMaxV0Lbar];   //[V0Lbar_]
   Float_t         V0Lbar_mV0Pos_mX3[kMaxV0Lbar];   //[V0Lbar_]
   UShort_t        V0Lbar_mDcaDaughters[kMaxV0Lbar];   //[V0Lbar_]
   UShort_t        V0Lbar_mDca2Vtx[kMaxV0Lbar];   //[V0Lbar_]
   Float_t         V0Lbar_mM[kMaxV0Lbar];   //[V0Lbar_]

   // List of branches
   TBranch        *b_Event_;   //!
   TBranch        *b_Event_mRunId;   //!
   TBranch        *b_Event_mEventId;   //!
   TBranch        *b_Event_mFillId;   //!
   TBranch        *b_Event_mBField;   //!
   TBranch        *b_Event_mPrimaryVertex_mX1;   //!
   TBranch        *b_Event_mPrimaryVertex_mX2;   //!
   TBranch        *b_Event_mPrimaryVertex_mX3;   //!
   TBranch        *b_Event_mSecondVertex_mX1;   //!
   TBranch        *b_Event_mSecondVertex_mX2;   //!
   TBranch        *b_Event_mSecondVertex_mX3;   //!
   TBranch        *b_Event_mTriggerWord;   //!
   TBranch        *b_Event_mRefMultFtpcEast;   //!
   TBranch        *b_Event_mRefMultFtpcWest;   //!
   TBranch        *b_Event_mRefMultNeg;   //!
   TBranch        *b_Event_mRefMultPos;   //!
   TBranch        *b_Event_mRefMult2NegEast;   //!
   TBranch        *b_Event_mRefMult2PosEast;   //!
   TBranch        *b_Event_mRefMult2NegWest;   //!
   TBranch        *b_Event_mRefMult2PosWest;   //!
   TBranch        *b_Event_mRefMultHalfNegEast;   //!
   TBranch        *b_Event_mRefMultHalfPosEast;   //!
   TBranch        *b_Event_mRefMultHalfNegWest;   //!
   TBranch        *b_Event_mRefMultHalfPosWest;   //!
   TBranch        *b_Event_mNVpdHitsEast;   //!
   TBranch        *b_Event_mNVpdHitsWest;   //!
   TBranch        *b_Event_mNT0;   //!
   TBranch        *b_Event_mVzVpd;   //!
   TBranch        *b_Event_mZDCx;   //!
   TBranch        *b_Event_mBBCx;   //!
   TBranch        *b_Event_mBackgroundRate;   //!
   TBranch        *b_Event_mBbcBlueBackgroundRate;   //!
   TBranch        *b_Event_mBbcYellowBackgroundRate;   //!
   TBranch        *b_Event_mBbcEastRate;   //!
   TBranch        *b_Event_mBbcWestRate;   //!
   TBranch        *b_Event_mZdcEastRate;   //!
   TBranch        *b_Event_mZdcWestRate;   //!
   TBranch        *b_Event_mVpd;   //!
   TBranch        *b_Event_mZdcSumAdcEast;   //!
   TBranch        *b_Event_mZdcSumAdcWest;   //!
   TBranch        *b_Event_mZdcSmdEastHorizontal;   //!
   TBranch        *b_Event_mZdcSmdEastVertical;   //!
   TBranch        *b_Event_mZdcSmdWestHorizontal;   //!
   TBranch        *b_Event_mZdcSmdWestVertical;   //!
   TBranch        *b_Event_mSpaceCharge;   //!
   TBranch        *b_Event_mbTofTrayMultiplicity;   //!
   TBranch        *b_Event_mNumberOfGlobalTracks;   //!
   TBranch        *b_Event_mRanking;   //!
   TBranch        *b_Event_mNBEMCMatch;   //!
   TBranch        *b_Event_mNBTOFMatch;   //!
   TBranch        *b_Event_mBbcAdcEast;   //!
   TBranch        *b_Event_mBbcAdcWest;   //!
   TBranch        *b_Event_mHT_Th;   //!
   TBranch        *b_Event_mJP_Th;   //!
   TBranch        *b_Tracks_;   //!
   TBranch        *b_Tracks_mId;   //!
   TBranch        *b_Tracks_mChi2;   //!
   TBranch        *b_Tracks_mChi2Prob;   //!
   TBranch        *b_Tracks_mGMomentum_mX1;   //!
   TBranch        *b_Tracks_mGMomentum_mX2;   //!
   TBranch        *b_Tracks_mGMomentum_mX3;   //!
   TBranch        *b_Tracks_mPMomentum_mX1;   //!
   TBranch        *b_Tracks_mPMomentum_mX2;   //!
   TBranch        *b_Tracks_mPMomentum_mX3;   //!
   TBranch        *b_Tracks_mFlowFlag;   //!
   TBranch        *b_Tracks_mOriginX;   //!
   TBranch        *b_Tracks_mOriginY;   //!
   TBranch        *b_Tracks_mOriginZ;   //!
   TBranch        *b_Tracks_mGDca;   //!
   TBranch        *b_Tracks_mDedx;   //!
   TBranch        *b_Tracks_mNHitsFit;   //!
   TBranch        *b_Tracks_mNHitsMax;   //!
   TBranch        *b_Tracks_mNHitsDedx;   //!
   TBranch        *b_Tracks_mNSigmaPion;   //!
   TBranch        *b_Tracks_mNSigmaKaon;   //!
   TBranch        *b_Tracks_mNSigmaProton;   //!
   TBranch        *b_Tracks_mNSigmaElectron;   //!
   TBranch        *b_Tracks_mBTofCellId;   //!
   TBranch        *b_Tracks_mBTofMatchFlag;   //!
   TBranch        *b_Tracks_mBTof;   //!
   TBranch        *b_Tracks_mBTofBeta;   //!
   TBranch        *b_Tracks_mBTofYLocal;   //!
   TBranch        *b_Tracks_mBTofZLocal;   //!
   TBranch        *b_Tracks_mBTofHitPosX;   //!
   TBranch        *b_Tracks_mBTofHitPosY;   //!
   TBranch        *b_Tracks_mBTofHitPosZ;   //!
   TBranch        *b_Trigger_;   //!
   TBranch        *b_Trigger_fUniqueID;   //!
   TBranch        *b_Trigger_fBits;   //!
   TBranch        *b_Trigger_mFlag;   //!
   TBranch        *b_Trigger_mId;   //!
   TBranch        *b_Trigger_mAdc;   //!
   TBranch        *b_BTOWHit_;   //!
   TBranch        *b_BTOWHit_fUniqueID;   //!
   TBranch        *b_BTOWHit_fBits;   //!
   TBranch        *b_BTOWHit_mId;   //!
   TBranch        *b_BTOWHit_mAdc;   //!
   TBranch        *b_BTOWHit_mE;   //!
   TBranch        *b_BTofHit_;   //!
   TBranch        *b_BTofHit_fUniqueID;   //!
   TBranch        *b_BTofHit_fBits;   //!
   TBranch        *b_BTofHit_mId;   //!
   TBranch        *b_V0Ks_;   //!
   TBranch        *b_V0Ks_mIndex2Track;   //!
   TBranch        *b_V0Ks_mV0Pos_mX1;   //!
   TBranch        *b_V0Ks_mV0Pos_mX2;   //!
   TBranch        *b_V0Ks_mV0Pos_mX3;   //!
   TBranch        *b_V0Ks_mDcaDaughters;   //!
   TBranch        *b_V0Ks_mDca2Vtx;   //!
   TBranch        *b_V0Ks_mM;   //!
   TBranch        *b_V0L_;   //!
   TBranch        *b_V0L_mIndex2Track;   //!
   TBranch        *b_V0L_mV0Pos_mX1;   //!
   TBranch        *b_V0L_mV0Pos_mX2;   //!
   TBranch        *b_V0L_mV0Pos_mX3;   //!
   TBranch        *b_V0L_mDcaDaughters;   //!
   TBranch        *b_V0L_mDca2Vtx;   //!
   TBranch        *b_V0L_mM;   //!
   TBranch        *b_V0Lbar_;   //!
   TBranch        *b_V0Lbar_mIndex2Track;   //!
   TBranch        *b_V0Lbar_mV0Pos_mX1;   //!
   TBranch        *b_V0Lbar_mV0Pos_mX2;   //!
   TBranch        *b_V0Lbar_mV0Pos_mX3;   //!
   TBranch        *b_V0Lbar_mDcaDaughters;   //!
   TBranch        *b_V0Lbar_mDca2Vtx;   //!
   TBranch        *b_V0Lbar_mM;   //!

   ProdPicoDst(TTree *tree=0);
   virtual ~ProdPicoDst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(){};
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   virtual TTree * getTree() { return fChain; }

   inline virtual UInt_t triggerWord(){ return Event_mTriggerWord[0]; }
   inline virtual Int_t runId(){ return Event_mRunId[0]; }

   inline virtual Double_t vX(  ){ return Event_mPrimaryVertex_mX1[0]; }
   inline virtual Double_t vY(  ){ return Event_mPrimaryVertex_mX2[0]; }
   inline virtual Double_t vZ(  ){ return Event_mPrimaryVertex_mX3[0]; }
   inline virtual Double_t vR(  ){ return TMath::Sqrt( vX()*vX() + vY()*vY() ); }

   inline virtual Float_t refMult(){ return (Event_mRefMultPos[0] + Event_mRefMultNeg[0]); }
   inline virtual Int_t numTracks(){ return Event_mNumberOfGlobalTracks[0]; }
   inline virtual Int_t trackTofMatch( Int_t iHit ){ 
      return Tracks_mBTofMatchFlag[ iHit ]; 
   }
   inline virtual Int_t numTofMatchedTracks(){ 
      
      Int_t nMatched = 0;
      Int_t nT = numTracks();
      
      for ( int iTrack = 0; iTrack < nT; iTrack++ ){
         // only look at primary tracks
         if ( trackPt( iTrack) == 0 )
            continue;
         
         if ( Tracks_mBTofMatchFlag[iTrack] > 0 )
            nMatched++;
      }
      return nMatched;
   }
   

   inline virtual Double_t trackPt( Int_t iHit ){ 
      double px = Tracks_mPMomentum_mX1[iHit];
      double py = Tracks_mPMomentum_mX2[iHit];
      return TMath::Sqrt( px*px + py*py ); 
   }
   inline virtual Double_t trackPx( Int_t iHit ){
      return Tracks_mPMomentum_mX1[iHit]; 
   }
   inline virtual Double_t trackPy( Int_t iHit ){
      return Tracks_mPMomentum_mX2[iHit]; 
   }
   inline virtual Double_t trackPz( Int_t iHit ){
      return Tracks_mPMomentum_mX3[iHit]; 
   }
   inline virtual Double_t trackP( Int_t iHit ){ 
      double px = Tracks_mPMomentum_mX1[iHit]; 
      double py = Tracks_mPMomentum_mX2[iHit]; 
      double pz = Tracks_mPMomentum_mX3[iHit]; 
      return TMath::Sqrt( px*px + py*py + pz*pz ); 
   }
   inline virtual Double_t globalPt( Int_t iHit ){ 
      double px = Tracks_mGMomentum_mX1[iHit];
      double py = Tracks_mGMomentum_mX2[iHit];
      return TMath::Sqrt( px*px + py*py ); 
   }
   inline virtual Int_t trackCharge( Int_t iHit ){ 

      Int_t nhitsfit = Tracks_mNHitsFit[iHit];
      if ( nhitsfit > 0 )
         return 1;
      else if ( nhitsfit < 0 )
         return -1;
      
      return 0; 
   }
   inline virtual Double_t trackEta( Int_t iHit ){ 
      double pz = Tracks_mPMomentum_mX3[iHit]; 
      return TMath::ASinH( pz / trackPt( iHit ) ); 
   }

   // same as nHitsFit for primary track
   inline virtual Int_t trackNHits( Int_t iHit ){ return TMath::Abs( (Char_t)(Tracks_mNHitsFit[iHit]) ); }
   inline virtual Int_t trackNHitsDedx( Int_t iHit ){ return (UChar_t)(Tracks_mNHitsDedx[iHit]); }
   inline virtual Int_t trackNHitsFit( Int_t iHit ){ return TMath::Abs( (Char_t)(Tracks_mNHitsFit[iHit]) ); }
   inline virtual Int_t trackNHitsPossible( Int_t iHit ){ return (UChar_t)(Tracks_mNHitsMax[iHit]); }

   inline virtual Double_t trackDedx( Int_t iHit ){ return ((Float_t)Tracks_mDedx[iHit]) / 1000.0; }
   
   inline virtual Double_t trackBeta( Int_t iHit ){ return ((Tracks_mBTofBeta[iHit]) / 20000.0); }
   inline virtual Double_t trackYLocal( Int_t iHit ){ return ((Tracks_mBTofYLocal[iHit]) / 1000.0); }
   inline virtual Double_t trackZLocal( Int_t iHit ){ return ((Tracks_mBTofZLocal[iHit]) / 1000.0); }

   inline virtual Double_t trackDca( Int_t iHit ){ return (Tracks_mGDca[iHit] / 1000.0); }








};

#endif
