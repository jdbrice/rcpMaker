
std::string ts( int i, int len = -1){
	if ( len <= -1 )
		return (to_string( (long long int) i));
	else
		return (to_string( (long long int) i)).substr( 0, len );
}

TFile *f, *out;

int color[] = {632, 416, 600, 616, 800, 880, 820, 840, 860, 900};
string cNames[] = { "0-5%", "5-10%", "10-20%", "20-30%", "30-40%", "40-60%", "60-80%" };


TH1D* plotYield( string plc, string c, int iCen, double scale = 1.0 ){

	TH1D * hPi = (TH1D*)f->Get( (plc + "/pt_" + ts(iCen) +  "_" + c).c_str() );
	TH1D * hRM = (TH1D*)f->Get( "/EventQA/mappedRefMultBins" );
	double n0 = hRM->GetBinContent( iCen+1 );

	cout << "n0 = " << n0 << endl;	
	TH1D * h = (TH1D*)hPi->Clone( (plc + "_" + c + "_" + ts(iCen)).c_str() );
	h->SetMarkerStyle( 20 + iCen );
	h->SetMarkerColor( color[ iCen ] );
	h->SetLineWidth(2);
	h->Sumw2();
	for ( int i = 1; i < h->GetNbinsX(); i++ ){
		double bc = h->GetBinContent( i );
		double be = h->GetBinError( i );
		double pt = h->GetBinCenter( i );
		double bw = h->GetBinWidth( i );


		if ( i > 6 ){
			h->SetBinContent( i, scale * bc / ( n0 * 2 * 3.141592 * pt ) );	
			h->SetBinError( i,  scale * be / ( n0 * 2 * 3.141592 * pt  )  );	
		} else {
			h->SetBinContent( i, 0 );	
			h->SetBinError( i,  0  );	
		}
		
	}
	h->SetBinContent( 28, 0 );	
	h->SetBinError( 28,  0  );	

	if ( iCen == 0 ){
		h->Draw(  );
		h->GetYaxis()->SetRangeUser( 1e-14, 1e6 );
		gPad->SetLogy( 1 );
	}else 
		h->Draw("same" );
	
	return h;
}

string human( int iPlc, int iC ){

	string names[] = { "#pi +", "#pi -", "K+", "K-", "Proton", "Anti-Proton" };
	return names[ iPlc * 2 + iC ]; 
}


void spectra( string dFile = "collab_june_02_15/spectra_mf_full.root" ){

	gStyle->SetOptStat( 0 );
	//Reporter rp( "spectra.pdf", 350, 450 );

	TCanvas * can = new TCanvas( "c", "c", 600, 1000 );
	can->SetLeftMargin( 0.18 );
	can->SetBottomMargin( 0.15 );

	f = new TFile( dFile.c_str(), "READ" );
	out = new TFile( "dN_dptdy.root", "RECREATE" );

	string plcs[] = { "Pi", "K", "P" };
	string cs[] = { "p", "n" };
	int cen[] = {0, 1, 2, 3, 4, 5, 6};  

	for ( int iPlc = 0; iPlc < 3; iPlc++ ){
		
		for ( int iC = 0; iC < 2; iC++ ){
			TLegend * leg= new TLegend( 0.6, 0.6, 0.9, 0.9 );
			leg->SetTextSize(0.03);
			for ( int iCen = 0; iCen < 7; iCen++ ){
				TH1D * h = plotYield( plcs[iPlc], cs[iC], cen[iCen], pow( 10, -iCen ) );			
				
				h->SetTitle( human( iPlc, iC ).c_str() );
				h->GetYaxis()->SetLabelSize( 0.03 );
				h->GetXaxis()->SetLabelSize( 0.03 );
				h->GetYaxis()->SetTitle( "d^{2}N / ( N_{Events} 2#pi pT dpT dy )" );

				if ( iCen > 0 ){
					string n = cNames[ iCen ] + " x 10^{-" + ts(iCen) + "}";
					leg->AddEntry( h, n.c_str() );
				}
				else 
					leg->AddEntry( h, (cNames[ iCen ]).c_str() );
			}

			leg->Draw();
			can->Print( ( "spectra_" + plcs[ iPlc ] + "_" + cs[iC] + ".eps" ).c_str() );
			//rp.savePage();
			//rp.saveImage( ("report/spectra_" + plcs[iPlc] + "_" + cs[iC] + ".pdf").c_str() );
		}
	}
	



	out->Write();
}