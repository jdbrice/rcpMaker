


void gRcp( string cen = "../bin/simGauss.root" ){


	TFile * fc = new TFile( cen.c_str(), "READ" );

	TH1D* hC = fc->Get( "yield_n_0to5_K" );
	TH1D* hP = fc->Get( "yield_n_60to80_K" );

	hC->Sumw2();
	hP->Sumw2();

	hC->Scale( 19 );
	hP->Scale( 790 );

	hC->Divide( hP );


	hC->Draw();

	gPad->SetLogy();
	hC->GetYaxis()->SetRangeUser( 0.06, 3 );




}