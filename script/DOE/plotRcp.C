
string plcName( int plc ){

	if ( 0 == plc )
		return "Pi";
	if ( 1 == plc )
		return "K";
	if ( 2 == plc )
		return "P";
	return "";
}

string hName( int plc, int charge ){

	if ( 0 == plc && 0 == charge )
		return "Pi-";
	if ( 0 == plc && 1 == charge )
		return "Pi+";
	if ( 1 == plc && 0 == charge )
		return "K-";
	if ( 1 == plc && 1 == charge )
		return "K+";
	if ( 2 == plc && 1 == charge )
		return "Proton";
	if ( 2 == plc && 0 == charge )
		return "AntiProton";
	return "";
}

string charge( int c  ){
	if ( 0 == c  )
		return "n";
	if ( 1 == c )
		return "p";
	return "";
}




void individual() {
	TCanvas * can = new TCanvas( "c", "c", 800, 600 );
	gStyle->SetOptStat( 0 );

	string cpf = "collab_june_02_15/rcp.root";

	for ( int p = 0; p < 3; p ++ ){
		for ( int c = 0; c < 2; c++ ){
			string name = "rcp_" + plcName( p ) + "_" + charge( c ) + "_0_Over_6";
			TFile * f = new TFile( cpf.c_str(), "READ" );

			TH1D * crcp = (TH1D*)f->Get( ("rcp/"+name).c_str() );
			crcp->SetMarkerStyle( 22 );
			crcp->SetMarkerColor( kRed );

			for ( int i = 1; i < 6; i++ ){
				crcp->SetBinContent( i, 0 ); 
			}
			crcp->SetBinContent( 28, 0 );
			
			
			
			crcp->GetYaxis()->SetRangeUser( 0.11, 10);
			string title = hName( p, c ) + "; pT [GeV/c]; R_{CP} (0-5%) / (60-80%)";
			crcp->SetTitle( title.c_str() );
			crcp->GetYaxis()->SetTitleOffset( 0.8 );
			
			crcp->Draw();
			gPad->SetLogy(1);
			gPad->SetGrid( 1, 1 );

			can->Print( (plcName( p ) + "_" + charge( c ) + ".eps").c_str() );
		}
	}
}

void all(){
	TCanvas * can = new TCanvas( "c", "c", 800, 600 );
	can->Divide( 3, 2 );
	gStyle->SetOptStat( 0 );

	string cpf = "collab_june_02_15/rcp.root";

	for ( int p = 0; p < 3; p ++ ){
		for ( int c = 0; c < 2; c++ ){

			can->cd( p + c * 3 + 1 );

			string name = "rcp_" + plcName( p ) + "_" + charge( c ) + "_0_Over_6";
			TFile * f = new TFile( cpf.c_str(), "READ" );

			TH1D * crcp = (TH1D*)f->Get( ("rcp/"+name).c_str() );
			crcp->SetMarkerStyle( 22 );
			crcp->SetMarkerColor( kRed );

			for ( int i = 1; i < 6; i++ ){
				crcp->SetBinContent( i, 0 ); 
			}
			crcp->SetBinContent( 28, 0 );
			
			
			
			crcp->GetYaxis()->SetRangeUser( 0.11, 10);
			string title = hName( p, c ) + "; pT [GeV/c]; R_{CP} (0-5%) / (60-80%)";
			crcp->SetTitle( title.c_str() );
			crcp->GetYaxis()->SetTitleOffset( 0.85 );
			crcp->GetYaxis()->CenterTitle(kTRUE );
			
			crcp->Draw();
			gPad->SetLogy(1);
			gPad->SetGrid( 1, 1 );

			
		}
	}

	can->Print( "rcp_all.eps" );
}


void plotRcp(){
	
	all();
	individual();
	

}