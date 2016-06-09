#include "draw_rcp_vs_energy.C"

void pannel_rcp_vs_energy( ){

	Reporter rp( "_tmp.pdf", 1200, 700 );
	rp.newPage( 3, 2, 0, 0 );
	rp.cd( 1, 1 );
	rp.getCanvas()->SetLeftMargin( 0.15 );
	rp.getCanvas()->SetBottomMargin( 0.45 );
	for ( string charge : { "p", "n" } ){
		for ( string plc : { "Pi", "K", "P" } ){
		
			draw_rcp_vs_energy( plc, charge, "0", "6", &rp );
			
			if ( plc == "P" && charge == "n" ){

				rp.saveImage( "img/pannel_rcp_.pdf" );
			}

			rp.next();
		}
	}



}