
#include "draw_single_rcp.C"
#include "draw_ncoll_uncertainty.C"

void draw_rcp_for_energy( string en, string charge, string iCen = "0", string iPer = "6" ){

	int iColor = 0;
	for ( string plc : {"Pi", "K", "P"} ){

		double cen_n_coll = n_coll[ en ][ stoi( iCen ) ];
		double per_n_coll = n_coll[ en ][ stoi( iPer ) ];
		
		if ( 0 == iColor )
			draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "" );
		else 
			draw_single_rcp( en, plc, charge, iCen, iPer, cen_n_coll, per_n_coll, colors[ iColor ], "same" );


		draw_ncoll_uncertainty( iColor, cen_n_coll, 30, per_n_coll, 7 );

		iColor ++;
	}

}