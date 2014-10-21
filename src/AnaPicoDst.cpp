#include "AnaPicoDst.h"
#include <iostream>
using namespace std;


AnaPicoDst::AnaPicoDst(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      cout << "[AnaPicoDst] Error Invalid Tree " << endl;
      return;
   }
   Init(tree);
}

AnaPicoDst::~AnaPicoDst()
{
   //if (!fChain) return;
   //delete fChain->GetCurrentFile();
}

Int_t AnaPicoDst::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t AnaPicoDst::LoadTree(Long64_t entry)
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

void AnaPicoDst::Init(TTree *tree)
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

   fChain->SetBranchAddress("runid", &runid, &b_runid);
   fChain->SetBranchAddress("evtid", &evtid, &b_evtid);
   fChain->SetBranchAddress("itime", &itime, &b_itime);
   fChain->SetBranchAddress("refmult", &refmult, &b_refmult);
   fChain->SetBranchAddress("refmult2", &refmult2, &b_refmult2);
   fChain->SetBranchAddress("refmult3", &refmult3, &b_refmult3);
   fChain->SetBranchAddress("nprimary", &nprimary, &b_nprimary);
   fChain->SetBranchAddress("nglobal", &nglobal, &b_nglobal);
   fChain->SetBranchAddress("tofmult", &tofmult, &b_tofmult);
   fChain->SetBranchAddress("tofmatch", &tofmatch, &b_tofmatch);
   fChain->SetBranchAddress("vtx", vtx, &b_vtx);
   fChain->SetBranchAddress("vtxe", vtxe, &b_vtxe);
   fChain->SetBranchAddress("vtxrank", &vtxrank, &b_vtxrank);
   fChain->SetBranchAddress("trigindex", &trigindex, &b_trigindex);
   fChain->SetBranchAddress("trigindexupper", &trigindexupper, &b_trigindexupper);
   fChain->SetBranchAddress("field", &field, &b_field);
   fChain->SetBranchAddress("bbccoinrate", &bbccoinrate, &b_bbccoinrate);
   fChain->SetBranchAddress("zdccoinrate", &zdccoinrate, &b_zdccoinrate);
   fChain->SetBranchAddress("bbcsumadc", &bbcsumadc, &b_bbcsumadc);
   fChain->SetBranchAddress("zdcsumadc", &zdcsumadc, &b_zdcsumadc);
   fChain->SetBranchAddress("vpdsumadc", &vpdsumadc, &b_vpdsumadc);
   fChain->SetBranchAddress("emcsumadc", &emcsumadc, &b_emcsumadc);
   fChain->SetBranchAddress("emcsume", &emcsume, &b_emcsume);
   fChain->SetBranchAddress("bemcntow4rms", &bemcntow4rms, &b_bemcntow4rms);
   fChain->SetBranchAddress("etaasym", &etaasym, &b_etaasym);
   fChain->SetBranchAddress("etaasymtof", &etaasymtof, &b_etaasymtof);
   fChain->SetBranchAddress("ntrkgl", &ntrkgl, &b_ntrkgl);
   fChain->SetBranchAddress("ntrk", &ntrk, &b_ntrk);
   fChain->SetBranchAddress("flag", flag, &b_flag);
   fChain->SetBranchAddress("eta", eta, &b_eta);
   fChain->SetBranchAddress("phi", phi, &b_phi);
   fChain->SetBranchAddress("pt", pt, &b_pt);
   fChain->SetBranchAddress("ptot", ptot, &b_ptot);
   fChain->SetBranchAddress("chg", chg, &b_chg);
   fChain->SetBranchAddress("nhitsfit", nhitsfit, &b_nhitsfit);
   fChain->SetBranchAddress("nhitsdedx", nhitsdedx, &b_nhitsdedx);
   fChain->SetBranchAddress("nhitsposs", nhitsposs, &b_nhitsposs);
   fChain->SetBranchAddress("chi2", chi2, &b_chi2);
   fChain->SetBranchAddress("gldcax", gldcax, &b_gldcax);
   fChain->SetBranchAddress("gldcay", gldcay, &b_gldcay);
   fChain->SetBranchAddress("gldcaz", gldcaz, &b_gldcaz);
   fChain->SetBranchAddress("dedx", dedx, &b_dedx);
   fChain->SetBranchAddress("nsigtpc_e", nsigtpc_e, &b_nsigtpc_e);
   fChain->SetBranchAddress("nsigtpc_pi", nsigtpc_pi, &b_nsigtpc_pi);
   fChain->SetBranchAddress("nsigtpc_k", nsigtpc_k, &b_nsigtpc_k);
   fChain->SetBranchAddress("nsigtpc_p", nsigtpc_p, &b_nsigtpc_p);
   fChain->SetBranchAddress("defvpdvtxz", &defvpdvtxz, &b_defvpdvtxz);
   fChain->SetBranchAddress("defmatchFlag", defmatchFlag, &b_defmatchFlag);
   fChain->SetBranchAddress("defyLocal", defyLocal, &b_defyLocal);
   fChain->SetBranchAddress("defzLocal", defzLocal, &b_defzLocal);
   fChain->SetBranchAddress("defpathLength", defpathLength, &b_defpathLength);
   fChain->SetBranchAddress("deftot", deftot, &b_deftot);
   fChain->SetBranchAddress("deftof", deftof, &b_deftof);
   fChain->SetBranchAddress("definvbeta", definvbeta, &b_definvbeta);

   Notify();
}

Bool_t AnaPicoDst::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void AnaPicoDst::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t AnaPicoDst::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
