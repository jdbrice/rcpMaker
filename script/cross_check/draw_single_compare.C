#include "SpectraLoader.h"
#include "Utils.h"
#include "Logger.h"


#include <sys/stat.h>

#include "RooPlotLib.h"

 bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string file_name( string energy, string plc, string charge, string iCen ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/";
	return base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + iCen + ".dat";
}

string deepti_file_name( string energy, string plc, string charge, string iCen ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/deepti/compare/";
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
		out->SetBinContent( i + push, in->GetBinContent( i ) );
		out->SetBinError( i + push, in->GetBinError( i ) );
	}

	DEBUG( "returing normalized hitograms " << name  );
	return out;
}



void draw_single_compare( string plc="K", string charge="p", string iCen="0" ){

	string tag="cross_check";
	Logger::setGlobalLogLevel(Logger::llAll);

	//TCanvas *c = new TCanvas( "c", "c", 800, 800 );

	// Upper pad for the data points +  the fit
	TPad * p1 = new TPad( "spectra", "spectra", 0.001, 0.3, 0.99, 0.99 );
	p1->SetBottomMargin( 0.0 );
	p1->SetLeftMargin( 0.1 );

	// Lower pad with the data / fit ratio
	TPad * p2 = new TPad( "ratio", "ratio", 0.001, 0.01, 0.99, 0.3 );
	p2->SetTopMargin( 0.0 );
	p2->SetLeftMargin( 0.1 );
	p1->Draw();
	p2->Draw();
	p1->cd();

	RooPlotLib rpl;

	string energy = "14.5";
	INFO( tag, "Loading Daniel's Spectra" );
	string dafn = file_name( energy, plc, charge, iCen );
	if ( !file_exists( dafn ) ){
		ERROR( tag, dafn + " DNE" );
		return;
	}
	SpectraLoader da( dafn );

	INFO( tag, "Loading Deb's Spectra" );
	string defn = deepti_file_name( energy, plc, charge, iCen );
	if ( !file_exists( defn ) ){
		ERROR( tag, defn + " DNE" );
		return;
	}
	SpectraLoader de( defn );

	int push = 1;
	if ( plc == "P" )
		push = 2;

	TH1 * hda = normalize_binning( da.combinedErrorHisto("da") );
	TH1 * hde = normalize_binning( de.combinedErrorHisto("de"), push );

	TLegend * leg = new TLegend( 0.2, 0.1, 0.3, 0.3 );
	leg->AddEntry( hda, "Daniel" );
	leg->AddEntry( hde, "Deepti" );

	rpl.style(hda)
		.set( "mst", kCircle )
		.set( "color", kRed )
		.set( "fillstyle", 1001 )
		.set( "logy", 1 )
		.set( "xr", 0.5, 2 )
		.set( "yr", 1e-5, 1e3 )
		.set( "optstat", 0 )
		.set( "title", plc_label( plc, charge ) + " : " + cenLabels[ stoi( iCen ) ] )
		.draw();

	rpl.style(hde)
		.set( "mst", kOpenCircle )
		.set( "color", kBlue )
		.set( "draw", "same" )
		.draw();

	leg->Draw();

	p2->cd();

	TH1 * ratio = (TH1*) hda->Clone("ratio");
	ratio->Divide( hde );

	rpl.style( ratio )
		.set( "logy", 0 )
		.set( "optstat", 0 )
		.set( "xr", 0.5, 2 )
		.set( "yr", 0.5, 1.5 )
		.set( "yls", 0.09 )
		.draw();

	gPad->SetGrid(0, 1);

}