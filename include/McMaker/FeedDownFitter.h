#ifndef FEED_DOWN_FITTER_H
#define FEED_DOWN_FITTER_H

#include "Common.h"

// STL
#include <string>
#include <map>
#include <vector>
using namespace std;

// Roobarb
#include "HistoBook.h"
#include "HistoBins.h"
#include "XmlConfig.h"
#include "Reporter.h"
using namespace jdb;

#include "TFitResultPtr.h"
#include "TF1.h"

class FeedDownFitter
{
protected:
	XmlConfig * cfg;
	string nodePath;
	string outputPath;

	unique_ptr<HistoBook> book;
	unique_ptr<Reporter> reporter;

	vector<string> formulas;
	unique_ptr<HistoBins> rmb;

	// Centrality bins
	// maps bins from bin9 RMC into some other binning
	// used to combine bins
	map<int, int> centralityBinMap;
	// the vector of bins in the mapped space - makes it easy to loop over
	vector<int> centralityBins;

	map<int, string> plcName; 
	static vector<int> plcID;
	static vector<float> plcMass;

public:

	static constexpr auto tag = "FeedDownFitter";
	FeedDownFitter( XmlConfig * cfg, string nodePath );
	~FeedDownFitter();
	


	void make();

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

	void exportParams(int bin, TF1 * fn, TFitResultPtr res,  ofstream &out );
	void background( string, int, int, ofstream&);
};

#endif