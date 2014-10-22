#ifndef REF_MULT_PICO_DST_H
#define REF_MULT_PICO_DST_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "PicoDataStore.h"

const Int_t kMaxHits = 5000;
const Int_t kMaxTriggers = 400;

class RefMultPicoDst : public PicoDataStore {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   UInt_t          runId;
   UInt_t          eventId;
   UInt_t          iDay;
   UInt_t          iYear;
   Int_t           nTriggers;
   UInt_t          triggerIds[kMaxTriggers];   //[nTriggers]
   UShort_t        refMult;
   Float_t         vertexX;
   Float_t         vertexY;
   Float_t         vertexZ;
   Int_t           nTZero;
   Int_t           tofMult;
   Int_t           nTracks;
   Int_t           nPrimary;
   Int_t           nGlobal;
   Int_t           tofMatchFlag[kMaxHits];   //[nTracks]
   Float_t         yLocal[kMaxHits];   //[nTracks]
   Float_t         zLocal[kMaxHits];   //[nTracks]
   Int_t           charge[kMaxHits];   //[nTracks]
   Float_t         pt[kMaxHits];   //[nTracks]
   Float_t         eta[kMaxHits];   //[nTracks]
   Float_t         phi[kMaxHits];   //[nTracks]
   Float_t         pX[kMaxHits];   //[nTracks]
   Float_t         pY[kMaxHits];   //[nTracks]
   Float_t         pZ[kMaxHits];   //[nTracks]
   Float_t         p[kMaxHits];   //[nTracks]
   Float_t         dcaX[kMaxHits];   //[nTracks]
   Float_t         dcaY[kMaxHits];   //[nTracks]
   Float_t         dcaZ[kMaxHits];   //[nTracks]
   Float_t         dca[kMaxHits];   //[nTracks]
   Int_t           nHits[kMaxHits];   //[nTracks]
   Int_t           nHitsFit[kMaxHits];   //[nTracks]
   Int_t           nHitsPossible[kMaxHits];   //[nTracks]
   Int_t           nHitsDedx[kMaxHits];   //[nTracks]
   Float_t         dedx[kMaxHits];   //[nTracks]
   Float_t         pathLength[kMaxHits];   //[nTracks]
   Float_t         tof[kMaxHits];   //[nTracks]
   Float_t         beta[kMaxHits];   //[nTracks]
   Float_t         bbcCoinRate;
   Float_t         zdcCoinRate;

   // List of branches
   TBranch        *b_runId;   //!
   TBranch        *b_eventId;   //!
   TBranch        *b_iDay;   //!
   TBranch        *b_iYear;   //!
   TBranch        *b_nTriggers;   //!
   TBranch        *b_triggerIds;   //!
   TBranch        *b_refMult;   //!
   TBranch        *b_vertexX;   //!
   TBranch        *b_vertexY;   //!
   TBranch        *b_vertexZ;   //!
   TBranch        *b_nTZero;   //!
   TBranch        *b_tofMult;   //!
   TBranch        *b_nTracks;   //!
   TBranch        *b_nPrimary;   //!
   TBranch        *b_nGlobal;   //!
   TBranch        *b_tofMatchFlag;   //!
   TBranch        *b_yLocal;   //!
   TBranch        *b_zLocal;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_pX;   //!
   TBranch        *b_pY;   //!
   TBranch        *b_pZ;   //!
   TBranch        *b_p;   //!
   TBranch        *b_dcaX;   //!
   TBranch        *b_dcaY;   //!
   TBranch        *b_dcaZ;   //!
   TBranch        *b_dca;   //!
   TBranch        *b_nHits;   //!
   TBranch        *b_nHitsFit;   //!
   TBranch        *b_nHitsPossible;   //!
   TBranch        *b_nHitsDedx;   //!
   TBranch        *b_dedx;   //!
   TBranch        *b_pathLength;   //!
   TBranch        *b_tof;   //!
   TBranch        *b_beta;   //!
   TBranch        *b_bbcCoinRate;   //!
   TBranch        *b_zdcCoinRate;   //!


   RefMultPicoDst(TTree *tree=0);

   virtual ~RefMultPicoDst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   //virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   /**
    * Implement the PicoDataStore Interface
    */
   
   virtual TTree * getTree() { return fChain; }
   
   virtual Int_t numTofTracks(){ return tofMult; }
   virtual Double_t trackPt( Int_t iHit ) { return pt[ iHit ]; }
   virtual Double_t trackP( Int_t iHit ){ return p[ iHit ]; }
   virtual Double_t trackEta( Int_t iHit ) { return eta[ iHit ]; }
   virtual Double_t trackDedx( Int_t iHit ){ return dedx[ iHit ]; }
   virtual Double_t trackPathLength( Int_t iHit ){ return pathLength[ iHit ]; }
   virtual Double_t trackTof( Int_t iHit ){ return tof[ iHit ]; }
   virtual Double_t trackBeta( Int_t iHit ){ return beta[ iHit ]; }
   virtual Int_t trackTofMatch( Int_t iHit ){ return tofMatchFlag[ iHit ]; }
   virtual Int_t trackCharge( Int_t iHit ){ return charge[ iHit ]; }

   virtual Double_t eventVertexX( ){ return vertexX; }
   virtual Double_t eventVertexY( ){ return vertexY; }
   virtual Double_t eventVertexZ( ){ return vertexZ; }

   virtual Double_t trackDcaX( Int_t iHit ){ return dcaX[iHit]; }
   virtual Double_t trackDcaY( Int_t iHit ){ return dcaY[iHit]; }
   virtual Double_t trackDcaZ( Int_t iHit ){ return dcaZ[iHit]; }
   virtual Double_t trackDca( Int_t iHit ){ return dcaZ[iHit]; }

   virtual Double_t trackYLocal( Int_t iHit ){ return yLocal[iHit]; }

   
   virtual Double_t eventRefMultPos() { return 0; }
   virtual Double_t eventRefMultNeg() { return 0; }

   virtual Double_t trackNHits( Int_t iHit ) { return nHits[ iHit ]; } // not included but cut > 15
   virtual Double_t trackNHitsDedx( Int_t iHit ){return nHitsDedx[ iHit ]; }
   virtual Double_t trackNHitsFit( Int_t iHit ){ return nHitsFit[ iHit ]; }
   virtual Double_t trackNHitsPossible( Int_t iHit ) { return nHitsPossible[ iHit ]; }

   virtual Int_t numEventTriggers() { return 0; }
   virtual std::vector<UInt_t> eventTriggerIds() { std::vector<UInt_t> v( std::begin(triggerIds), std::end(triggerIds) ); return v; }

   virtual UInt_t eventRunId(){ return runId; }
   virtual UInt_t eventEventId(){ return eventId; }
   virtual UInt_t eventDay(){ return iDay; }
   virtual UInt_t eventYear(){ return iYear; }

   virtual Int_t eventNTZero() { return nTZero; }
   virtual Short_t eventTofMult(){return tofMult; }
   virtual UShort_t eventRefMult(){ return refMult; }
   virtual Int_t eventNumTracks(){ return tofMult; }
   virtual Short_t eventNumPrimary(){ return nPrimary; }
   virtual Short_t eventNumGlobal(){ return nGlobal; }

   virtual Float_t eventZDC(){ return zdcCoinRate; }
   virtual Float_t eventBBC(){ return bbcCoinRate; } 


};

#endif

