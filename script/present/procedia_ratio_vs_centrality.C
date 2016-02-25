#include "draw_single_spectra.C"
#include "draw_single_ratio.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void procedia_ratio_vs_centrality( string en="14.5", string plc1="P", string plc2="Pi", string charge="p", Reporter * rp = nullptr ){
	Logger::setGlobalLogLevel( Logger::llAll );

	
	rp = new Reporter( "tmp.pdf", 500, 750 );
	RooPlotLib rpl;


	TH1 * master = new TH1F( ("frame"+en).c_str(), "frame", 500, 0, 10 );
	master->Draw();


	gPad->SetRightMargin(0.01);
	gPad->SetLeftMargin(0.14);
	gPad->SetTopMargin(0.01);
	gPad->SetBottomMargin(0.12);


	int iColor = 0;


	TLegend * leg;
	leg = new TLegend( 0.66, gPad->GetBottomMargin(), 0.99, 0.85, "Centrality:" );

	
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );
	leg->SetNColumns(1);

	gStyle->SetTitleSize( 0.1, "t" );
	gStyle->SetTitleX( 0.35 );
	gStyle->SetTitleY( 0.98 );
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

	leg->Draw();

	rpl.style( master )
		.set( "logy", 0 )
		.set( "title", en + "; p_{T} [GeV/c]   ; " + plc_label( plc1, charge ) + " / " + plc_label( plc2, charge ) + "" )
		.set( "xr", 0.1, 5.8 )
		.set( "yr", 0.08, 4.7 )
		.set( "xts", 0.06 )
		.set( "xto", 0.8 )
		.set( "xls", 0.05 )
		.set( "yts", 0.07 )
		.set( "yto", 0.7 )
		.set( "yls", 0.05 );
	
	
	rpl.set( "title", "Au+Au #sqrt{s_{NN}}=" + en + " GeV; p_{T} [GeV/c]   ; " + plc_label( plc1, charge ) + " / " + plc_label( plc2, charge ) + "" );
	
	
	rpl.set( "yr", 0.08, 6.7 );


	gPad->SetLogy(0);




	rp->saveImage( "img/procedia_ratio_" + en + "_" + plc1 + "_over_" + plc2 + "_" + charge + ".pdf" );
	

}