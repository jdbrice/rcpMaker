#include "TofPicoDst.h"
#include <iostream>
using namespace std;















TofPicoDst::TofPicoDst(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      cout << "[TofPicoDst] Error Invalid Tree " << endl;
      return;
   }
   Init(tree);
}

TofPicoDst::~TofPicoDst()
{
   //if (!fChain) return;
   //delete fChain->GetCurrentFile();
}

Int_t TofPicoDst::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TofPicoDst::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TofPicoDst::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nTriggers", &nTriggers, &b_nTriggers);
   fChain->SetBranchAddress("triggerIds", &triggerIds, &b_triggerIds);
   
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("refMult", &refMult, &b_refMult);
   fChain->SetBranchAddress("refMultNeg", &refMultNeg, &b_refMultNeg);
   fChain->SetBranchAddress("refMultPos", &refMultPos, &b_refMultPos);


   fChain->SetBranchAddress("vertexX", &vertexX, &b_vertexX);
   fChain->SetBranchAddress("vertexY", &vertexY, &b_vertexY);
   fChain->SetBranchAddress("vertexZ", &vertexZ, &b_vertexZ);
   fChain->SetBranchAddress("vpdEast", &vpdEast, &b_vpdEast);
   fChain->SetBranchAddress("vpdWest", &vpdWest, &b_vpdWest);
   fChain->SetBranchAddress("numberOfVpdEast", &numberOfVpdEast, &b_numberOfVpdEast);
   fChain->SetBranchAddress("numberOfVpdWest", &numberOfVpdWest, &b_numberOfVpdWest);
   fChain->SetBranchAddress("tDiff", &tDiff, &b_tDiff);
   fChain->SetBranchAddress("tStart", &tStart, &b_tStart);
   fChain->SetBranchAddress("nTZero", &nTZero, &b_nTZero);

   fChain->SetBranchAddress("vpdVz", &vpdVz, &b_vpdVz);
   fChain->SetBranchAddress("vpdLeEast", vpdLeEast, &b_vpdLeEast);
   fChain->SetBranchAddress("vpdLeWest", vpdLeWest, &b_vpdLeWest);
   fChain->SetBranchAddress("vpdTotEast", vpdTotEast, &b_vpdTotEast);
   fChain->SetBranchAddress("vpdTotWest", vpdTotWest, &b_vpdTotWest);
   
   // for backwards compatibility
   // works either way but prints lots of errors accessing the branch without this
   if ( fChain->GetListOfBranches()->FindObject( "vpdBbqAdcWest" ) ){
      
     fChain->SetBranchAddress("vpdBbqAdcWest", vpdBbqAdcWest, &b_vpdBbqAdcWest);
     fChain->SetBranchAddress("vpdBbqAdcEast", vpdBbqAdcEast, &b_vpdBbqAdcEast);
     fChain->SetBranchAddress("vpdBbqTdcWest", vpdBbqTdcWest, &b_vpdBbqTdcWest);
     fChain->SetBranchAddress("vpdBbqTdcEast", vpdBbqTdcEast, &b_vpdBbqTdcEast);
     
     fChain->SetBranchAddress("vpdMxqAdcWest", vpdMxqAdcWest, &b_vpdMxqAdcWest);
     fChain->SetBranchAddress("vpdMxqAdcEast", vpdMxqAdcEast, &b_vpdMxqAdcEast);
     fChain->SetBranchAddress("vpdMxqTdcWest", vpdMxqTdcWest, &b_vpdMxqTdcWest);
     fChain->SetBranchAddress("vpdMxqTdcEast", vpdMxqTdcEast, &b_vpdMxqTdcEast);
   }


   fChain->SetBranchAddress("nTofHits", &nTofHits, &b_nTofHits);
   fChain->SetBranchAddress("tray", tray, &b_tray);
   fChain->SetBranchAddress("module", module, &b_module);
   fChain->SetBranchAddress("cell", cell, &b_cell);
   fChain->SetBranchAddress("leTime", leTime, &b_leTime);
   fChain->SetBranchAddress("tot", tot, &b_tot);
   fChain->SetBranchAddress("yLocal", yLocal, &b_yLocal);
   fChain->SetBranchAddress("zLocal", zLocal, &b_zLocal);
   fChain->SetBranchAddress("thetaLocal", thetaLocal, &b_thetaLocal);
   fChain->SetBranchAddress("xGlobal", xGlobal, &b_xGlobal);
   fChain->SetBranchAddress("yGlobal", yGlobal, &b_yGlobal);
   fChain->SetBranchAddress("zGlobal", zGlobal, &b_zGlobal);
   fChain->SetBranchAddress("trackId", trackId, &b_trackId);
   fChain->SetBranchAddress("charge", charge, &b_charge);
   fChain->SetBranchAddress("pt", pt, &b_pt);
   fChain->SetBranchAddress("eta", eta, &b_eta);
   fChain->SetBranchAddress("phi", phi, &b_phi);
   fChain->SetBranchAddress("pX", pX, &b_pX);
   fChain->SetBranchAddress("pY", pY, &b_pY);
   fChain->SetBranchAddress("pZ", pZ, &b_pZ);
   fChain->SetBranchAddress("p", p, &b_p);

   fChain->SetBranchAddress("dcaX", dcaX, &b_dcaX);
   fChain->SetBranchAddress("dcaY", dcaY, &b_dcaY);
   fChain->SetBranchAddress("dcaZ", dcaZ, &b_dcaZ);
   
   fChain->SetBranchAddress("nHits", nHits, &b_nHits);
   fChain->SetBranchAddress("nHitsPossible", nHitsPossible, &b_nHitsPossible);
   fChain->SetBranchAddress("nHitsFit", nHitsFit, &b_nHitsFit);
   fChain->SetBranchAddress("nHitsDedx", nHitsDedx, &b_nHitsDedx);

   fChain->SetBranchAddress("dedx", dedx, &b_dedx);
   fChain->SetBranchAddress("nSigE", nSigE, &b_nSigE);
   fChain->SetBranchAddress("nSigPi", nSigPi, &b_nSigPi);
   fChain->SetBranchAddress("nSigK", nSigK, &b_nSigK);
   fChain->SetBranchAddress("nSigP", nSigP, &b_nSigP);
   fChain->SetBranchAddress("length", length, &b_length);
   fChain->SetBranchAddress("tof", tof, &b_tof);
   fChain->SetBranchAddress("beta", beta, &b_beta);

   Notify();
}

Bool_t TofPicoDst::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TofPicoDst::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TofPicoDst::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
