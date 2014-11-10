

void gnRcp( string nName = "~/bnl/local/data/allSpectra.root", string gName = "simGauss.root" ){


	TFile * nF = new TFile( nName.c_str(), "READ" );
	TFile * gF = new TFile( gName.c_str(), "READ" );

	TCanvas * c = new TCanvas( "c", "rcp", 900, 600 );


	

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

			TH1D* hC = ((TH1D*)gF->Get( ("yield_"+charge+"_0to5_"+species[iS]).c_str() ))->Clone( species[iS].c_str() );
			TH1D* hP = ((TH1D*)gF->Get( ("yield_"+charge+"_60to80_"+species[iS]).c_str() ))->Clone( ("per"+species[iS]).c_str() );;
			hC->Rebin(2);
			hP->Rebin(2);

			hC->SetTitle( "" );
			//hC->Sumw2();
			//hP->Sumw2();
			
			hC->Scale( 19 );

			hP->Scale( 790 );

			//hC->Divide( hP );
			hC->SetLineColor( kRed );

			hC->Scale(7.2111);

			hC->Draw();
			hC->SetLineWidth( 2 );

			
			//hC->GetYaxis()->SetRangeUser( 0.06, 3 );


			
			string bName = "pt_dedx_" + species[ iS ] + "_" + charge + "_";
			string cName = bName + "0to5";
			string pName = bName + "60to80";	

			TH1D * central = ((TH1D*)nF->Get( cName.c_str() ))->Rebin(2);
			TH1D * per = ((TH1D*)nF->Get( pName.c_str() ))->Rebin(2);
			
			per->Sumw2();
			central->Sumw2();

			central->Scale( 19 );
			per->Scale( 790 );
			
			TH1D* rcp = (TH1D*)central->Clone( ("rcp_ellipse"+species[iS]).c_str() );
			
			rcp->SetTitle( "" );
			//rcp->Divide( per );
			rcp->SetLineWidth( 2 );
			//rcp->GetYaxis()->SetRangeUser( .06, 3 );
			rcp->GetXaxis()->SetTitle("");

			//rcp->SetLineColor( 2 );
			//rcp->SetBinContent( 1, -99 );

			
			rcp->Draw("SAME");
			//rcp->Draw();
			//hC->Draw("SAME");
			gPad->SetLogy();

			if ( iS == 0 && iC == 0  ){
				leg = new TLegend( 0.1, 0.7, 0.6, 1.0 );
				leg->SetFillColor( kWhite );
				leg->AddEntry( rcp, "n#sigma dEdx", "lpf" );
				leg->AddEntry( hC, "Sim Gaussians", "lpf" );
				leg->Draw();
			}
			

			
			
		}
	}




}