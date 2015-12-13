#include "draw_spectra_vs_centrality.C"

void pannel_spectra_vs_centrality( string energy = "14.5" ){
	gStyle->SetTitleSize( 0.25, "t" );
	gStyle->SetTitleX( 0.92 );

	Reporter rp( "_tmp.pdf", 1100, 800 );
	rp.newPage( 3, 2, 0, 0 );

	
	
	
	rp.cd( 1, 1 );

	for ( string charge : { "p", "n" } ){
		for ( string plc : { "Pi", "K", "P" } ){
			
			if ( "Pi" == plc ){
				gPad->SetLeftMargin( 0.19 );
			}
			if ( "n" == charge ){
				gPad->SetBottomMargin( 0.19 );
			}

			draw_spectra_vs_centrality( energy, plc, charge );
			
			if ( plc == "P" && charge == "n" )
				rp.saveImage( "img/pannel_spectra_" + energy + ".pdf" );

			rp.next();
		}
	}

}