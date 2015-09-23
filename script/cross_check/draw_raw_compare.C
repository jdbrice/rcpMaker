#include "SpectraLoader.h"
#include "Utils.h"
#include "Logger.h"


#include <sys/stat.h>

#include "RooPlotLib.h"

 bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string deepti_file_name( string energy, string plc, string charge, string iCen ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/deepti/raw/";
	return base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + iCen + ".dat";
}

int nPtBins = 28;
double ptBins[] = { 
0.0,
0.1,
0.5,
0.6, 
0.7, 
0.8, 
0.9, 
1.0, 
1.1, 
1.2, 
1.3, 
1.4, 
1.5, 
1.6, 
1.7, 
1.8, 
1.9, 
2.0	, 
2.2	, 
2.4	, 
2.6	, 
2.8	,
3.0 , 
3.5, 
4.5, 
5.0,
5.7, 
6.0, 
6.8 };


vector<string> cenLabels = { "0-5%", "5-10%", "10-20%", "20-30%", "30-40%", "40-60%", "60-80%" };
string plc_label( string plc, string charge ){
	if ( "Pi" == plc && "p" == charge )
		return "#pi^{+}";
	if ( "Pi" == plc && "n" == charge )
		return "#pi^{-}";
	if ( "K" == plc && "p" == charge )
		return "K^{+}";
	if ( "K" == plc && "n" == charge )
		return "K^{-}";
	if ( "P" == plc && "p" == charge )
		return "p";
	if ( "P" == plc && "n" == charge )
		return "#bar{p}";
	return "";
}


TH1 * normalize_binning( TH1 * in, int push = 2 ){
	INFO( "Normalizing the bins for " << in->GetName() );
	string name = in->GetName();
	name += + "_normed";
	TH1 * out = new TH1D( name.c_str(), "", nPtBins, ptBins );


	DEBUG( "Input has " << in->GetNbinsX() << " bins" );
	for ( int i = 1; i <= in->GetNbinsX(); i++ ){
		if ( 1 == push && 1 == i )
			continue;
		double y = in->GetBinContent( i );
		double ey = in->GetBinError( i );
		out->SetBinContent( i + push, y );
		out->SetBinError( i + push, sqrt( ey*ey ) );
	}

	DEBUG( "returing normalized hitograms " << name  );
	return out;
}



void draw_raw_compare( string plc="Pi", string charge="p", string iCen="0" ){

	string tag = "RAW COMPARE";

	RooPlotLib rpl;

	string energy = "14.5";

	// INFO( tag, "Loading Daniel's Spectra" );
	// string dafn = file_name( energy, plc, charge, iCen );
	// if ( !file_exists( dafn ) ){
	// 	ERROR( tag, dafn + " DNE" );
	// 	return;
	// }
	// SpectraLoader da( dafn );

	INFO( tag, "Loading Deb's Spectra" );
	string defn = deepti_file_name( energy, plc, charge, iCen );
	if ( !file_exists( defn ) ){
		ERROR( tag, defn + " DNE" );
		return;
	}
	SpectraLoader de( defn );


	TH1 * h = de.statHisto( "deraw" );
	h->Draw();

}