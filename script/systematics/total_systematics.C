#include "single_compare.C"
#include "cov_matrix.C"
#include "tof_eff_systematic.C"

double total_systematics( int iPt, vector<double> &weights, vector<string> &sources, string plc ="Pi", string charge="p", string iCen="0" ){

	vector<double> sigma;
	vector<double> sigmaNW;

	TH1D * nominal = (TH1D*)yield_hist_for( "nominal", plc, charge, iCen );
	double yNominal = nominal->GetBinContent( iPt );

	TRACE( "Nominal Yield = " << yNominal );

	int i = 0;
	for ( string source : sources ){

		TRACE( "getting hist for " << source  );
		TH1D * h = (TH1D*)yield_hist_for( source, plc, charge, iCen );

		TH1D* n2 = (TH1D*)nominal->Clone( "n2" ); 

		n2->Add( h, -1 );

		TRACE( "" << source << "Yield = " << h->GetBinContent( iPt ) );


		double delta = n2->GetBinContent( iPt );//nominal->GetBinContent( iPt ) - h->GetBinContent( iPt );
		
		// reject the crazy outliers with huge uncertainty
		if ( ("dca_low" == source ) && iPt > 22 ){
			delta = yNominal * .005;
		} else if ( iPt > 22 ){ // ~1.5 GeV
			TF1 * ff = new TF1( "ff", "pol0", 1.5, 3.0 );
			n2->Fit( ff, "RNQ" );
			delta = ff->GetParameter( 0 );
		} 


		// is it consistent with zero?
		if ( abs(n2->GetBinError( iPt )) > abs( delta )  )
			delta = 0.0;

		if ( abs (delta / yNominal) > 0.05 )
			delta = 0;

		TRACE( "\t\tRealtive = " << delta / yNominal );

		TRACE( "\t\tDelta = " << delta << " * " << weights[ i ] );
		sigma.push_back( weights[ i ] * delta );
		sigmaNW.push_back( delta );
		
		n2->Delete();
		i++;
	}

	// avg feedDown from other part
	sigma.push_back( yNominal * 0.015 );
	// avg sigma from other part
	sigma.push_back( yNominal * 0.012 );



	
	double total_sqr = 0;
	for ( double v : sigma ){
		total_sqr += v*v;
	}

	double total_sqr_nw = 0;
	for ( double v : sigmaNW ){
		total_sqr_nw += v*v;
	}
	double total = sqrt( total_sqr );
	double totalNW = sqrt( total_sqr_nw );

	TRACE( "Total = " << total << " Relative = " << total / yNominal );
	TRACE( "Total Realtive (No Correlation)= " << totalNW / yNominal );

	return total;
}


vector<double> total_systematics( vector<double> &weights, vector<string> &sources, string plc ="Pi", string charge="p", string iCen="0" ){

	vector<double> totals;


	TH1D * nominal = (TH1D*)yield_hist_for( "nominal", plc, charge, iCen );
	
	// these are relative so multiply by the nominal yield
	vector<double> tof_eff_unc = tof_eff_systematic( plc, charge, iCen );

	for ( int iPt = 1; iPt <= nominal->GetNbinsX(); iPt++ ){

		vector<double> sigma;
		vector<double> sigmaNW;

		double yNominal = nominal->GetBinContent( iPt );
		// cout << "Relative TofEff Unc " << tof_eff_unc[ iPt - 1 ] << endl;
		tof_eff_unc[ iPt - 1 ] = tof_eff_unc[ iPt - 1 ] * yNominal;

		// cout << "Nominal Yield = " << yNominal << endl;


		int i = 0;
		for ( string source : sources ){

			TRACE( "getting hist for " << source  );
			TH1D * h = (TH1D*)yield_hist_for( source, plc, charge, iCen );

			TH1D* n2 = (TH1D*)nominal->Clone( "n2" ); 

			n2->Add( h, -1 );

			TRACE( "" << source << "Yield = " << h->GetBinContent( iPt ) );


			double delta = n2->GetBinContent( iPt );//nominal->GetBinContent( iPt ) - h->GetBinContent( iPt );
			
			// reject the crazy outliers with huge uncertainty
			if ( ("dca_low" == source ) && iPt > 22 ){
				delta = yNominal * .005;
			} else if ( iPt > 22 ){ // ~1.5 GeV
				TF1 * ff = new TF1( "ff", "pol0", 1.5, 3.0 );
				n2->Fit( ff, "RNQ" );
				delta = ff->GetParameter( 0 );
			} 

			// is it consistent with zero?
			if ( abs(n2->GetBinError( iPt )) > abs( delta )  )
				delta = 0.0;

			if ( abs (delta / yNominal) > 0.05 )
				delta = 0;

			TRACE( "\t\tRealtive = " << delta / yNominal );

			TRACE( "\t\tDelta = " << delta << " * " << weights[ i ] );
			sigma.push_back( weights[ i ] * delta );
			sigmaNW.push_back( delta );
			
			n2->Delete();
			i++;
		}

		// avg sigma from other part
		sigma.push_back( yNominal * 0.0 );

		
		double total_sqr = 0;
		for ( double v : sigma ){
			total_sqr += v*v;
		}

		double total = sqrt( total_sqr );
		
		totals.push_back( total );
	}

	// work in the tof_eff ones
	for ( int i = 0; i < totals.size(); i++ ){

		totals[ i ] = sqrt(  tof_eff_unc[i]*tof_eff_unc[i]  );

	}

	cout << " Totals for " << plc << "_" << charge << "_" << iCen << endl;
	for ( double v : totals ){
		cout << v << ", ";
	}
	cout << endl;

	return totals;
}