
#include "draw_single_spectra.C"

void draw_spectra_vs_energy( string plc, string charge, string cen ){

	int iColor = 0;
	for ( string en : energies ){

		if ( 0 == iColor )
			draw_single_spectra( en, plc, charge, cen, colors[ iColor ], "" );
		else 
			draw_single_spectra( en, plc, charge, cen, colors[ iColor ], "same" );

		iColor ++;
	}

}