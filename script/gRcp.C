


void gRcp( string cen = "simGauss.root", string per = "per.root" ){


	TFile * fc = new TFile( cen.c_str(), "READ" );
	TFile * fp = new TFile( per.c_str(), "READ" );

	TH1D* hC = fc->Get( "yield_p_cen_P" );
	TH1D* hP = fc->Get( "yield_p_per_P" );

	hC->Sumw2();
	hP->Sumw2();

	hC->Scale( 19 );
	hP->Scale( 790 );

	hC->Divide( hP );


	hC->Draw();

	gPad->SetLogy();
	hC->GetYaxis()->SetRangeUser( 0.06, 3 );




}