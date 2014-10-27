

void nRcp( string fName = "paramSpectra.root" ){

	vector<string> cuts;
	cuts.push_back( "dedx" );
	cuts.push_back( "tof" );
	//cuts.push_back( "square" );
	cuts.push_back( "ellipse" );

	vector<string> cutTitles;
	cutTitles.push_back( "dE/dx\n" );
	cutTitles.push_back( "#beta^{-1}" );
	cutTitles.push_back( "Ellipse" );

	TFile * f = new TFile( fName.c_str(), "READ" );

	TCanvas * c = new TCanvas( "c", "rcp", 800, 600 );

	gStyle->SetOptStat( 0 );

	c->Divide( 3, 2, 0, 0.2 );
	c->cd(1);

	vector<string> species;
	species.push_back( "Pi" );
	species.push_back( "K" );
	species.push_back( "P" );

	for ( int iC = 0; iC < 2; iC++ ){
		string charge = "p";
		if ( 1 == iC )
			charge = "n";
		for ( int iS = 0; iS < species.size(); iS++ ){
			c->cd( 3*iC + iS + 1); 

			TLegend *leg = new TLegend( 0.1, 0.5, 0.5, 1.0 );
			for ( int i = 0; i < cuts.size(); i++  ){

				string bName = "pt_" + cuts[ i ] + "_" + species[ iS ] + "_" + charge + "_";
				string cName = bName + "0to5";
				string pName = bName + "60to80";	

				TH1D * central = ((TH1D*)f->Get( cName.c_str() ))->Rebin(2);
				TH1D * per = ((TH1D*)f->Get( pName.c_str() ))->Rebin(2);
				
				per->Sumw2();
				central->Sumw2();

				central->Scale( 19 );
				per->Scale( 790 );
				
				TH1D* rcp = (TH1D*)central->Clone( ("rcp_"+cuts[i]).c_str() );
				
				rcp->SetTitle( "" );
				rcp->Divide( per );
				rcp->SetLineWidth( 2 );
				rcp->GetYaxis()->SetRangeUser( .06, 3 );
				rcp->GetXaxis()->SetTitle("");

				rcp->SetLineColor( i+1 );
				//rcp->SetBinContent( 1, -99 );

				if ( 0 == i )
					rcp->Draw();
				else 
					rcp->Draw("SAME" );
				gPad->SetLogy(); 


				if ( 0 == iS && 0 == iC ){
					leg->AddEntry( rcp, cutTitles[i].c_str() );
					leg->SetTextSize( .1 );
					leg->Draw();	
				}


				
			}
		}
	}




}