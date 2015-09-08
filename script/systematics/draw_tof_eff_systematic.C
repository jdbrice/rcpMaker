#include "single_compare.C"
#include "tof_eff_systematic.C"

void draw_tof_eff_systematic( string plc, string charge, string iCen ){

	TH1 *h = yield_hist_for( "nominal", plc, charge, iCen );

	TH1 *hU = (TH1D*)h->Clone( "sys_unc" ); 

	vector<double> sys_unc = tof_eff_systematic( plc, charge, iCen );

	for ( int i = 0; i < sys_unc.size(); i++ ){
		hU->SetBinContent( i + 1, sys_unc[ i ] );
		hU->SetBinError( i + 1, 0 );
	}

	hU->Draw();
}