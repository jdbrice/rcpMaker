#include "draw_rcp_vs_energy.C"


void draw_rcp_vs_energy_all(){

	for ( string plc :  { "Pi", "K", "P" } ){
		for ( string c : { "p", "n" } ){
			draw_rcp_vs_energy( plc, c );
		}
	}
}