
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "draw_single_spectra.C"

void draw_rcp_vs_energy( string plc, string charge, string iCen = "0", string iPer = "6" ){

	RooPlotLib rpl;
	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.82 );
	int iColor = 0;
	TH1 *master;
	TLegend * leg = new TLegend( 0.4, 0.1, 0.8, 0.5 );
	leg-> SetNColumns(2);
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );
	for ( string en : energies ){

		double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
		double per_n_coll = n_coll[ en ][ stoi( iPer ) ];

		double cen_n_coll_unc = n_coll_unc[ en ][ stoi( iCen ) ];
		double per_n_coll_unc = n_coll_unc[ en ][ stoi( iPer ) ];

		TH1 * h;
		if ( 0 == iColor ){
			master = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
			h = master;
		} else 
			h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );


		draw_ncoll_uncertainty( iColor, cen_n_coll, cen_n_coll_unc, per_n_coll, per_n_coll_unc, false );

		leg->AddEntry( h, en.c_str() );

		iColor ++;
	}



	master->SetTitle( plc_label( plc, charge ).c_str() );
	master->GetXaxis()->SetTitle( "pT [GeV/c]  " );
	master->GetXaxis()->SetRangeUser( 0.1, 5.7 );
	

	if ( "P" == plc &&  "p" == charge )
		leg->Draw("same");
	// TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title")); 
	// pt->SetTextSize(0.1); 
	// gPad->Modified();

}