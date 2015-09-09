#include "draw_spectra_vs_centrality.C"

void pannel_spectra_vs_centrality( string energy ){
	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.82 );

	Reporter rp( "_tmp.pdf", 1200, 700 );
	rp.newPage( 3, 2, 0, 0 );
	rp.cd( 1, 1 );

	for ( string charge : { "p", "n" } ){
		for ( string plc : { "Pi", "K", "P" } ){
		
			draw_spectra_vs_centrality( energy, plc, charge );
			
			if ( plc == "P" && charge == "n" )
				rp.saveImage( "img/pannel_spectra_" + energy + ".pdf" );

			rp.next();
		}
	}

}