
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "draw_single_spectra.C"

void draw_rcp_vs_energy( string plc, string charge, string iCen = "0", string iPer = "6", Reporter * rp = nullptr ){

	RooPlotLib rpl;
	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.72 );

	if ( nullptr == rp )
		rp = new Reporter( "img/single_rcp.pdf", 500, 340 );



	int iColor = 0;
	TH1 *master;
	TLegend * leg = new TLegend( 0.35, 0.15, 0.75, 0.5 );
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

			gPad->SetTopMargin( 0.01 );
			gPad->SetBottomMargin( 0.12 );
			gPad->SetLeftMargin( 0.08 );

		} else 
			h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );


		draw_ncoll_uncertainty( iColor, cen_n_coll, cen_n_coll_unc, per_n_coll, per_n_coll_unc, false );

		leg->AddEntry( h, en.c_str() );

		iColor ++;
	}




	// master->SetTitle( plc_label( plc, charge ).c_str() );
	// master->GetXaxis()->SetTitle( "pT [GeV/c]  " );
	// master->GetXaxis()->SetRangeUser( 0.1, 5.7 );

	rpl.style( master )
		.set( "title", plc_label( plc, charge ) + "; p_{T} [GeV/c]  " )
		.set( "xr", 0.1, 5.7 )
		.set( "yto", 0.5 );

	
	if ( "Pi" == plc && "p" == charge )
		rpl.set( "yr", 0.29, 3.5 );
	if ( "Pi" == plc && "n" == charge )
		rpl.set( "yr", 0.36, 2.4 );

	if ( "K" == plc && "p" == charge )
		rpl.set( "yr", 0.45, 4.5 );
	if ( "K" == plc && "n" == charge )
		rpl.set( "yr", 0.4, 5.0 );

	if ( "P" == plc && "p" == charge )
		rpl.set( "yr", 0.39, 5.0 );
	if ( "P" == plc && "n" == charge )
		rpl.set( "yr", 0.12, 3.0 );
	

	TLine * lUnity = new TLine( 0.1, 1.0, 5.7, 1.0 );
	lUnity->SetLineColor( kBlack );
	lUnity->SetLineWidth( 3 );
	lUnity->SetLineStyle( kDashed );
	lUnity->Draw( "same" );
	

	if ( "P" == plc &&  "n" == charge )
		leg->Draw("same");
	// TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title")); 
	// pt->SetTextSize(0.1); 
	// gPad->Modified();
	// 
	
	if ( nullptr != rp ){
		rp->saveImage( "img/rcp_vs_energy_"+ plc + "_" + charge +".pdf" );
	}

}