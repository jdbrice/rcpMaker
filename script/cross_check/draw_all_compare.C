#include "draw_single_compare.C"


vector<string> plcs = { "Pi", "K", "P" };
vector<string> charges = { "p", "n" };
vector<string> centralities = { "0", "1", "2", "3", "4" };

void draw_all_compare(){
	
	Reporter rp( "rp_comparison.pdf", 1200, 900 );

	for ( string plc : plcs ){
		for ( string c : charges ){
			for ( string iCen : centralities ){

				rp.newPage();
				draw_single_compare( plc, c, iCen );
				rp.savePage();
			}
		}
	}

}