#include "draw_single_spectra.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_spectra_vs_centrality( string en="14.5", string plc="Pi", string charge="p" ){
	Logger::setGlobalLogLevel( Logger::llAll );

	//Reporter rp( "tmp.pdf", 900, 700 );
	RooPlotLib rpl;

	TH1 * master;
	int iColor = 0;
	TLegend * leg = new TLegend( 0.05, 0.05, 0.95, 0.95 );

	for ( string iCen : centralities ){
		INFO( "Drawing " << en << " " << plc << " " << charge << " " << iCen );
		
		TH1 * h;
		if ( 0 == iColor ){
			master = draw_single_spectra( en, plc, charge, iCen, colors[ iColor ], "", pow( 10, -iColor ) );
			h = master;
		} else 
			h = draw_single_spectra( en, plc, charge, iCen, colors[ iColor ], "same", pow( 10, -iColor ) );

			string sc = centrality_labels[iColor] + " x 1";
			if ( iColor > 0 ){
				sc = centrality_labels[iColor] + " x 10^{-" + ts(iColor) + "}";
			}
			leg->AddEntry( h, sc.c_str() );
		iColor ++;
	}

	master->GetYaxis()->SetRangeUser( 1.5e-13, 5e2 );
	rpl.style( master ).set( "yto", 1.1 ).set( "markerstyle", 8 );

	rpl.style( master ).set( "xr", 0, 4.5 );
	
	TCanvas * c = new TCanvas( "c", "c", 400, 300 );
	leg->Draw();
	c->Print( ("img/centrality_legend_" + en +".pdf").c_str() ); 


	//rp.saveImage( "img/spectra_" + en + "_" + plc + "_" + charge + ".pdf" );
	

}