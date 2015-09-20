#include "draw_ratio_vs_centrality.C"


void pannel_ratio_vs_centrality( string charge ="p" ){

	gStyle->SetOptStat(0);

	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.82 );

	Reporter rp( "_tmp.pdf", 900, 700 );
	rp.newPage( 3, 2, 0, 0 );
	rp.cd( 1, 1 );


	
	int index = 0;

	for ( string energy : renergies ){
	
		if ( "39.0" == energy || "14.5" == energy ){
			gPad->SetLeftMargin(0.18);
		}
		if ( "11.5" == energy || "14.5" == energy || "7.7" == energy ){
			gPad->SetBottomMargin(0.2);
		}

		draw_ratio_vs_centrality( energy, "P", "Pi", charge, &rp );
		
	
		if ( "7.7" == energy )
			rp.saveImage( "img/pannel_ratio_" + charge + ".pdf" );

		rp.next();
	}
	
}