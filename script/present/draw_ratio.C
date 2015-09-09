
#include "draw_single_spectra.C"
#include "RooPlotLib.h"


void draw_ratio( 	string energy, 
					string plc1, string charge1, 
					string plc2, string charge2, 
					string iCen1 = "0", string iCen2 ="0",
					int color = kRed, string draw_opt = "", double lb = 4.5 ){
	

	RooPlotLib rpl;

	string fn1 = file_name( energy, plc1, charge1, iCen1 );
	string fn2 = file_name( energy, plc2, charge2, iCen2 );

	if ( !file_exists( fn1 ) ){
		ERROR( "File DNE : " << fn1 );
		return;
	}
	if ( !file_exists( fn2 ) ){
		ERROR( "File DNE : " << fn2 );
		return;
	}


	SpectraLoader sl1( fn1 );
	SpectraLoader sl2( fn2 );


	TH1* sys1 = normalize_binning( sl1.sysHisto( fn1 + "_sys1" ) );
	TH1* stat1 = normalize_binning( sl1.statHisto( fn1 + "_stat1" ) );

	TH1* sys2 = normalize_binning( sl2.sysHisto( fn2 + "_sys2" ) );
	TH1* stat2 = normalize_binning( sl2.statHisto( fn2 + "_stat2" ) );


	TH1* sys = (TH1*)sys1->Clone( "sys_ratio" );
	TH1* stat = (TH1*)stat1->Clone( "stat_ratio" );

	sys->Divide( sys2 );
	stat->Divide( stat2 );

	rpl.style( sys ).set( "color", color, 0.5 ).set( "fca", color, 0.5 )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", 8 ).set( "draw", draw_opt + " e2" ).draw();

	rpl.style( stat ).set( "color", color, 0.5 )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", 8 ).set( "draw", "same" ).draw();

}