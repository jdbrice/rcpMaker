#include "ProdPicoDst.h"


ProdPicoDst::ProdPicoDst(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("st_physics_adc_15067048_raw_4500005.picoDst.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("st_physics_adc_15067048_raw_4500005.picoDst.root");
      }
      f->GetObject("PicoDst",tree);

   }

   Init(tree);

   fChain->SetBranchStatus("*", 0 );
   fChain->SetBranchStatus("Event*", 1 );
   fChain->SetBranchStatus("Track*", 1 );
}

ProdPicoDst::~ProdPicoDst()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ProdPicoDst::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ProdPicoDst::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ProdPicoDst::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Event", &Event_, &b_Event_);
   fChain->SetBranchAddress("Event.mRunId", Event_mRunId, &b_Event_mRunId);
   fChain->SetBranchAddress("Event.mEventId", Event_mEventId, &b_Event_mEventId);
   fChain->SetBranchAddress("Event.mFillId", Event_mFillId, &b_Event_mFillId);
   fChain->SetBranchAddress("Event.mBField", Event_mBField, &b_Event_mBField);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX1", Event_mPrimaryVertex_mX1, &b_Event_mPrimaryVertex_mX1);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX2", Event_mPrimaryVertex_mX2, &b_Event_mPrimaryVertex_mX2);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX3", Event_mPrimaryVertex_mX3, &b_Event_mPrimaryVertex_mX3);
   fChain->SetBranchAddress("Event.mSecondVertex.mX1", Event_mSecondVertex_mX1, &b_Event_mSecondVertex_mX1);
   fChain->SetBranchAddress("Event.mSecondVertex.mX2", Event_mSecondVertex_mX2, &b_Event_mSecondVertex_mX2);
   fChain->SetBranchAddress("Event.mSecondVertex.mX3", Event_mSecondVertex_mX3, &b_Event_mSecondVertex_mX3);
   fChain->SetBranchAddress("Event.mTriggerWord", Event_mTriggerWord, &b_Event_mTriggerWord);
   fChain->SetBranchAddress("Event.mRefMultFtpcEast", Event_mRefMultFtpcEast, &b_Event_mRefMultFtpcEast);
   fChain->SetBranchAddress("Event.mRefMultFtpcWest", Event_mRefMultFtpcWest, &b_Event_mRefMultFtpcWest);
   fChain->SetBranchAddress("Event.mRefMultNeg", Event_mRefMultNeg, &b_Event_mRefMultNeg);
   fChain->SetBranchAddress("Event.mRefMultPos", Event_mRefMultPos, &b_Event_mRefMultPos);
   fChain->SetBranchAddress("Event.mRefMult2NegEast", Event_mRefMult2NegEast, &b_Event_mRefMult2NegEast);
   fChain->SetBranchAddress("Event.mRefMult2PosEast", Event_mRefMult2PosEast, &b_Event_mRefMult2PosEast);
   fChain->SetBranchAddress("Event.mRefMult2NegWest", Event_mRefMult2NegWest, &b_Event_mRefMult2NegWest);
   fChain->SetBranchAddress("Event.mRefMult2PosWest", Event_mRefMult2PosWest, &b_Event_mRefMult2PosWest);
   fChain->SetBranchAddress("Event.mRefMultHalfNegEast", Event_mRefMultHalfNegEast, &b_Event_mRefMultHalfNegEast);
   fChain->SetBranchAddress("Event.mRefMultHalfPosEast", Event_mRefMultHalfPosEast, &b_Event_mRefMultHalfPosEast);
   fChain->SetBranchAddress("Event.mRefMultHalfNegWest", Event_mRefMultHalfNegWest, &b_Event_mRefMultHalfNegWest);
   fChain->SetBranchAddress("Event.mRefMultHalfPosWest", Event_mRefMultHalfPosWest, &b_Event_mRefMultHalfPosWest);
   fChain->SetBranchAddress("Event.mNVpdHitsEast", Event_mNVpdHitsEast, &b_Event_mNVpdHitsEast);
   fChain->SetBranchAddress("Event.mNVpdHitsWest", Event_mNVpdHitsWest, &b_Event_mNVpdHitsWest);
   fChain->SetBranchAddress("Event.mNT0", Event_mNT0, &b_Event_mNT0);
   fChain->SetBranchAddress("Event.mVzVpd", Event_mVzVpd, &b_Event_mVzVpd);
   fChain->SetBranchAddress("Event.mZDCx", Event_mZDCx, &b_Event_mZDCx);
   fChain->SetBranchAddress("Event.mBBCx", Event_mBBCx, &b_Event_mBBCx);
   fChain->SetBranchAddress("Event.mBackgroundRate", Event_mBackgroundRate, &b_Event_mBackgroundRate);
   fChain->SetBranchAddress("Event.mBbcBlueBackgroundRate", Event_mBbcBlueBackgroundRate, &b_Event_mBbcBlueBackgroundRate);
   fChain->SetBranchAddress("Event.mBbcYellowBackgroundRate", Event_mBbcYellowBackgroundRate, &b_Event_mBbcYellowBackgroundRate);
   fChain->SetBranchAddress("Event.mBbcEastRate", Event_mBbcEastRate, &b_Event_mBbcEastRate);
   fChain->SetBranchAddress("Event.mBbcWestRate", Event_mBbcWestRate, &b_Event_mBbcWestRate);
   fChain->SetBranchAddress("Event.mZdcEastRate", Event_mZdcEastRate, &b_Event_mZdcEastRate);
   fChain->SetBranchAddress("Event.mZdcWestRate", Event_mZdcWestRate, &b_Event_mZdcWestRate);
   fChain->SetBranchAddress("Event.mVpd[64]", Event_mVpd, &b_Event_mVpd);
   fChain->SetBranchAddress("Event.mZdcSumAdcEast", Event_mZdcSumAdcEast, &b_Event_mZdcSumAdcEast);
   fChain->SetBranchAddress("Event.mZdcSumAdcWest", Event_mZdcSumAdcWest, &b_Event_mZdcSumAdcWest);
   fChain->SetBranchAddress("Event.mZdcSmdEastHorizontal[8]", Event_mZdcSmdEastHorizontal, &b_Event_mZdcSmdEastHorizontal);
   fChain->SetBranchAddress("Event.mZdcSmdEastVertical[8]", Event_mZdcSmdEastVertical, &b_Event_mZdcSmdEastVertical);
   fChain->SetBranchAddress("Event.mZdcSmdWestHorizontal[8]", Event_mZdcSmdWestHorizontal, &b_Event_mZdcSmdWestHorizontal);
   fChain->SetBranchAddress("Event.mZdcSmdWestVertical[8]", Event_mZdcSmdWestVertical, &b_Event_mZdcSmdWestVertical);
   fChain->SetBranchAddress("Event.mSpaceCharge", Event_mSpaceCharge, &b_Event_mSpaceCharge);
   fChain->SetBranchAddress("Event.mbTofTrayMultiplicity", Event_mbTofTrayMultiplicity, &b_Event_mbTofTrayMultiplicity);
   fChain->SetBranchAddress("Event.mNumberOfGlobalTracks", Event_mNumberOfGlobalTracks, &b_Event_mNumberOfGlobalTracks);
   fChain->SetBranchAddress("Event.mRanking", Event_mRanking, &b_Event_mRanking);
   fChain->SetBranchAddress("Event.mNBEMCMatch", Event_mNBEMCMatch, &b_Event_mNBEMCMatch);
   fChain->SetBranchAddress("Event.mNBTOFMatch", Event_mNBTOFMatch, &b_Event_mNBTOFMatch);
   fChain->SetBranchAddress("Event.mBbcAdcEast[24]", Event_mBbcAdcEast, &b_Event_mBbcAdcEast);
   fChain->SetBranchAddress("Event.mBbcAdcWest[24]", Event_mBbcAdcWest, &b_Event_mBbcAdcWest);
   fChain->SetBranchAddress("Event.mHT_Th[3]", Event_mHT_Th, &b_Event_mHT_Th);
   fChain->SetBranchAddress("Event.mJP_Th[3]", Event_mJP_Th, &b_Event_mJP_Th);
   fChain->SetBranchAddress("Tracks", &Tracks_, &b_Tracks_);
   fChain->SetBranchAddress("Tracks.mId", Tracks_mId, &b_Tracks_mId);
   fChain->SetBranchAddress("Tracks.mChi2", Tracks_mChi2, &b_Tracks_mChi2);
   fChain->SetBranchAddress("Tracks.mChi2Prob", Tracks_mChi2Prob, &b_Tracks_mChi2Prob);
   fChain->SetBranchAddress("Tracks.mGMomentum.mX1", Tracks_mGMomentum_mX1, &b_Tracks_mGMomentum_mX1);
   fChain->SetBranchAddress("Tracks.mGMomentum.mX2", Tracks_mGMomentum_mX2, &b_Tracks_mGMomentum_mX2);
   fChain->SetBranchAddress("Tracks.mGMomentum.mX3", Tracks_mGMomentum_mX3, &b_Tracks_mGMomentum_mX3);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX1", Tracks_mPMomentum_mX1, &b_Tracks_mPMomentum_mX1);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX2", Tracks_mPMomentum_mX2, &b_Tracks_mPMomentum_mX2);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX3", Tracks_mPMomentum_mX3, &b_Tracks_mPMomentum_mX3);
   fChain->SetBranchAddress("Tracks.mFlowFlag", Tracks_mFlowFlag, &b_Tracks_mFlowFlag);
   fChain->SetBranchAddress("Tracks.mOriginX", Tracks_mOriginX, &b_Tracks_mOriginX);
   fChain->SetBranchAddress("Tracks.mOriginY", Tracks_mOriginY, &b_Tracks_mOriginY);
   fChain->SetBranchAddress("Tracks.mOriginZ", Tracks_mOriginZ, &b_Tracks_mOriginZ);
   fChain->SetBranchAddress("Tracks.mGDca", Tracks_mGDca, &b_Tracks_mGDca);
   fChain->SetBranchAddress("Tracks.mDedx", Tracks_mDedx, &b_Tracks_mDedx);
   fChain->SetBranchAddress("Tracks.mNHitsFit", Tracks_mNHitsFit, &b_Tracks_mNHitsFit);
   fChain->SetBranchAddress("Tracks.mNHitsMax", Tracks_mNHitsMax, &b_Tracks_mNHitsMax);
   fChain->SetBranchAddress("Tracks.mNHitsDedx", Tracks_mNHitsDedx, &b_Tracks_mNHitsDedx);
   fChain->SetBranchAddress("Tracks.mNSigmaPion", Tracks_mNSigmaPion, &b_Tracks_mNSigmaPion);
   fChain->SetBranchAddress("Tracks.mNSigmaKaon", Tracks_mNSigmaKaon, &b_Tracks_mNSigmaKaon);
   fChain->SetBranchAddress("Tracks.mNSigmaProton", Tracks_mNSigmaProton, &b_Tracks_mNSigmaProton);
   fChain->SetBranchAddress("Tracks.mNSigmaElectron", Tracks_mNSigmaElectron, &b_Tracks_mNSigmaElectron);
   fChain->SetBranchAddress("Tracks.mBTofCellId", Tracks_mBTofCellId, &b_Tracks_mBTofCellId);
   fChain->SetBranchAddress("Tracks.mBTofMatchFlag", Tracks_mBTofMatchFlag, &b_Tracks_mBTofMatchFlag);
   fChain->SetBranchAddress("Tracks.mBTof", Tracks_mBTof, &b_Tracks_mBTof);
   fChain->SetBranchAddress("Tracks.mBTofBeta", Tracks_mBTofBeta, &b_Tracks_mBTofBeta);
   fChain->SetBranchAddress("Tracks.mBTofYLocal", Tracks_mBTofYLocal, &b_Tracks_mBTofYLocal);
   fChain->SetBranchAddress("Tracks.mBTofZLocal", Tracks_mBTofZLocal, &b_Tracks_mBTofZLocal);
   fChain->SetBranchAddress("Tracks.mBTofHitPosX", Tracks_mBTofHitPosX, &b_Tracks_mBTofHitPosX);
   fChain->SetBranchAddress("Tracks.mBTofHitPosY", Tracks_mBTofHitPosY, &b_Tracks_mBTofHitPosY);
   fChain->SetBranchAddress("Tracks.mBTofHitPosZ", Tracks_mBTofHitPosZ, &b_Tracks_mBTofHitPosZ);
   fChain->SetBranchAddress("Trigger", &Trigger_, &b_Trigger_);
   fChain->SetBranchAddress("Trigger.fUniqueID", &Trigger_fUniqueID, &b_Trigger_fUniqueID);
   fChain->SetBranchAddress("Trigger.fBits", &Trigger_fBits, &b_Trigger_fBits);
   fChain->SetBranchAddress("Trigger.mFlag", &Trigger_mFlag, &b_Trigger_mFlag);
   fChain->SetBranchAddress("Trigger.mId", &Trigger_mId, &b_Trigger_mId);
   fChain->SetBranchAddress("Trigger.mAdc", &Trigger_mAdc, &b_Trigger_mAdc);
   fChain->SetBranchAddress("BTOWHit", &BTOWHit_, &b_BTOWHit_);
   fChain->SetBranchAddress("BTOWHit.fUniqueID", &BTOWHit_fUniqueID, &b_BTOWHit_fUniqueID);
   fChain->SetBranchAddress("BTOWHit.fBits", &BTOWHit_fBits, &b_BTOWHit_fBits);
   fChain->SetBranchAddress("BTOWHit.mId", &BTOWHit_mId, &b_BTOWHit_mId);
   fChain->SetBranchAddress("BTOWHit.mAdc", &BTOWHit_mAdc, &b_BTOWHit_mAdc);
   fChain->SetBranchAddress("BTOWHit.mE", &BTOWHit_mE, &b_BTOWHit_mE);
   fChain->SetBranchAddress("BTofHit", &BTofHit_, &b_BTofHit_);
   fChain->SetBranchAddress("BTofHit.fUniqueID", BTofHit_fUniqueID, &b_BTofHit_fUniqueID);
   fChain->SetBranchAddress("BTofHit.fBits", BTofHit_fBits, &b_BTofHit_fBits);
   fChain->SetBranchAddress("BTofHit.mId", BTofHit_mId, &b_BTofHit_mId);
   fChain->SetBranchAddress("V0Ks", &V0Ks_, &b_V0Ks_);
   fChain->SetBranchAddress("V0Ks.mIndex2Track[2]", &V0Ks_mIndex2Track, &b_V0Ks_mIndex2Track);
   fChain->SetBranchAddress("V0Ks.mV0Pos.mX1", &V0Ks_mV0Pos_mX1, &b_V0Ks_mV0Pos_mX1);
   fChain->SetBranchAddress("V0Ks.mV0Pos.mX2", &V0Ks_mV0Pos_mX2, &b_V0Ks_mV0Pos_mX2);
   fChain->SetBranchAddress("V0Ks.mV0Pos.mX3", &V0Ks_mV0Pos_mX3, &b_V0Ks_mV0Pos_mX3);
   fChain->SetBranchAddress("V0Ks.mDcaDaughters", &V0Ks_mDcaDaughters, &b_V0Ks_mDcaDaughters);
   fChain->SetBranchAddress("V0Ks.mDca2Vtx", &V0Ks_mDca2Vtx, &b_V0Ks_mDca2Vtx);
   fChain->SetBranchAddress("V0Ks.mM", &V0Ks_mM, &b_V0Ks_mM);
   fChain->SetBranchAddress("V0L", &V0L_, &b_V0L_);
   fChain->SetBranchAddress("V0L.mIndex2Track[2]", &V0L_mIndex2Track, &b_V0L_mIndex2Track);
   fChain->SetBranchAddress("V0L.mV0Pos.mX1", &V0L_mV0Pos_mX1, &b_V0L_mV0Pos_mX1);
   fChain->SetBranchAddress("V0L.mV0Pos.mX2", &V0L_mV0Pos_mX2, &b_V0L_mV0Pos_mX2);
   fChain->SetBranchAddress("V0L.mV0Pos.mX3", &V0L_mV0Pos_mX3, &b_V0L_mV0Pos_mX3);
   fChain->SetBranchAddress("V0L.mDcaDaughters", &V0L_mDcaDaughters, &b_V0L_mDcaDaughters);
   fChain->SetBranchAddress("V0L.mDca2Vtx", &V0L_mDca2Vtx, &b_V0L_mDca2Vtx);
   fChain->SetBranchAddress("V0L.mM", &V0L_mM, &b_V0L_mM);
   fChain->SetBranchAddress("V0Lbar", &V0Lbar_, &b_V0Lbar_);
   fChain->SetBranchAddress("V0Lbar.mIndex2Track[2]", &V0Lbar_mIndex2Track, &b_V0Lbar_mIndex2Track);
   fChain->SetBranchAddress("V0Lbar.mV0Pos.mX1", &V0Lbar_mV0Pos_mX1, &b_V0Lbar_mV0Pos_mX1);
   fChain->SetBranchAddress("V0Lbar.mV0Pos.mX2", &V0Lbar_mV0Pos_mX2, &b_V0Lbar_mV0Pos_mX2);
   fChain->SetBranchAddress("V0Lbar.mV0Pos.mX3", &V0Lbar_mV0Pos_mX3, &b_V0Lbar_mV0Pos_mX3);
   fChain->SetBranchAddress("V0Lbar.mDcaDaughters", &V0Lbar_mDcaDaughters, &b_V0Lbar_mDcaDaughters);
   fChain->SetBranchAddress("V0Lbar.mDca2Vtx", &V0Lbar_mDca2Vtx, &b_V0Lbar_mDca2Vtx);
   fChain->SetBranchAddress("V0Lbar.mM", &V0Lbar_mM, &b_V0Lbar_mM);
   Notify();
}

Bool_t ProdPicoDst::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ProdPicoDst::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ProdPicoDst::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}