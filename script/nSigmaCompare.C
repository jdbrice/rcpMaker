

void nSigmaCompare( string fname = "paramSpectra.root", string s = "K"){

	TCanvas * c = new TCanvas( "c", "Rcp", 800, 600 );
	TFile * f = new TFile( fname.c_str(), "READ" );

	vector<string> cuts;
	cuts.push_back( "dedx" );
	cuts.push_back( "tof" );
	//cuts.push_back( "square" );
	cuts.push_back( "ellipse" );

	TLegend *leg = new TLegend( 0.6, 0.6, 0.9, 0.9 );
	for ( int i = 0; i < cuts.size(); i++  ){

		string bName = "pt_" + cuts[ i ] + "_" + s + "_n_";
		string cName = bName + "0to5";
		string pName = bName + "60to80";	

		TH1D * central = ((TH1D*)f->Get( cName.c_str() ))->Rebin(3);
		TH1D * per = ((TH1D*)f->Get( pName.c_str() ))->Rebin(3);
		
		per->Sumw2();
		central->Sumw2();

		central->Scale( 19 );
		per->Scale( 790 );
		
		TH1D* rcp = (TH1D*)central->Clone( ("rcp_"+cuts[i]).c_str() );
		
		rcp->SetTitle( "0%-5% / 60%-80% " );
		rcp->Divide( per );

		rcp->SetLineColor( i+1 );

		if ( 0 == i )
			rcp->Draw();
		else 
		rcp->Draw("SAME" ); 

		leg->AddEntry( rcp, cuts[i].c_str() );

		leg->Draw();


	}

	
	
	

}