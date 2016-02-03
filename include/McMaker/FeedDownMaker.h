#ifndef FEED_DOWN_MAKER_H
#define FEED_DOWN_MAKER_H

// STL
#include <map>
#include <vector>
using namespace std;

// Roobarb
#include "TreeAnalyzer.h"
#include "HistoBins.h"
#include "ConfigRange.h"
using namespace jdb;


class FeedDownMaker : public TreeAnalyzer
{
protected:
	//unique_ptr<RefMultCorrection> rmc;
	float corrRefMult;


	// Track cuts loaded from config
	
	// nHitsFit
	unique_ptr<ConfigRange> cut_nHitsFit;
	// Distance to closest approach [cm]
	unique_ptr<ConfigRange> cut_dca;
	// nHitsFit / nHitsPossible
	unique_ptr<ConfigRange> cut_nHitsFitOverPossible;
	// nHitDedx
	unique_ptr<ConfigRange> cut_nHitsDedx;
	// Minimum pT [GeV/c]
	unique_ptr<ConfigRange> cut_pt;
	// ptGlobal / ptPrimary
	unique_ptr<ConfigRange> cut_ptGlobalOverPrimary;
	// Rapidity based on mass assumption
	unique_ptr<ConfigRange> cut_rapidity;

	vector<string> formulas;
	unique_ptr<HistoBins> rmb;

	// Centrality bins
	// maps bins from bin9 RMC into some other binning
	// used to combine bins
	map<int, int> centralityBinMap;
	// the vector of bins in the mapped space - makes it easy to loop over
	vector<int> centralityBins;

	int cBin;

public:
	virtual const char* classname() const { return "FeedDownMaker"; }
	~FeedDownMaker();
	
	virtual void initialize(  );


protected:

	map<int, string> plcName; 
	static vector<int> plcID;
	static vector<float> plcMass;

	/**
	 * Before the event loop starts - for subclass init
	 */
	virtual void preEventLoop();

	/**
	 * After the event loop starts - for subclass reporting
	 */
	virtual void postEventLoop();

	/**
	 * Analyze an Event
	 */
	virtual void analyzeEvent( );

	/**
	 * Analyze a track in the current Event
	 * @param	iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );

	/**
	 * Performs event based cuts
	 * @return 	True 	- Keep Event 
	 *          False 	- Reject Event
	 */
	virtual bool keepEvent();

	/**
	 * Performs track based cuts
	 * @return 	True 	- Keep Track 
	 *          False 	- Reject Track
	 */
	virtual bool keepTrack( Int_t iTrack );

	void addGEANTLabels( TH1* h ){
		TAxis * x = h->GetXaxis();


		x->SetBinLabel( 1, "No ID" );
	    x->SetBinLabel( 2, "#gamma");
	    x->SetBinLabel( 3, "e^{+}" );
	    x->SetBinLabel( 4, "e^{-}" );
	    x->SetBinLabel( 5, "#nu" );
	    x->SetBinLabel( 6, "#mu^{+}" );
	    x->SetBinLabel( 7, "#mu^{-}" );
	    x->SetBinLabel( 8, "#pi" );
	    x->SetBinLabel( 9, "#pi^{+}" );
	    x->SetBinLabel( 10, "#pi^{-}" );
	    x->SetBinLabel( 11, "K^{0}_{L}" );
	    x->SetBinLabel( 12, "K^{+}" );
	    x->SetBinLabel( 13, "K^{-}" );
	    x->SetBinLabel( 14, "n" );
	    x->SetBinLabel( 15, "P" );
	    x->SetBinLabel( 16, "#bar{P}" );
	    x->SetBinLabel( 17, "K^{0}_{S}" );
	    x->SetBinLabel( 18, "#eta" );
	    x->SetBinLabel( 19, "#Lambda" );
	    x->SetBinLabel( 20, "#Sigma^{+}" );
	    x->SetBinLabel( 21, "#Sigma^{0}" );
	    x->SetBinLabel( 22, "#Sigma^{-}" );
	    x->SetBinLabel( 23, "#Xi^{0}" );
	    x->SetBinLabel( 24, "#Xi^{-}" );
	    x->SetBinLabel( 25, "#Omega^{-}" );
	    x->SetBinLabel( 26, "#bar{n}" );
	    x->SetBinLabel( 27, "#bar{#Lambda}" );
	    x->SetBinLabel( 28, "#bar{#Sigma^{-}}" );
	    x->SetBinLabel( 29, "#bar{#Sigma^{0}}" );
	    x->SetBinLabel( 30, "#bar{#Sigma^{+}}" );
	    x->SetBinLabel( 31, "#bar{#Xi^{0}}" );
	    x->SetBinLabel( 32, "#bar{#Xi^{+}}" );
	    x->SetBinLabel( 33, "#bar{#Omega^{+}}" );
	}

	void exportParams(int bin, TF1 * fn, ofstream &out );
	void background( string, int, int, ofstream&);

};

#endif