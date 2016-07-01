#include "draw_single_spectra.C"

string lokesh_file_name( string energy, string plc, string charge, string iCen ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/lokesh/";

	// for easy comparison
	// if ( "14.5" == energy )
	// 	base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/deepti/compare/";
		// base ="/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/deepti_wo_fdc/";

	return base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + iCen + ".dat";

}

void ratio_w_lokesh(		string energy ="7.7", 
							string plc="Pi", string charge="p", 
							string iCen = "0",
							int color = kRed, 
							string draw_opt = "", 
							int ms =8 ){
	

	RooPlotLib rpl;
	Logger::setGlobalLogLevel( "all" );

	string fn1 = file_name( energy, plc, charge, iCen );
	string fn2 = lokesh_file_name( energy, plc, charge, iCen );

	if ( !file_exists( fn1 ) ){
		ERROR( "File DNE : " << fn1 );
		return new TH1D( "error", "", 1, 0, 1 );
	}
	if ( !file_exists( fn2 ) ){
		ERROR( "File DNE : " << fn2 );
		return new TH1D( "error", "", 1, 0, 1 );
	}


	SpectraLoader sl1( fn1 );
	SpectraLoader sl2( fn2, true );



	// TH1* sys1 = normalize_binning( sl1.sysHisto( fn1 + energy + "_sys1" ) );
	TH1* stat1 = normalize_binning( sl1.statHisto( fn1 + energy+"_stat1" ) );

	// TH1* sys2 = normalize_binning( sl2.sysHisto( fn2 + energy+"_sys2" ) );
	TH1* stat2 = normalize_binning( sl2.statHisto( fn2 + energy+"_stat2" ) );

	cout << "Err1 " << stat1->GetBinError(4) << endl;
	cout << "Err2 " << stat2->GetBinError(4) << endl;

	// stat2->Draw();
	// stat1->GetXaxis()->SetRangeUser( 0, 2 );
	TCanvas * cc1 = new TCanvas( "cc1" );
	rpl.style( stat1 )
		.set( "xr", 0, 2.0 )
		.set( "logY", 1 )
		.draw();
	rpl.style( stat2 )
		.set( "lc", kRed )
		.set( "draw", "same" )
		.draw();

	// TH1* sys = (TH1*)sys1->Clone( (energy+"sys_ratio").c_str() );
	TH1* stat = (TH1*)stat2->Clone( (energy+"stat_ratio").c_str() );

	// // sys->Divide( sys2 );
	stat->Divide( stat1 );
	// cout << "ErrTot = " << stat->GetBinError( 4 ) << endl;


	map<string, string> cLabels = 
	{
		{ "0", "0-5%" }
	};

	// // rpl.style( sys ).set( "color", color, 0.5 ).set( "fca", color, 0.15 )
	// // 	.set( "title", " ; pT [GeV/c]; " + plc + " / " + plc )
	// // 	.set( "markerstyle", ms )
	// // 	.set( "markersize", 2 )
	// // 	.set( "optstat", 0 )
	// // 	.set( "draw", draw_opt + " e2" )
	// // 	.set( "xts", 0.10 )
	// // 	.set( "xto", 0.8 )
	// // 	.set( "xls", 0.06 )
	// // 	.set( "yts", 0.10 )
	// // 	.set( "yto", 0.7 )
	// // 	.set( "yls", 0.06 )
	// // 	.set( "xr", 0.1, 10.0 ).draw();

	TCanvas * cc2 = new TCanvas( "cc2" );
	TLine * l = new TLine( 0, 1.0, 2.0, 1.0 );
	gStyle->SetOptFit(111);
	if ( "P" == plc && "n" == charge && "0" ==iCen )
		stat->Fit( "pol0", "R", "", 0.7, 1.3 );
	else	
		stat->Fit( "pol0" );

	rpl.style( stat ).set( "color", color, 0.15 )
		.set( "title", plc_label( plc, charge ) + " " + cLabels[ iCen ] + " ; pT [GeV/c]; Lokesh /  Evan" )
		.set( "markerstyle", ms ).set( "draw", "same" )
		.set( "xts", 0.05 )
		.set( "xr", 0, 2.0 )
		.set( "yr", 0.5, 2.0 )
		.set( "logY", 0 )
		.set( "xto", 0.8 )
		// .set( "xls", 0.06 )
		// .set( "yts", 0.10 )
		// .set( "yto", 0.7 )
		// .set( "yls", 0.06 )
		.draw();
	l->SetLineColor( kBlack );
	l->SetLineStyle( kDashed );
	l->Draw( "same" );

	cc2->Print( ("ratio" + energy + "_" + plc + "_" + charge + "_" + iCen + ".pdf").c_str() );

	// return sys;
}