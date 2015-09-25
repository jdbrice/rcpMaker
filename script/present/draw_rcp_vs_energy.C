
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"
#include "draw_single_spectra.C"

void draw_rcp_vs_energy( string plc, string charge, string iCen = "0", string iPer = "6", Reporter * rp = nullptr ){

	RooPlotLib rpl;
	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.99 );

	if ( nullptr == rp )
		rp = new Reporter( "img/single_rcp.pdf", 450, 300 );


	vector<int> markers = { 20, 21, 34, 22, 33, 29 };
	int iColor = 0;
	
	TLegend * leg = new TLegend( 0.35, 0.15, 0.75, 0.5 );
	leg-> SetNColumns(2);
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );

	TH1 * master = new TH1D( "frame", "frame", 100, 0, 6 );
	master->Draw();

	gPad->SetTopMargin( 0.01 );
	gPad->SetBottomMargin( 0.14 );
	gPad->SetLeftMargin( 0.1 );
	gPad->SetRightMargin( 0.01 );


	bool alone = false;

	vector<string> energySet = nullenergies;
	if ( !alone )energySet = rcpenergies;

	for ( string en : energySet ){

		if ( "" == en ){
			iColor++;
			continue;
		}

		double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
		double per_n_coll = n_coll[ en ][ stoi( iPer ) ];

		double cen_n_coll_unc = n_coll_unc[ en ][ stoi( iCen ) ];
		double per_n_coll_unc = n_coll_unc[ en ][ stoi( iPer ) ];

		TH1 * h = draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, rcpcolors[ iColor ], "same", markers[ iColor ] );


		draw_ncoll_uncertainty( iColor, cen_n_coll, cen_n_coll_unc, per_n_coll, per_n_coll_unc, false, rcpcolors[ iColor ] );

		leg->AddEntry( h, en.c_str() );



		iColor ++;
	}




	// master->SetTitle( plc_label( plc, charge ).c_str() );
	// master->GetXaxis()->SetTitle( "pT [GeV/c]  " );
	// master->GetXaxis()->SetRangeUser( 0.1, 5.7 );

	rpl.style( master )
		.set( "title", plc_label( plc, charge ) + "; p_{T} [GeV/c] ; R_{CP} ( " + centrality_labels[ stoi( iCen ) ] + " ) / ( " + centrality_labels[ stoi( iPer ) ] + " ) " )
		.set( "xr", 0.1, 5.7 )
		.set( "xts", 0.06 )
		.set( "xto", 1.0 )
		.set( "xls", 0.06 )
		.set( "yto", 0.4 )
		.set( "yts", 0.08 )
		.set( "yls", 0.06 );

	
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
	lUnity->SetLineWidth( 4 );
	lUnity->SetLineStyle( kDashed );
	lUnity->Draw( "same" );
	

	if ( "P" == plc &&  "n" == charge && !alone )
		leg->Draw("same");
	// TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title")); 
	// pt->SetTextSize(0.1); 
	// gPad->Modified();
	// 
	
	if ( nullptr != rp ){
		if ( alone )
			rp->saveImage( "img/rcp_vs_energy_"+ plc + "_" + charge +"_solo.pdf" );
		else
			rp->saveImage( "img/rcp_vs_energy_"+ plc + "_" + charge +".pdf" );
	}

}