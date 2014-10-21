#ifndef ANA_PICO_DST_H
#define ANA_PICO_DST_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "PicoDataStore.h"

class AnaPicoDst : public PicoDataStore {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   UInt_t          runid;
   UInt_t          evtid;
   UInt_t          itime;
   Short_t         refmult;
   Short_t         refmult2;
   Short_t         refmult3;
   Short_t         nprimary;
   Short_t         nglobal;
   Short_t         tofmult;
   Short_t         tofmatch;
   Float_t         vtx[3];
   Float_t         vtxe[3];
   Float_t         vtxrank;
   UInt_t          trigindex;
   UInt_t          trigindexupper;
   Float_t         field;
   Float_t         bbccoinrate;
   Float_t         zdccoinrate;
   Float_t         bbcsumadc;
   Float_t         zdcsumadc;
   Float_t         vpdsumadc;
   Float_t         emcsumadc;
   Float_t         emcsume;
   Int_t           bemcntow4rms;
   Float_t         etaasym;
   Float_t         etaasymtof;
   Int_t           ntrkgl;
   Int_t           ntrk;
   Int_t           flag[609];   //[ntrk]
   Float_t         eta[609];   //[ntrk]
   Float_t         phi[609];   //[ntrk]
   Float_t         pt[609];   //[ntrk]
   Float_t         ptot[609];   //[ntrk]
   Short_t         chg[609];   //[ntrk]
   UChar_t         nhitsfit[609];   //[ntrk]
   UChar_t         nhitsdedx[609];   //[ntrk]
   UChar_t         nhitsposs[609];   //[ntrk]
   Float_t         chi2[609];   //[ntrk]
   Float_t         gldcax[609];   //[ntrk]
   Float_t         gldcay[609];   //[ntrk]
   Float_t         gldcaz[609];   //[ntrk]
   Float_t         dedx[609];   //[ntrk]
   Short_t         nsigtpc_e[609];   //[ntrk]
   Short_t         nsigtpc_pi[609];   //[ntrk]
   Short_t         nsigtpc_k[609];   //[ntrk]
   Short_t         nsigtpc_p[609];   //[ntrk]
   Float_t         defvpdvtxz;
   UShort_t        defmatchFlag[609];   //[ntrk]
   Float_t         defyLocal[609];   //[ntrk]
   Float_t         defzLocal[609];   //[ntrk]
   Float_t         defpathLength[609];   //[ntrk]
   Float_t         deftot[609];   //[ntrk]
   Float_t         deftof[609];   //[ntrk]
   Float_t         definvbeta[609];   //[ntrk]

   // List of branches
   TBranch        *b_runid;   //!
   TBranch        *b_evtid;   //!
   TBranch        *b_itime;   //!
   TBranch        *b_refmult;   //!
   TBranch        *b_refmult2;   //!
   TBranch        *b_refmult3;   //!
   TBranch        *b_nprimary;   //!
   TBranch        *b_nglobal;   //!
   TBranch        *b_tofmult;   //!
   TBranch        *b_tofmatch;   //!
   TBranch        *b_vtx;   //!
   TBranch        *b_vtxe;   //!
   TBranch        *b_vtxrank;   //!
   TBranch        *b_trigindex;   //!
   TBranch        *b_trigindexupper;   //!
   TBranch        *b_field;   //!
   TBranch        *b_bbccoinrate;   //!
   TBranch        *b_zdccoinrate;   //!
   TBranch        *b_bbcsumadc;   //!
   TBranch        *b_zdcsumadc;   //!
   TBranch        *b_vpdsumadc;   //!
   TBranch        *b_emcsumadc;   //!
   TBranch        *b_emcsume;   //!
   TBranch        *b_bemcntow4rms;   //!
   TBranch        *b_etaasym;   //!
   TBranch        *b_etaasymtof;   //!
   TBranch        *b_ntrkgl;   //!
   TBranch        *b_ntrk;   //!
   TBranch        *b_flag;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_ptot;   //!
   TBranch        *b_chg;   //!
   TBranch        *b_nhitsfit;   //!
   TBranch        *b_nhitsdedx;   //!
   TBranch        *b_nhitsposs;   //!
   TBranch        *b_chi2;   //!
   TBranch        *b_gldcax;   //!
   TBranch        *b_gldcay;   //!
   TBranch        *b_gldcaz;   //!
   TBranch        *b_dedx;   //!
   TBranch        *b_nsigtpc_e;   //!
   TBranch        *b_nsigtpc_pi;   //!
   TBranch        *b_nsigtpc_k;   //!
   TBranch        *b_nsigtpc_p;   //!
   TBranch        *b_defvpdvtxz;   //!
   TBranch        *b_defmatchFlag;   //!
   TBranch        *b_defyLocal;   //!
   TBranch        *b_defzLocal;   //!
   TBranch        *b_defpathLength;   //!
   TBranch        *b_deftot;   //!
   TBranch        *b_deftof;   //!
   TBranch        *b_definvbeta;   //!


