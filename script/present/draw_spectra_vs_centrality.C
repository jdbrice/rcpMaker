#include "draw_single_spectra.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_spectra_vs_centrality( string en="14.5", string plc="Pi", string charge="p" ){
	Logger::setGlobalLogLevel( Logger::llTrace );


	//Reporter rp( "tmp.pdf", 900, 700 );
	RooPlotLib rpl;


	gStyle->SetLabelSize( 0.075,"x" );
	gStyle->SetLabelSize( 0.05,"y" );

	TH1 * master = new TH1F( "frame", "frame", 1000, 0, 10 );
	master->Draw();


	int iColor = 0;
	TLegend * leg = new TLegend( 0.05, 0.05, 0.95, 0.95 );
	int tester = 0;

	for ( string iCen : centralities ){
		INFO( "Drawing " << en << " " << plc << " " << charge << " " << iCen );
		
		TH1 * h = draw_single_spectra( en, plc, charge, iCen, colors[ iColor ], "same", pow( 10, -iColor ) );
		string sc = centrality_labels[iColor] + " x 1";
		if ( iColor > 0 ){
			sc = centrality_labels[iColor] + " x 10^{-" + ts(iColor) + "}";
		}
		leg->AddEntry( h, sc.c_str() );
		iColor ++;
	}
	
	gPad->SetLogy( 1 );
	master->GetYaxis()->SetRangeUser( 1.5e-13, 5e2 );
	rpl.style( master )
		.set( "title", plc_label( plc, charge ) + " ; p_{T} [GeV/c]   ;     " ) //dN^{2} / ( N_{evt} 2 #pi pT dpT dy )
		.set( "xr", 0.2, 3.8 )
		.set( "yto", 1.35 )
		.set( "yls", 0.07 )
		.set( "yticks", 7, 10, 0, 0 )
		.set( "yts", 0.065 )
		.set( "xto", 0.9 )
		.set( "xts", 0.08 )
		.set( "markerstyle", 8 );


	TCanvas * c = new TCanvas( "c", "c", 400, 500 );
	leg->Draw();
	c->Print( ("img/centrality_legend_" + en +".pdf").c_str() ); 


	//rp.saveImage( "img/spectra_" + en + "_" + plc + "_" + charge + ".pdf" );
	

}