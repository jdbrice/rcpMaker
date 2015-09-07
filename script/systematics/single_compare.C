
#include "Logger.h"


string file_name( string source, string plc ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/products/";
	return base + source + "/PostCorr_" + plc +".root";
}


TH1 * yield_hist_for( string source, string plc, string charge, string iCen ){

	string name = plc + "_yield/yield_" + plc + "_" + iCen + "_" + charge;
	TFile * f = new TFile( file_name( source, plc ).c_str(), "READ" );

	TH1 * h = (TH1*) f->Get( name.c_str() );
	h->SetDirectory( 0 );

	return h;
}


void single_compare( string source2, string source1 = "nominal", string plc = "Pi" ){

	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/products/";
	string fn1 = base + source1 + "/PostCorr_" + plc +".root";
	string fn2 = base + source2 + "/PostCorr_" + plc +".root";

	TFile * f1 = new TFile( fn1.c_str(), "READ" );
	TFile * f2 = new TFile( fn2.c_str(), "READ" );

	if ( !f1->IsOpen() || !f2->IsOpen() ){
		ERROR( "Could not open a file" );
		return;
	}

	string name = plc + "_yield/yield_" + plc + "_0_p";
	TH1D * h1 = (TH1D*)f1->Get( name.c_str() );
	TH1D * h2 = (TH1D*)f2->Get( name.c_str() );
	h2->SetLineColor( kRed );

	cout << " I_h1  = " << h1->Integral() << endl;
	cout << " I_h2  = " << h2->Integral() << endl;
	//h2->Scale( h1->Integral() / h2->Integral() );

	TCanvas * c1 = new TCanvas( "c1", "c1" );
	h1->Draw();
	h2->Draw("same");

	TCanvas * c2 = new TCanvas( "c2", "Difference" );
	TH1D * dif = (TH1D*) h1->Clone( "dif" );

	dif->Add( h2, -1 );

	dif->Draw();

	TCanvas * c3 = new TCanvas( "c3", "Ratio" );
	TH1D * ratio = (TH1D*) h1->Clone( "ratio" );

	ratio->Divide( h2 );

	ratio->Draw();

	TCanvas * c4 = new TCanvas( "c4", "Rel" );
	TH1D * rel = (TH1D*) dif->Clone( "rel" );

	rel->Divide( h1 );

	rel->Draw();



	
}