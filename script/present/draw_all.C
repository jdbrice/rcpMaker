#include "pannel_spectra_vs_centrality.C"
#include "pannel_rcp_vs_energy.C"
#include "draw_rcp_for_energy.C"
#include "draw_double_ratio_vs_energy.C"
#include "common.C"


void draw_all(){

	draw_rcp_for_energy("14.5", "p");
	draw_rcp_for_energy("14.5", "n");
	
	for ( string en : energies ){
		pannel_spectra_vs_centrality( en );
	}
	
	pannel_rcp_vs_energy();

	draw_double_ratio_vs_energy("p");
	draw_double_ratio_vs_energy("n");

	

}