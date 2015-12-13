#include "draw_single_spectra.C"
#include "draw_single_ratio.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_ratio_vs_centrality( string en="14.5", string plc1="P", string plc2="Pi", string charge="p", Reporter * rp = nullptr ){
	Logger::setGlobalLogLevel( Logger::llAll );

	if ( nullptr == rp  ){
		 rp = new Reporter( "tmp.pdf", 500, 750 );
	}
	RooPlotLib rpl;

	TH1 * master = new TH1F( ("frame"+en).c_str(), "frame", 500, 0, 10 );
	master->Draw();

	gPad->SetRightMargin(0.01);
	gPad->SetTopMargin(0.01);

	if ( "39.0" == en || "14.5" == en )
		gPad->SetLeftMargin(0.17);
	else 
		gPad->SetLeftMargin(0.1);
	int iColor = 0;

	if ( "n" == charge && !( "39.0" == en || "14.5" == en ))
			gPad->SetLeftMargin(0.13);

	TLegend * leg;
	if ( "p" == charge ) 
		leg = new TLegend( 0.66, gPad->GetBottomMargin(), 0.99, 0.99, "Centrality:" );
	else 
		leg = new TLegend( gPad->GetLeftMargin(), gPad->GetBottomMargin(), 0.99, 0.99, "Centrality:" );
	
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );
	leg-> SetNColumns(1);

	gStyle->SetTitleSize( 0.15, "t" );
	gStyle->SetTitleX( 0.42 );
	gStyle->SetTitleY( 0.95 );
	gStyle->SetTitleAlign( 23 );

	vector<int> markers = { 20, 21, 34, 22, 33, 29, 8 };

	int iiCen = 0;
	for ( string iCen : rcentralities ){
		

		TH1 * h = draw_single_ratio( en, 
			plc1, charge, plc2, charge,
			iCen, iCen, colors[ iColor ], "same pe ", markers[ iColor ] );

		iColor ++;

		leg->AddEntry( h, rcentrality_labels[ iiCen ].c_str() );
		iiCen ++;

	}

	if ( "7.7" == en && "p" == charge){
		leg->Draw();
	} else if ( "14.5" == en && "n" == charge ){
		//TCanvas * cleg = new TCanvas( "cc", "cc", 200, 200 );
		leg->Draw();
		//cleg->Print( "legend_ratio_negative.pdf" ) ;
	}

	rpl.style( master )
		.set( "logy", 0 )
		.set( "title", en + "; p_{T} [GeV/c]   ; " + plc_label( plc1, charge ) + " / " + plc_label( plc2, charge ) + "" )
		.set( "xr", 0.1, 5.8 )
		.set( "yr", 0.08, 4.7 )
		.set( "xts", 0.10 )
		.set( "xto", 0.8 )
		.set( "xls", 0.07 )
		.set( "yts", 0.10 )
		.set( "yto", 0.8 )
		.set( "yls", 0.07 );
	
	if ( "39.0" == en || "14.5" == en )
		rpl.set( "title", en + "; p_{T} [GeV/c]   ; " + plc_label( plc1, charge ) + " / " + plc_label( plc2, charge ) + "" );
	else
		rpl.set( "title", en + "; p_{T} [GeV/c]   ; " );
 
	if ( stof( en ) < 15 ){
		rpl.style( master )
			.set( "yr", 0.15, 20 );
	}

	if ( "7.7" == en)
		rpl.set( "yr", 0.08, 18.7 );
	if ( "11.5" == en)
		rpl.set( "yr", 0.08, 11.7 );
	if ( "14.5" == en)
		rpl.set( "yr", 0.08, 6.7 );
	if ( "19.6" == en)
		rpl.set( "yr", 0.08, 4.7 );
	if ( "27.0" == en)
		rpl.set( "yr", 0.08, 3.7 );
	if ( "39.0" == en)
		rpl.set( "yr", 0.08, 2.7 );
	

	if ( "n" == charge ){

		if( stof( en ) < 15 ){
			rpl.set( "yr", 0.002, 0.95 );
		} else {
			rpl.set( "yr", 0.02, 2 );
		}
	}

	gPad->SetLogy(1);




	rp->saveImage( "img/ratio_" + en + "_" + plc1 + "_over_" + plc2 + "_" + charge + ".pdf" );
	

}