

void MathematicaExport( string name="tofSigmaK", string inFile = "../../products/sgf/15GeV/sgf_lParams.root", string hName = "K_zbSigma/sigma_K_0_n_0" ){


	TFile * f = new TFile( inFile.c_str(), "READ" );

	if ( !f ){
		cout << "File DNE" << endl;
		return;
	}

	TH1D * h = (TH1D*)f->Get( hName.c_str() );
	if ( !h ){
		cout << "Histo DNE" << endl;
		return;
	}

	cout << endl<< name << "={";
	for ( int i = 1; i <= h->GetNbinsX(); i++ ){

		double p = h->GetBinCenter( i );
		double bc = h->GetBinContent( i );
		cout << "{ " << p * 1000 << ", " << bc << " }";

		if ( i !=  h->GetNbinsX() )
			cout << ", "; 
		
	}

	cout <<"};" << endl;
	

	cout << endl<< name << "={";
	for ( int i = 1; i <= h->GetNbinsX(); i++ ){

		double p = h->GetBinCenter( i );
		double bc = h->GetBinContent( i );
		cout << "{{ " << p * 1000 << ", " << bc << " }";
		cout << ", ErrorBar[ " << h->GetBinError( i ) << " ] }";

		if ( i !=  h->GetNbinsX() )
			cout << ", "; 
		
	}

	cout <<"};" << endl;



}