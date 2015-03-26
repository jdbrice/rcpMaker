


void gRcp( string cen = "simGauss.root" ){


	TFile * fc = new TFile( cen.c_str(), "READ" );

	TH1D* hC = fc->Get( "yield_p_1_P" );
	TH1D* hP = fc->Get( "yield_p_0_P" );

	hC->Sumw2();
	hP->Sumw2();

	hC->Scale( 19 );
	hP->Scale( 790 );

	hC->Divide( hP );


	hC->Draw();

	gPad->SetLogy();
	hC->GetYaxis()->SetRangeUser( 0.06, 3 );




}