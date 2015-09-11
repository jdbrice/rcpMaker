

// [0] = A1
// [1] = mu1
// [2] = sig1
// [3] = A2 
// [4] = mu2 
// [5] = sig2 
// [6] = A3
// [7] = mu3
// [8] = sig3
string gauss3 = "[0] / ( [2] * 2.506628 )* exp( -0.5*(  (x-[1])^2 / ([2]^2)  ) ) + [3] / ( [5] * 2.506628 )* exp( -0.5*(  (x-[4])^2 / ([5]^2)  ) ) + [6] / ( [8] * 2.506628 )* exp( -0.5*(  (x-[7])^2 / ([8]^2)  ) ) ";
string gauss2 = "[0] / ( [2] * 2.506628 )* exp( -0.5*(  (x-[1])^2 / ([2]^2)  ) ) + [3] / ( [5] * 2.506628 )* exp( -0.5*(  (x-[4])^2 / ([5]^2)  ) )";
string gauss1 = "[0] / ( [2] * 2.506628 )* exp( -0.5*(  (x-[1])^2 / ([2]^2)  ) )";


void name( TF1 * f ){
	f->SetParName( 0, "A1" );
	f->SetParName( 3, "A2" );
	f->SetParName( 6, "A3" );
	f->SetParName( 1, "mu1" );
	f->SetParName( 4, "mu2" );
	f->SetParName( 7, "mu3" );
	f->SetParName( 2, "sig1" );
	f->SetParName( 5, "sig2" );
	f->SetParName( 8, "sig3" );
}

void limit_A( TF1 * f, double max = 1000, double bw = 1.0 ){
	f->SetParLimits( 0, 1 * bw, max );
	f->SetParLimits( 3, 1 * bw, max );
	f->SetParLimits( 6, 1 * bw, max );
}

void draw_comp( TF1 * f, int s ){
	TF1 * f_c = new TF1( ("ff" + ts(s)).c_str(), gauss1.c_str(), -10, 10 );
	f_c->SetLineColor( s+1 );
	f_c->SetParameters( f->GetParameter(s+0), f->GetParameter(s+1), f->GetParameter(s+2) );
	f_c->Draw( "same" );
}

void limit_mu( TF1 * f, int p, double mu, double sig = 0.012 ){
	f->SetParLimits( p, mu - sig * 1, mu + sig * 1 );
}

void limit_sig( TF1 * f, int p, double sig = 0.012 ){
	f->SetParLimits( p, sig  * 0.8, sig * 1.2 );
}

void fix_current( TF1 * f, int p ){
	f->FixParameter( p, f->GetParameter( p ) );
}
void fix_shapes( TF1* f){
	fix_current( f, 2 );
	fix_current( f, 3 );

	fix_current( f, 5 );
	fix_current( f, 6 );

	fix_current( f, 8 );
	fix_current( f, 9 );
}

void double_current( TF1 * f, int p ){
	f->FixParameter( p, f->GetParameter( p ) * 2.0 );
}

void run_simple_fit( TH1 * h, double mu1, double mu2, double mu3, double sig = 0.012, double bw = 1.0 ){
	
	TF1 * f = new TF1( "ff", gauss3.c_str(), mu1 - sig * 2, mu3 + sig * 2 );
	double A = 10;
	f->SetParameters( A, mu1, 0.012, A, mu2, 0.012, A * 5, mu3, 0.012 );
	
	name( f );
	limit_A( f, 100000, bw );
	limit_mu( f, 1, mu1 );
	limit_mu( f, 4, mu2 );
	limit_mu( f, 7, mu3 );

	limit_sig( f, 2, 0.011 );
	limit_sig( f, 5, 0.011 );
	limit_sig( f, 8, 0.012 );

	f->FixParameter( 4, 0 );
	f->FixParameter( 5, 9.78598e-03 );
	// f->FixParameter( 8, 0.011 );
	
	h->Fit( f, "R" );
	// double_current( f, 3 );
	// double_current( f, 3 );
	// double_current( f, 3 );
	// double_current( f, 3 );
	// fix_shapes( f );
	h->Fit( f, "R" );
	// h->Fit( f, "R" );

	draw_comp( f, 0 );
	draw_comp( f, 3 );
	draw_comp( f, 6 );

	string tag = "simple_fit";
	INFO( tag, "Yield A1 = " << f->GetParameter(0) );
	INFO( tag, "Yield A2 = " << f->GetParameter(3) );
	INFO( tag, "Yield A3 = " << f->GetParameter(6) );

}