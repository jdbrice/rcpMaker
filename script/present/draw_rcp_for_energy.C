
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

	double cen_n_coll_unc = n_coll_unc[ en ][ stoi( iCen ) ];
	double per_n_coll_unc = n_coll_unc[ en ][ stoi( iPer ) ];

	TH1 * master;

	int iColor = 0;

	TLegend * leg = new TLegend( 0.6, 0.1, 0.9, 0.4 );
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );


	vector<string> charges = { "p", "n" };

	if ( "p" == chargeIn )
		charges = { "p" };
	else if ( "n" == chargeIn )
		charges = { "n" };

	for ( string charge : charges ){
		for ( string plc : {"Pi", "K", "P"} ){

			TH1 *h;
			if ( 0 == iColor ){
				master = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
				h = master;
			} else 
				h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );
			iColor ++;

			leg->AddEntry( h, plc_label( plc, charge ).c_str(), "LPFE" );
		}
	}

	rpl.style(master).set( "xr", 0, 4.0 ).set( "yr", 0.2, 4 );
	
	draw_ncoll_uncertainty( 0, cen_n_coll, cen_n_coll_unc, per_n_coll, per_n_coll_unc, true, 13 );
	
	leg->Draw();

	rp.saveImage( "img/rcp_" + en + "_" + chargeIn + "_" + iCen + "_" + iPer + ".pdf" );

}