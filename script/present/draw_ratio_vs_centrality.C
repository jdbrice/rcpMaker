#include "draw_single_spectra.C"
#include "draw_ratio.C"
#include "RooPlotLib.h"
#include "Reporter.h"


void draw_ratio_vs_centrality( string en="14.5", string plc1="P", string plc2="Pi", string charge="p" ){
	Logger::setGlobalLogLevel( Logger::llAll );

	// Reporter rp( "tmp.pdf", 900, 700 );
	RooPlotLib rpl;

	TH1 * master;
	int iColor = 0;

	TLegend * leg = new TLegend( 0.65, 0.1, 1.0, 0.8 );
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.07 );

	gStyle->SetTitleSize( 0.15, "t" );
	gStyle->SetTitleX( 0.26 );

	int iiCen = 0;
	for ( string iCen : rcentralities ){
		
		TH1 * h;
		if ( 0 == iColor ){
			master = draw_ratio( en, 
				plc1, charge, plc2, charge,
				iCen, iCen, colors[ iColor ], "" );
			h = master;
		}
		else 
			h = draw_ratio( en, 
				plc1, charge, plc2, charge,
				iCen, iCen, colors[ iColor ], "same" );

		iColor ++;

		cout << "Energy = " << en << endl;
		leg->AddEntry( h, rcentrality_labels[ iiCen ].c_str() );
		iiCen ++;

	}

	if ( "7.7" == en ){
		leg->Draw();
	}

	rpl.style( master ).set( "title", en );
	

	// master->GetYaxis()->SetRangeUser( 10e-14, 1e2 );
	// rpl.style( master ).set( "yto", 1.1 ).set( "markerstyle", 8 );

	// if ( "14.5" == en ){
	// 	rpl.style( master ).set( "xr", 0, 4.0 );
	// 	//if ( "P" == plc && "n" == charge )
						
	// }


	// rp.saveImage( "img/spectra_" + en + "_" + plc + "_" + charge + ".pdf" );
	

}