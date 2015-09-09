#include "draw_ratio_vs_centrality.C"


void pannel_ratio_vs_centrality( string charge ){

	gStyle->SetOptStat(0);

	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.82 );

	Reporter rp( "_tmp.pdf", 900, 700 );
	rp.newPage( 3, 2, 0, 0 );
	rp.cd( 1, 1 );


	
	for ( string energy : energies ){
	
		draw_ratio_vs_centrality( energy, "P", "Pi", charge );
		
		if ( "39.0" == energy )
			rp.saveImage( "img/pannel_ratio_" + charge + ".pdf" );

		rp.next();
	}
	
}