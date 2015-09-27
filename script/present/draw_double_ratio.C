

#include "draw_single_spectra.C"
#include "RooPlotLib.h"

TH1 *draw_double_ratio( 	string energy, 
							string plc1="P", string plc2="Pi", 
							string charge = "p", 
							string iCen1="0", string iCen2="6",
							int color = kRed, 
							string draw_opt = "",
							double scaler = 1.0,
							Reporter *rp = nullptr,
							int marker = 8 ){

	RooPlotLib rpl;

	string fn1 = file_name( energy, plc1, charge, iCen1 );
	string fn2 = file_name( energy, plc2, charge, iCen1 );
	string fn3 = file_name( energy, plc1, charge, iCen2 );
	string fn4 = file_name( energy, plc2, charge, iCen2 );

	if ( !file_exists( fn1 ) ){
		ERROR( "File DNE : " << fn1 );
		return NULL;
	}
	if ( !file_exists( fn2 ) ){
		ERROR( "File DNE : " << fn2 );
		return NULL;
	}
	if ( !file_exists( fn3 ) ){
		ERROR( "File DNE : " << fn3 );
		return NULL;
	}
	if ( !file_exists( fn4 ) ){
		ERROR( "File DNE : " << fn4 );
		return NULL;
	}


	SpectraLoader sl1( fn1 );
	SpectraLoader sl2( fn2 );
	SpectraLoader sl3( fn3 );
	SpectraLoader sl4( fn4 );


	TH1* sys1 = normalize_binning( sl1.sysHisto( fn1 + "_sys1" ) );
	TH1* stat1 = normalize_binning( sl1.statHisto( fn1 + "_stat1" ) );

	TH1* sys2 = normalize_binning( sl2.sysHisto( fn2 + "_sys2" ) );
	TH1* stat2 = normalize_binning( sl2.statHisto( fn2 + "_stat2" ) );

	TH1* sys3 = normalize_binning( sl3.sysHisto( fn3 + "_sys3" ) );
	TH1* stat3 = normalize_binning( sl3.statHisto( fn3 + "_stat3" ) );

	TH1* sys4 = normalize_binning( sl4.sysHisto( fn4 + "_sys4" ) );
	TH1* stat4 = normalize_binning( sl4.statHisto( fn4 + "_stat4" ) );


	TH1* sys_num = (TH1*)sys1->Clone( "sys_ratio_num" );
	TH1* stat_num = (TH1*)stat1->Clone( "stat_ratio_num" );

	TH1* sys_den = (TH1*)sys3->Clone( "sys_ratio_den" );
	TH1* stat_den = (TH1*)stat3->Clone( "stat_ratio_den" );

	sys_num->Divide( sys2 );
	stat_num->Divide( stat2 );

	sys_den->Divide( sys4 );
	stat_den->Divide( stat4 );

	TH1* sys = (TH1*)sys_num->Clone( "sys_ratio" );
	TH1* stat = (TH1*)stat_num->Clone( "stat_ratio" );

	sys->Divide( sys_den );
	stat->Divide( stat_den );


	sys->Scale( scaler );
	stat->Scale( scaler );

	

	rpl.style( sys ).set( "color", color, 0.33 )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", 8 ).set( "draw", draw_opt + " e2" )
		.set( "markerstyle", marker )
		.draw();

	rpl.style( stat ).set( "color", color )
		.set( "title", " ; pT [GeV/c]; " + plc1 + " / " + plc2 )
		.set( "markerstyle", marker )
		.set( "markersize", 2 )
		.set( "draw", "same" ).draw();

		gPad->SetLogy(1);

	TLine * lUnity = new TLine( 0, scaler, 5.0, scaler );

	rpl.style( lUnity ).set( "color", color ).set( "lw", 2 );
	lUnity->SetLineColor( color );
	lUnity->SetLineWidth( 6 );
	lUnity->Draw();
	lUnity->SetLineStyle( kDashed );

	if ( nullptr != rp )
		rp->saveImage( "img/double_ratio_" + energy + "_" + iCen1+"_over_" + iCen2 +"_"+ plc1+"_over_"+plc2+"_"+charge+".pdf" );

	return sys;
}