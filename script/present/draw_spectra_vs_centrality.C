#include "draw_single_spectra.C"
#include "RooPlotLib.h"



void draw_spectra_vs_centrality( string en, string plc, string charge ){
	Logger::setGlobalLogLevel( Logger::llAll );

	RooPlotLib rpl;

	TH1 * master;
	int iColor = 0;
	for ( string iCen : centralities ){
		INFO( "Drawing " << en << " " << plc << " " << charge << " " << iCen );
		
		if ( 0 == iColor )
			master = draw_single_spectra( en, plc, charge, iCen, colors[ iColor ], "", pow( 10, -iColor ) );
		else 
			draw_single_spectra( en, plc, charge, iCen, colors[ iColor ], "same", pow( 10, -iColor ) );

		iColor ++;
	}

	master->GetYaxis()->SetRangeUser( 10e-14, 1e2 );
	rpl.style( master ).set( "yto", 1.1 );

}