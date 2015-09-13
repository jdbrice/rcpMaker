
#include "draw_single_spectra.C"
#include "RooPlotLib.h"


TH1* draw_single_ratio( 	string energy ="14.5", 
							string plc1="P", string charge1="p", 
							string plc2="Pi", string charge2="p", 
							string iCen1 = "0", string iCen2 ="0",
							int color = kRed, string draw_opt = "" ){
	

	RooPlotLib rpl;

	string fn1 = file_name( energy, plc1, charge1, iCen1 );
	string fn2 = file_name( energy, plc2, charge2, iCen2 );

	if ( !file_exists( fn1 ) ){
		ERROR( "File DNE : " << fn1 );
		return new TH1D( "error", "", 1, 0, 1 );
	}
	if ( !file_exists( fn2 ) ){
		ERROR( "File DNE : " << fn2 );
		return new TH1D( "error", "", 1, 0, 1 );
	}


	SpectraLoader sl1( fn1 );
	SpectraLoader sl2( fn2 );


	TH1* sys1 = normalize_binning( sl1.sysHisto( fn1 + energy + "_sys1" ) );
	TH1* stat1 = normalize_binning( sl1.statHisto( fn1 + energy+"_stat1" ) );

	TH1* sys2 = normalize_binning( sl2.sysHisto( fn2 + energy+"_sys2" ) );
	TH1* stat2 = normalize_binning( sl2.statHisto( fn2 + energy+"_stat2" ) );


	TH1* sys = (TH1*)sys1->Clone( (energy+"sys_ratio").c_str() );
	TH1* stat = (TH1*)stat1->Clone( (energy+"stat_ratio").c_str() );

	sys->Divide( sys2 );
	stat->Divide( stat2 );

	rpl.style( sys ).set( "color", color, 0.5 ).set( "fca", color, 0.5 )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", 8 ).set( "draw", draw_opt + " e2" )
		.set( "xr", 0.1, 5.0 ).draw();

	rpl.style( stat ).set( "color", color, 0.5 )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", 8 ).set( "draw", "same" ).draw();

	return sys;

}