#ifndef FEED_DOWN_MAKER_H
#define FEED_DOWN_MAKER_H

// STL
#include <map>
#include <vector>
using namespace std;

// Roobarb
#include "TreeAnalyzer.h"
#include "RefMultCorrection.h"
using namespace jdb;


#include "RefMultCorrection.h"

class FeedDownMaker : public TreeAnalyzer
{
protected:
	unique_ptr<RefMultCorrection> rmc;
	float corrRefMult;

public:
	FeedDownMaker( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~FeedDownMaker();
	

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


	double rapidity( double pt, double eta, double m ){
		double a = sqrt( m*m + pt*pt*cosh( eta )*cosh( eta ) ) + pt * sinh( eta );
		double b = sqrt( m*m + pt*pt );
		return log( a / b );
	}

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

	void exportParams(string name, int plcIndex, float cl, float ch, TF1 * fn, string formula);


	void background( string, int, int, int );

};

#endif