   AnaPicoDst(TTree *tree=0);

   static const Int_t startWest = 0;
   static const Int_t endWest = 19;
   static const Int_t trgEndWest = 16;

   static const Int_t startEast = 19;
   static const Int_t endEast = 38;
   static const Int_t trgEndEast = 35;

   virtual ~AnaPicoDst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   //virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   const Int_t TrigIDs14_015[4]     = {440005,           // BBC_MB
                                       440015,           // BBC_MB
                                       440006,           // bbc_mon_tof
                                       440016 };

   /**
    * Implement the PicoDataStore Interface
    */
   
   virtual TTree * getTree() { return fChain; }
   
   virtual Int_t numTofTracks(){ return tofmult; }
   virtual Double_t trackPt( Int_t iHit ) { return pt[ iHit ]; }
   virtual Double_t trackP( Int_t iHit ){ return ptot[ iHit ]; }
   virtual Double_t trackEta( Int_t iHit ) { return eta[ iHit ]; }
   virtual Double_t trackDedx( Int_t iHit ){ return dedx[ iHit ]; }
   virtual Double_t trackPathLength( Int_t iHit ){ return defpathLength[ iHit ]; }
   virtual Double_t trackTof( Int_t iHit ){ return deftof[ iHit ]; }
   virtual Double_t trackBeta( Int_t iHit ){ return (1.0/definvbeta[ iHit ]); }
   virtual Int_t trackCharge( Int_t iHit ){ return chg[ iHit ]; }

   virtual Double_t eventVertexX( ){ return vtx[0]; }
   virtual Double_t eventVertexY( ){ return vtx[1]; }
   virtual Double_t eventVertexZ( ){ return vtx[2]; }

   virtual Double_t trackDcaX( Int_t iHit ){ return gldcax[iHit]; }
   virtual Double_t trackDcaY( Int_t iHit ){ return gldcay[iHit]; }
   virtual Double_t trackDcaZ( Int_t iHit ){ return gldcaz[iHit]; }

   virtual Double_t trackYLocal( Int_t iHit ){ return defyLocal[iHit]; }

   virtual Double_t eventNTZero() { return 0; }
   virtual Double_t eventRefMult() { return refmult; }
   virtual Double_t eventRefMultPos() { return 0; }
   virtual Double_t eventRefMultNeg() { return 0; }

   virtual Double_t trackNHits( Int_t iHit ) { return 999; } // not included but cut > 15
   virtual Double_t trackNHitsDedx( Int_t iHit ){return nhitsdedx[ iHit ]; }
   virtual Double_t trackNHitsFit( Int_t iHit ){ return nhitsfit[ iHit ]; }
   virtual Double_t trackNHitsPossible( Int_t iHit ) { return nhitsposs[ iHit ]; }

   virtual Int_t numEventTriggers() { return 0; }
   virtual std::vector<UInt_t> eventTriggerIds() { 

      std::vector<UInt_t> v; 
      if ( trigindex & 1 )
         v.push_back( TrigIDs14_015[0] );
      if ( trigindex & 2 )
         v.push_back( TrigIDs14_015[1] );
      if ( trigindex & 4 )
         v.push_back( TrigIDs14_015[2] );
      if ( trigindex & 8 )
         v.push_back( TrigIDs14_015[3] );
      return v;
   }


};

#endif

