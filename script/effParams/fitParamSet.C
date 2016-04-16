


void fitParamSet( string plc = "Pi", string charge = "n", int iPar = 0 ){

	gStyle->SetOptStat( 0 );
	gStyle->SetOptFit( 111 );

	XmlConfig cfg( "TpcEff.xml" );
	Logger::setGlobalLogLevel( "info" );
	vector<int> centralityBins = { 0, 1, 2, 3, 4, 5, 6 };
	vector<double> centrality = { 2.5, 7.5, 15, 25, 35, 50, 70 };
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
	TH1I* frame = new TH1I( "frame", "frame", 100, 0, 100 );
	TGraphErrors * gA = new TGraphErrors( x.size(), x.data(), y.data(), 0, ey.data() );
	TF1 * fpol = new TF1( "fpol1", "pol1" );
	TFitResultPtr fitResult = gA->Fit( fpol, "S" );
	TGraphErrors * band = FitConfidence::choleskyBands( fitResult, fpol, 500, 100, nullptr, *std::min_element(x.begin(),x.end()) - 0.05, *std::max_element(x.begin(),x.end()) + 0.05 );

	frame->Draw();
	frame->GetYaxis()->SetRangeUser( *std::min_element(y.begin(),y.end()) - 0.05, *std::max_element(y.begin(),y.end()) + 0.05 );
	
	gA->Draw( "same pe" );
	band->SetFillColorAlpha( kRed, 0.7 );
	band->Draw( "same e3" );
	gA->SetMarkerStyle( 8 );
	gA->SetMarkerSize( 2 );

	// gA->GetXaxis()->SetRangeUser( 0, 10 );
	// XmlFunction fn( &cfg, "Pi_n.TpcEffParams[0]" );

	
	// INFO( "", fn.getTF1()->GetParameter( 0 ) );

}