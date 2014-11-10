#ifndef ANA_PICO_DST_H
#define ANA_PICO_DST_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>

#include "PicoDataStore.h"

class AnaPicoDst : public PicoDataStore {
public :
	TTree          *fChain;   //pointer to the analyzed TTree or TChain
	Int_t           fCurrent; //current Tree number in a TChain

	// Declaration of leave types
	Int_t           run;
	Int_t           evt;
	Int_t           refMult;

	Float_t         vertexX;
	Float_t         vertexY;
	Float_t         vertexZ;
	Int_t           nTZero;
	Float_t         vpdVz;
	
	Int_t           nTofHits;
	Float_t         yLocal[8000];   
	Float_t         zLocal[8000];   
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
	Float_t         length[8000];
	Float_t         tof[8000];   
	Float_t         beta[8000];   
	

	// from VPD start time if it has been calculated
	Double_t        T0;//added
	Double_t        vzVpd;//added
	
	// List of branches
	TBranch        *b_nTriggers;   
	TBranch        *b_triggerIds;   
	TBranch        *b_run;   
	TBranch        *b_evt;   
	TBranch        *b_refMult;
	TBranch        *b_vertexX;   
	TBranch        *b_vertexY;   
	TBranch        *b_vertexZ;   
	TBranch        *b_nTZero;
	TBranch        *b_vpdVz;   
	TBranch        *b_nTofHits;   
	TBranch        *b_yLocal;   
	TBranch        *b_zLocal;   
	TBranch        *b_charge;   
	TBranch        *b_pt;   
	TBranch        *b_eta;   
	TBranch        *b_phi;   
	TBranch        *b_pX, *b_pY, *b_pZ, *b_p;
	TBranch        *b_dcaX;   
	TBranch        *b_dcaY;   
	TBranch        *b_dcaZ;   
	TBranch        *b_length;   
	TBranch        *b_nHits;   
	TBranch        *b_nHitsPossible;
	TBranch        *b_nHitsFit;   
	TBranch        *b_nHitsDedx;   
	TBranch        *b_dedx;   
	TBranch        *b_tof;   
	TBranch        *b_beta;   



	TBranch        *b_T0;
	TBranch        *b_vzVpd;
	

	AnaPicoDst(TTree *tree=0);

	virtual ~AnaPicoDst();
	virtual Int_t    Cut(Long64_t entry);
	virtual Int_t    GetEntry(Long64_t entry);
	virtual Long64_t LoadTree(Long64_t entry);
	virtual void     Init(TTree *tree);
	//virtual void     Loop();
	virtual Bool_t   Notify();
	virtual void     Show(Long64_t entry = -1);




	virtual TTree * getTree() { return fChain; }
	
	virtual Double_t trackPt( Int_t iHit )    { return pt[ iHit ]; }
	virtual Double_t trackP( Int_t iHit )     { return p[ iHit ]; }
	virtual Double_t trackEta( Int_t iHit )   { return eta[ iHit ]; }
	virtual Int_t trackCharge( Int_t iHit )   { return charge[ iHit ]; }

	virtual Double_t trackDedx( Int_t iHit )  { return dedx[ iHit ]; }
	virtual Double_t trackBeta( Int_t iHit )  { return beta[ iHit ]; }
	virtual Int_t trackTofMatch( Int_t iHit ) { return 1;}
	

	virtual Double_t eventVertexX( )          { return vertexX; }
	virtual Double_t eventVertexY( )          { return vertexY; }
	virtual Double_t eventVertexZ( )          { return vertexZ; }

	virtual Double_t trackDcaX( Int_t iHit )  { return dcaX[iHit]; }
	virtual Double_t trackDcaY( Int_t iHit )  { return dcaY[iHit]; }
	virtual Double_t trackDcaZ( Int_t iHit )  { return dcaZ[iHit]; }
	virtual Double_t trackDca( Int_t iHit )   { return TMath::Sqrt(dcaX[iHit]*dcaX[iHit]+dcaY[iHit]*dcaY[iHit]+dcaZ[iHit]*dcaZ[iHit]) ; }

	virtual Double_t trackNHits( Int_t iHit ) { return nHits[ iHit ]; }
	virtual Double_t trackNHitsDedx( Int_t iHit ){return nHitsDedx[ iHit ]; }
	virtual Double_t trackNHitsFit( Int_t iHit ){ return nHitsFit[ iHit ]; }
	virtual Double_t trackNHitsPossible( Int_t iHit ) { return nHitsPossible[ iHit ]; }

	virtual UInt_t eventRunId(){return run;}

	virtual Short_t eventTofMult()      {return nTofHits; }
	virtual UShort_t eventRefMult()     {return refMult;}
	virtual Int_t eventNumTracks()      {return nTofHits;}


};

#endif

