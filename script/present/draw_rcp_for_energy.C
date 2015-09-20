
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "Reporter.h"
#include "RooPlotLib.h"

void draw_rcp_for_energy( string en, string chargeIn ="p+n", string iCen = "0", string iPer = "6" ){

	Reporter rp( "tmp.pdf", 400, 700 );
	gPad->SetTopMargin( 0.01 );
	gPad->SetLeftMargin( 0.15 );
	gPad->SetBottomMargin( 0.15 );
	//gPad->SetRightMargin( 0.01 );
	RooPlotLib rpl;

	double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
	double per_n_coll = n_coll[ en ][ stoi( iPer ) ];

	double cen_n_coll_unc = n_coll_unc[ en ][ stoi( iCen ) ];
	double per_n_coll_unc = n_coll_unc[ en ][ stoi( iPer ) ];

	int iColor = 0;

	TLegend * leg = new TLegend( 0.6, 0.2, 0.8, 0.5 );
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );


	vector<string> charges = { "p", "n" };

	if ( "p" == chargeIn )
		charges = { "p" };
	else if ( "n" == chargeIn )
		charges = { "n" };


	TH1* master = new TH1F( "frame", "frame", 500, 0, 10 );
	master->Draw();


	for ( string charge : charges ){
		for ( string plc : {"Pi", "K", "P"} ){

			TH1 *h;
			// if ( 0 == iColor ){
			// 	h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
			// } else 
			h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );
			iColor ++;

			leg->AddEntry( h, plc_label( plc, charge ).c_str(), "LPFE" );
		}
	}

	double xmax = 3.5;
	double ymin = 0.2;



	rpl.style(master)
		.set( "title", " ;p_{T} [GeV/c]; R_{CP} ( " + centrality_labels[ stoi( iCen ) ] + " ) / ( " + centrality_labels[ stoi( iPer ) ] + " )    " )
		.set( "yts", 0.05 )
		.set( "yls", 0.06 )
		.set( "xts", 0.05 )
		.set( "xls", 0.05 )
		.set( "xr", 0.0, 3.2 )
		.set( "yr", ymin, 4 );
	
	draw_ncoll_uncertainty( 0, cen_n_coll, cen_n_coll_unc, per_n_coll, per_n_coll_unc, true, 13 );
	
	leg->Draw();

	rp.saveImage( "img/rcp_" + en + "_" + chargeIn + "_" + iCen + "_" + iPer + ".pdf" );

}