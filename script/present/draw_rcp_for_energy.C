
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "Reporter.h"
#include "RooPlotLib.h"

void draw_rcp_for_energy( string en, string chargeIn ="p+n", string iCen = "0", string iPer = "6" ){

	Reporter rp( "tmp.pdf", 300, 400 );
	gPad->SetTopMargin( 0.01 );
	//gPad->SetRightMargin( 0.01 );
	RooPlotLib rpl;

	double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
	double per_n_coll = n_coll[ en ][ stoi( iPer ) ];

	TH1 * master;

	int iColor = 0;

	vector<string> charges = { "p", "n" };

	if ( "p" == chargeIn )
		charges = { "p" };
	else if ( "n" == chargeIn )
		charges = { "n" };

	for ( string charge : charges ){
		for ( string plc : {"Pi", "K", "P"} ){

			if ( 0 == iColor )
				master = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
			else 
				draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );
			iColor ++;
		}
	}

	rpl.style(master).set( "xr", 0, 4.0 );
	
	draw_ncoll_uncertainty( 0, cen_n_coll, 30, per_n_coll, 7, true );
	
	rp.saveImage( "img/rcp_" + en + "_" + chargeIn + "_" + iCen + "_" + iPer + ".pdf" );

}