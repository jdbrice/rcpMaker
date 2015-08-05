

double y(double m, double pt, double eta) {
	return TMath::Log(  (TMath::Sqrt( m*m + pt*pt*TMath::CosH(eta)*TMath::CosH(eta)) + pt * TMath::SinH( eta ) ) / TMath::Sqrt( m*m + pt*pt ) );
}


void rPlotter(){
	


}