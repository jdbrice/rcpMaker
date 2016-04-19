

// Copied from Common.h
string plc_label( string plc, string charge = "na" ){
		if ( "Pi" == plc && "p" == charge )
			return "#pi^{+}";
		if ( "Pi" == plc && "n" == charge )
			return "#pi^{-}";
		if ( "K" == plc && "p" == charge )
			return "K^{+}";
		if ( "K" == plc && "n" == charge )
			return "K^{-}";
		if ( "P" == plc && "p" == charge )
			return "p";
		if ( "P" == plc && "n" == charge )
			return "#bar{p}";

		if ( "Pi" == plc )
			return "#pi";
		if ( "K" == plc )
			return "K";
		if ( "P" == plc )
			return "p";

		return "";
	}


void fitParamSet( string plc = "Pi", string charge = "n", int iPar = 0 ){

	gStyle->SetOptStat( 0 );
	gStyle->SetOptFit( 111 );

	XmlConfig cfg( "TpcEff.xml" );
	Logger::setGlobalLogLevel( "info" );
	vector<int> centralityBins = { 0, 1, 2, 3, 4, 5, 6 };
	vector<double> centrality = { 266.9, 218.8, 167.2, 114.3, 75.06, 36.99, 11.8 };
	vector<XmlFunction> fns;

	vector<double> x, y, ey;

	for ( int i : centralityBins ){
		INFO( "", fmt::format("{0}_{1}.TpcEffParams[{2}]", plc, charge, i) );
		XmlFunction fn( &cfg, fmt::format("{0}_{1}.TpcEffParams[{2}]", plc, charge, i) );
		fns.push_back( fn );
	}




	// Parameter A
	y.clear();
	x.clear();
	for ( int i : centralityBins ){
		y.push_back( fns[i].getTF1()->GetParameter( iPar ) );
		ey.push_back( fns[i].getTF1()->GetParError( iPar ) );
		x.push_back( centrality[ i ] );
	}
	TH1I* frame = new TH1I( "frame", (plc_label( plc, charge ) + " efficiency fit parameter " + ts( iPar ) ).c_str(), 100, 0, 300 );
	TGraphErrors * gA = new TGraphErrors( x.size(), x.data(), y.data(), 0, ey.data() );
	TF1 * fpol = new TF1( "fpol1", "pol1" );
	gA->Fit( fpol, "SE" );
	TFitResultPtr fitResult = gA->Fit( fpol, "SE" );

	double x1 = 0;
	double x2 = 300;

	TGraphErrors * band = FitConfidence::choleskyBands( fitResult, fpol, 500, 100, nullptr, x1, x2 );

	frame->Draw();
	vector<double> rangeMod = { 0.05, 0.05, 2 };
	frame->GetYaxis()->SetRangeUser( *std::min_element(y.begin(),y.end()) - rangeMod[ iPar ], *std::max_element(y.begin(),y.end()) + rangeMod[ iPar ] );
	
	gA->Draw( "same pe" );
	band->SetFillColorAlpha( kRed, 0.7 );
	band->Draw( "same e3" );
	gA->SetMarkerStyle( 8 );
	gA->SetMarkerSize( 2 );

	// gA->GetXaxis()->SetRangeUser( 0, 10 );
	// XmlFunction fn( &cfg, "Pi_n.TpcEffParams[0]" );

	
	// INFO( "", fn.getTF1()->GetParameter( 0 ) );
	

	// translate name to even format
	map<string, string> chargeMap = { { "p" , "c1" }, { "n" , "c0" } } ;
	map<string, string> plcMap = { { "Pi" , "p0" }, { "K" , "p1" }, { "P" , "p2" } };
	map<int, string> parMap = { { 0 , "a" }, { 1 , "b" }, { 2 , "c" } };


	gPad->Print( fmt::format( "efficiency_{0}_{1}_{2}.png", chargeMap[ charge ], plcMap[ plc ], parMap[ iPar ] ).c_str() );

}