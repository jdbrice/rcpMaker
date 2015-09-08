#include "pannel_spectra_vs_centrality.C"
#include "common.C"


void draw_all(){

	for ( string en : energies ){
		pannel_spectra_vs_centrality( en );
	}
	
}