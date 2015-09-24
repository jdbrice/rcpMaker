

TH1 * events;

void normalize( TH1 * h, int iCen ){

	//debadeepti's rapidity cut
	double dy = ( 0.1 -  -0.1 );

	// +1 bc iCen == 0 ==> first bin
	h->Scale( 1.0 / ( events->GetBinContent( iCen + 1 ) ) );
	h->Scale( 1.0 / ( 2 * M_PI ) );
	h->Scale( 1.0 / ( dy ) );

	for ( int i = 1; i <= h->GetNbinsX(); i++ ){

		double pT 		= h->GetBinCenter( i );
		double val 		= h->GetBinContent( i );
		double error 	= h->GetBinError( i );

		h->SetBinContent( i, val / pT  );
		h->SetBinError( i, error / pT );
	}	
}

void write( TH1 * h, string fn ){

	ofstream fout( fn.c_str() );

	fout << std::setprecision( 10 ) << std::left << std::setw(20) << "pT" << std::left << std::setw(20) << "value" << std::left << std::setw(20) << "stat" << std::left << std::setw(20) << "sys" << endl; 
	
	for ( int i = 1; i <= h->GetNbinsX(); i++ ){
		fout << std::setprecision( 10 ) << std::left << std::setw(20) << h->GetBinCenter( i ) << std::left << std::setw(20) << h->GetBinContent( i ) << std::left << std::setw(20) << h->GetBinError( i ) << std::left << std::setw(20) << 0.0 << endl; 
	}

	fout.close();

}


void export_raw_spectra( string plc ="Pi", string charge="p" ){

	string fn = "inclusive_" + plc + "_evt.root";

	TFile * f = new TFile( fn.c_str(), "READ" );

	events = (TH1D*)f->Get( "EventQA/mappedRefMultBins" );

	for ( int iCen = 0; iCen < 7; iCen++ ){

		TH1 * hTpc = (TH1D*)f->Get( ("inclusive/pt_" + ts(iCen) + "_" + charge ).c_str() );

		normalize( hTpc, iCen );
		write( hTpc, "tpc_inclusive_raw_" + plc + "_" + charge + ".dat" );

		TH1 * hTof = (TH1D*)f->Get( ("inclusiveTof/pt_" + ts(iCen) + "_" + charge ).c_str() );
		normalize( hTof, iCen );
		write( hTof, "tof_inclusive_raw_" + plc + "_" + charge + ".dat" );

	}

}