

void rapidity( double m = 0.139, double pt = 0.2, double eta = 0.0 ){

	double y = TMath::Log(  (TMath::Sqrt( m*m + pt*pt*TMath::CosH(eta)*TMath::CosH(eta)) + pt * TMath::SinH( eta ) ) / TMath::Sqrt( m*m + pt*pt ) );

	cout << "y = " << y << endl;
	cout << "p = " << pt * TMath::CosH( eta ) << endl;
}