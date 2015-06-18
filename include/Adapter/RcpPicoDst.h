//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Apr 13 20:37:06 2015 by ROOT version 5.34/25
// from TTree RcpPicoDst/Rcp Pid Spectra Data
// found on file: allTuple.root
//////////////////////////////////////////////////////////

#ifndef RCP_PICO_DST_H
#define RCP_PICO_DST_H

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "PicoDataStore.h"
#include "TMath.h"

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.
const int kMaxTracks = 5000;

class RcpPicoDst : public PicoDataStore{
public:



   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   TTree * getTree() { return fChain; }
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           rrunId;
   Float_t         weight;
   Float_t         corrRefMult;
   UShort_t        bin9;
   UShort_t        bin16;

   Int_t           nTracks;
   Float_t         ppT[kMaxTracks];   //[nTracks]
   Float_t         pP[kMaxTracks];   //[nTracks]
   Float_t         pEta[kMaxTracks];   //[nTracks]
   UShort_t        dedx[kMaxTracks];   //[nTracks]
   UShort_t        beta[kMaxTracks];   //[nTracks]
   Float_t         yLocal[ kMaxTracks ];
   Float_t         zLocal[ kMaxTracks ];
   Float_t         matchFlag[ kMaxTracks ];

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_bin9;   //!
   TBranch        *b_bin16;   //!
   TBranch        *b_corrRefMult;   //!
   TBranch        *b_nTracks;   //!
   TBranch        *b_ppT;   //!
   TBranch        *b_pP;   //!
   TBranch        *b_pEta;   //!
   TBranch        *b_dedx;   //!
   TBranch        *b_beta;   //!
   TBranch        *b_yLocal;   //!
   TBranch        *b_zLocal;   //!
   TBranch        *b_matchFlag;   //!

   RcpPicoDst(TTree *tree=0);
   virtual ~RcpPicoDst();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   inline virtual Int_t runId(){ return rrunId; }
   inline virtual Float_t eventWeight(){ return weight; }
   inline virtual Float_t refMult(){ return corrRefMult; }
   inline virtual Int_t numTracks(){ return nTracks; }
   inline virtual Double_t trackPt( Int_t iHit ){ 
      return TMath::Abs(ppT[iHit]); 
   }
   inline virtual Double_t trackP( Int_t iHit ){ 
      return pP[iHit]; 
   }
   inline virtual Int_t trackCharge( Int_t iHit ){ 

      if ( ppT[iHit] > 0 )
         return 1;
      else if ( ppT[iHit] < 0 )
         return -1;
      
      return 0; 
   }
   inline virtual Double_t trackEta( Int_t iHit ){ 
      return pEta[ iHit ];
   }

   inline virtual Double_t trackDedx( Int_t iHit ){ return ((Float_t)dedx[ iHit ]) / 1000.0; }
   inline virtual Double_t trackBeta( Int_t iHit ){ return ((Float_t)beta[ iHit ] / 20000.0); }
   inline virtual Double_t trackYLocal( Int_t iHit ){ return yLocal[ iHit ]; }
   inline virtual Double_t trackZLocal( Int_t iHit ){ return zLocal[ iHit ]; }
   inline virtual Int_t trackTofMatch( Int_t iHit ){ return matchFlag[ iHit ]; }

   inline virtual UShort_t b9() { return bin9; }
   inline virtual UShort_t b16() { return bin16; }

};

#endif

