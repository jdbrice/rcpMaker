#include "FemtoDst.h"


FemtoDst::FemtoDst(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("femtoDst.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("femtoDst.root");
      }
      f->GetObject("FemtoDst",tree);

   }
   Init(tree);
}

FemtoDst::~FemtoDst()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t FemtoDst::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t FemtoDst::LoadTree(Long64_t entry)
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

void FemtoDst::Init(TTree *tree)
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

   fChain->SetBranchAddress("Event.mTriggerWord", &Event_mTriggerWord, &b_Event_mTriggerWord);
   fChain->SetBranchAddress("Event.mRunId", &Event_mRunId, &b_Event_mRunId);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX1", &Event_mPrimaryVertex_mX1, &b_Event_mPrimaryVertex_mX1);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX2", &Event_mPrimaryVertex_mX2, &b_Event_mPrimaryVertex_mX2);
   fChain->SetBranchAddress("Event.mPrimaryVertex.mX3", &Event_mPrimaryVertex_mX3, &b_Event_mPrimaryVertex_mX3);
   fChain->SetBranchAddress("Event.mRefMult", &Event_mRefMult, &b_Event_mRefMult);
   fChain->SetBranchAddress("Event.mNumberOfPrimaryTracks", &Event_mNumberOfPrimaryTracks, &b_Event_mNumberOfPrimaryTracks);
   fChain->SetBranchAddress("Event.mNBTOFMatchedTracks", &Event_mNBTOFMatchedTracks, &b_Event_mNBTOFMatchedTracks);
   fChain->SetBranchAddress("Tracks.mEta", Tracks_mEta, &b_Tracks_mEta);
   fChain->SetBranchAddress("Tracks.mPt", Tracks_mPt, &b_Tracks_mPt);
   fChain->SetBranchAddress("Tracks.mDedx", Tracks_mDedx, &b_Tracks_mDedx);
   fChain->SetBranchAddress("Tracks.mBTofBeta", Tracks_mBTofBeta, &b_Tracks_mBTofBeta);
   fChain->SetBranchAddress("Tracks.mBTofYLocal", Tracks_mBTofYLocal, &b_Tracks_mBTofYLocal);
   fChain->SetBranchAddress("Tracks.mBTofZLocal", Tracks_mBTofZLocal, &b_Tracks_mBTofZLocal);
   fChain->SetBranchAddress("Tracks.mGDca", Tracks_mGDca, &b_Tracks_mGDca);
   fChain->SetBranchAddress("Tracks.mCharge", Tracks_mCharge, &b_Tracks_mCharge);
   fChain->SetBranchAddress("Tracks.mNHitsFit", Tracks_mNHitsFit, &b_Tracks_mNHitsFit);
   fChain->SetBranchAddress("Tracks.mNHitsMax", Tracks_mNHitsMax, &b_Tracks_mNHitsMax);
   fChain->SetBranchAddress("Tracks.mNHitsDedx", Tracks_mNHitsDedx, &b_Tracks_mNHitsDedx);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX1", Tracks_mPMomentum_mX1, &b_Tracks_mPMomentum_mX1);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX2", Tracks_mPMomentum_mX2, &b_Tracks_mPMomentum_mX2);
   fChain->SetBranchAddress("Tracks.mPMomentum.mX3", Tracks_mPMomentum_mX3, &b_Tracks_mPMomentum_mX3);
   Notify();
}

Bool_t FemtoDst::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void FemtoDst::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t FemtoDst::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
