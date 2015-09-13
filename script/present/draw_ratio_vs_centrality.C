#include "draw_single_spectra.C"
#include "draw_single_ratio.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_ratio_vs_centrality( string en="14.5", string plc1="P", string plc2="Pi", string charge="p", Reporter * rp = nullptr ){
	Logger::setGlobalLogLevel( Logger::llAll );

	if ( nullptr == rp  ){
		 rp = new Reporter( "tmp.pdf", 900, 700 );
	}
	RooPlotLib rpl;

	TH1 * master = new TH1F( ("frame"+en).c_str(), "frame", 500, 0, 5 );
	master->Draw();
	int iColor = 0;

	TLegend * leg = new TLegend( 0.3, 0.15, 0.95, 0.6 );
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );
	leg-> SetNColumns(2);

	gStyle->SetTitleSize( 0.15, "t" );
	gStyle->SetTitleX( 0.3 );
	gStyle->SetTitleAlign( 23 );

	int iiCen = 0;
	for ( string iCen : rcentralities ){
		
		TH1 * h = draw_single_ratio( en, 
			plc1, charge, plc2, charge,
			iCen, iCen, colors[ iColor ], "same" );

		iColor ++;

		leg->AddEntry( h, rcentrality_labels[ iiCen ].c_str() );
		iiCen ++;

	}

	if ( "7.7" == en ){
		leg->Draw();
	}

	if ( "7.7" == en )
		en = "7.7   ";

	rpl.style( master )
		.set( "logy", 1 )
		.set( "title", en + "; p_{T} [GeV/c]   ; " + plc_label( plc1, charge ) + " / " + plc_label( plc2, charge ) + "   " )
		.set( "xr", 0.1, 4.8 )
		.set( "yr", 0.08, 6 )
		.set( "xts", 0.10 )
		.set( "xto", 0.6 )
		.set( "xls", 0.06 )
		.set( "yts", 0.10 )
		.set( "yto", 0.5 );
	

	if ( stof( en ) < 15 ){
		rpl.style( master )
			.set( "yr", 0.08, 20 );
	}

	rp->saveImage( "img/ratio_" + en + "_" + plc1 + "_over_" + plc2 + "_" + charge + ".pdf" );
	

}