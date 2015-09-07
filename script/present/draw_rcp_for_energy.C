
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "Reporter.h"
#include "RooPlotLib.h"

void draw_rcp_for_energy( string en, string charge, string iCen = "0", string iPer = "6" ){

	Reporter rp( "tmp.pdf", 400, 600 );
	RooPlotLib rpl;

	double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
	double per_n_coll = n_coll[ en ][ stoi( iPer ) ];

	TH1 * master;

	int iColor = 0;
	for ( string plc : {"Pi", "K", "P"} ){

		if ( 0 == iColor )
			master = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
		else 
			draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );
		iColor ++;
	}

	if ( "14.5" == en ){
		if ( "n" == charge )
			rpl.style(master).set( "xr", 0, 3.5 );
		else
			rpl.style(master).set( "xr", 0, 4.0 );
	}

	draw_ncoll_uncertainty( 0, cen_n_coll, 30, per_n_coll, 7, true );

	rp.saveImage( "img/rcp_" + en + "_" + charge + "_" + iCen + "_" + iPer + ".eps" );
	rp.saveImage( "img/rcp_" + en + "_" + charge + "_" + iCen + "_" + iPer + ".pdf" );
	rp.saveImage( "img/rcp_" + en + "_" + charge + "_" + iCen + "_" + iPer + ".png" );

}