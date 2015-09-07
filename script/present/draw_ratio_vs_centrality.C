#include "draw_single_spectra.C"
#include "draw_ratio.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_ratio_vs_centrality( string en, string plc1, string plc2, string charge ){
	Logger::setGlobalLogLevel( Logger::llAll );

	// Reporter rp( "tmp.pdf", 900, 700 );
	RooPlotLib rpl;

	TH1 * master;
	int iColor = 0;
	for ( string iCen : centralities ){
		
		
		if ( 0 == iColor )
			draw_ratio( en, 
				plc1, charge, plc2, charge,
				iCen, iCen, colors[ iColor ], "" );
		else 
			draw_ratio( en, 
				plc1, charge, plc2, charge,
				iCen, iCen, colors[ iColor ], "same" );

		iColor ++;
	}

	// master->GetYaxis()->SetRangeUser( 10e-14, 1e2 );
	// rpl.style( master ).set( "yto", 1.1 ).set( "markerstyle", 8 );

	// if ( "14.5" == en ){
	// 	rpl.style( master ).set( "xr", 0, 4.0 );
	// 	//if ( "P" == plc && "n" == charge )
						
	// }


	// rp.saveImage( "img/spectra_" + en + "_" + plc + "_" + charge + ".pdf" );
	

}