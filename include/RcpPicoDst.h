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

class RcpPicoDst : public PicoDataStore{
public:



   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   TTree * getTree() { return fChain; }
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           rrunId;
   Float_t         weight;
   UShort_t        corrRefMult;
   Int_t           nTracks;
   Float_t         ppT[1000];   //[nTracks]
   Float_t         pP[1000];   //[nTracks]
   Float_t         pEta[1000];   //[nTracks]
   UShort_t        dedx[1000];   //[nTracks]
   UShort_t        beta[1000];   //[nTracks]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_corrRefMult;   //!
   TBranch        *b_nTracks;   //!
   TBranch        *b_ppT;   //!
   TBranch        *b_pP;   //!
   TBranch        *b_pEta;   //!
   TBranch        *b_dedx;   //!
   TBranch        *b_beta;   //!

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
   inline virtual UShort_t refMult(){ return corrRefMult; }
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

};

#endif

