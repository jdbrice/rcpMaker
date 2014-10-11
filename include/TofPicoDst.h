//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Apr 22 16:44:52 2009 by ROOT version 5.12/00h-rc2
// from TTree tof/BTof cell data
// found on file: ntupleAllDays.root
//////////////////////////////////////////////////////////

#ifndef TOF_PICO_DST_H
#define TOF_PICO_DST_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "PicoDataStore.h"

class TofPicoDst : public PicoDataStore {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           run;
   Int_t           evt;
   Int_t             refMult, refMultNeg, refMultPos;

   Float_t         vertexX;
   Float_t         vertexY;
   Float_t         vertexZ;
   Int_t           vpdEast;
   Int_t           vpdWest;
   Int_t           numberOfVpdEast;
   Int_t           numberOfVpdWest;
   Float_t         tDiff;
   Double_t        tStart;
   Int_t             nTZero;
   Float_t         vpdVz;
   Double_t        vpdLeEast[19];
   Double_t        vpdLeWest[19];
   Double_t        vpdTotEast[19];
   Double_t        vpdTotWest[19];

   // VPD trigger side bbq electronics
   UShort_t    vpdBbqAdcEast[16], 
               vpdBbqTdcEast[16];
   UShort_t    vpdBbqAdcWest[16], 
               vpdBbqTdcWest[16];

    // VPD trigger side mxq electronics
   UShort_t    vpdMxqAdcEast[16], 
               vpdMxqTdcEast[16];
   UShort_t    vpdMxqAdcWest[16], 
               vpdMxqTdcWest[16];

   Int_t           nTofHits;
   Int_t           tray[8000];   
   Int_t           module[8000];   
   Int_t           cell[8000];   
   Double_t        leTime[8000];   
   Double_t        tot[8000];   
   Float_t         yLocal[8000];   
   Float_t         zLocal[8000];   
   Float_t         thetaLocal[8000];   
   Float_t         xGlobal[8000];   
   Float_t         yGlobal[8000];   
   Float_t         zGlobal[8000];   
   Int_t           trackId[8000];   
   Int_t           charge[8000];   
   Float_t         pt[8000];   
   Float_t         eta[8000];   
   Float_t         phi[8000];
   Float_t         pX[8000], pY[8000], pZ[8000], p[8000];
   Float_t         dcaX[8000];   
   Float_t         dcaY[8000];   
   Float_t         dcaZ[8000];   
   Int_t           nHits[8000];   
   Int_t           nHitsPossible[8000];
   Int_t           nHitsFit[8000];   
   Int_t           nHitsDedx[8000];   
   Float_t         dedx[8000];   
   Float_t         nSigE[8000];   
   Float_t         nSigPi[8000];   
   Float_t         nSigK[8000];   
   Float_t         nSigP[8000];   
   Float_t         length[8000];
   Float_t         tof[8000];   
   Float_t         beta[8000];   
   

   // from VPD start time if it has been calculated
   Double_t        T0;//added
   Double_t        vzVpd;//added
   Int_t           Iwest;//added
   Int_t           Ieast;//added

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_refMult, *b_refMultNeg, *b_refMultPos;
   TBranch        *b_vertexX;   //!
   TBranch        *b_vertexY;   //!
   TBranch        *b_vertexZ;   //!
   TBranch        *b_vpdEast;   //!
   TBranch        *b_vpdWest;   //!
   TBranch        *b_numberOfVpdEast;   //!
   TBranch        *b_numberOfVpdWest;   //!
   TBranch        *b_tDiff;   //!
   TBranch        *b_tStart;   //!
   TBranch        *b_nTZero;
   TBranch        *b_vpdVz;   //!
   TBranch        *b_vpdLeEast;   //!
   TBranch        *b_vpdLeWest;   //!
   TBranch        *b_vpdTotEast;   //!
   TBranch        *b_vpdTotWest;   //!
   TBranch        *b_vpdBbqAdcWest; 
   TBranch        *b_vpdBbqTdcWest; 
   TBranch        *b_vpdBbqAdcEast; 
   TBranch        *b_vpdBbqTdcEast; 
   TBranch        *b_vpdMxqAdcWest; 
   TBranch        *b_vpdMxqTdcWest; 
   TBranch        *b_vpdMxqAdcEast; 
   TBranch        *b_vpdMxqTdcEast;
   TBranch        *b_nTofHits;   //!
   TBranch        *b_tray;   //!
   TBranch        *b_module;   //!
   TBranch        *b_cell;   //!
   TBranch        *b_leTime;   //!
   TBranch        *b_tot;   //!
   TBranch        *b_yLocal;   //!
   TBranch        *b_zLocal;   //!
   TBranch        *b_thetaLocal;   //!
   TBranch        *b_xGlobal;   //!
   TBranch        *b_yGlobal;   //!
   TBranch        *b_zGlobal;   //!
   TBranch        *b_trackId;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_pX, *b_pY, *b_pZ, *b_p;
   TBranch        *b_dcaX;   //!
   TBranch        *b_dcaY;   //!
   TBranch        *b_dcaZ;   //!
   TBranch        *b_length;   //!
   TBranch        *b_nHits;   //!
   TBranch        *b_nHitsPossible;
   TBranch        *b_nHitsFit;   //!
   TBranch        *b_nHitsDedx;   //!
   TBranch        *b_dedx;   //!
   TBranch        *b_nSigE;   //!
   TBranch        *b_nSigPi;   //!
   TBranch        *b_nSigK;   //!
   TBranch        *b_nSigP;   //!
   TBranch        *b_tof;   //!
   TBranch        *b_beta;   //!



   TBranch        *b_T0;
   TBranch        *b_vzVpd;
   TBranch        *b_Ieast;
   TBranch        *b_Iwest;


   TofPicoDst(TTree *tree=0);

   static const Int_t startWest = 0;
   static const Int_t endWest = 19;
   static const Int_t trgEndWest = 16;

   static const Int_t startEast = 19;
   static const Int_t endEast = 38;
   static const Int_t trgEndEast = 35;

   virtual ~TofPicoDst();
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
   virtual Int_t numTofTracks();
   virtual Double_t trackPt( Int_t iHit );
   virtual Double_t trackP( Int_t iHit );
   virtual Double_t trackEta( Int_t iHit );
   virtual Double_t trackDedx( Int_t iHit );
   virtual Double_t trackPathLength( Int_t iHit );
   virtual Double_t trackTof( Int_t iHit );
   virtual Double_t trackBeta( Int_t iHit );
   virtual Int_t trackCharge( Int_t iHit );


};

#endif

