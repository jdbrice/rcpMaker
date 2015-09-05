#include "Common.h"

// ROOT
#include "TVirtualFitter.h"


 vector<string> Common::species = { "Pi", "K", "P" };

 vector<int> Common::charges = { -1, 1 };
 vector<string> Common::sCharges = { "n", "p" };

 string Common::toXml( TF1 * f ){

 	string line = "";
 	line += "formula=\"";
 	line += f->GetTitle();
 	line += "\"";

 	for ( int ip = 0; ip < f->GetNpar(); ip++ ){

 		line += " p" + ts(ip) + "=\"";
 		line += dts(f->GetParameter(ip));
 		line += "\"";

 		line += " e" + ts(ip) + "=\"";
 		line += dts(f->GetParError(ip));
 		line += "\"";
 	}

 	return line;
 }

 string Common::toXml( TF1 * f, TFitResultPtr result ){

 	string line = "";
 	line += "formula=\"";
 	line += f->GetTitle();
 	line += "\"";

 	for ( int ip = 0; ip < f->GetNpar(); ip++ ){

 		line += " p" + ts(ip) + "=\"";
 		line += dts(f->GetParameter(ip));
 		line += "\"";

 		line += " e" + ts(ip) + "=\"";
 		line += dts(f->GetParError(ip));
 		line += "\"";
 	}

 	// include covariance matrix if available
	int nP = f->GetNpar();

	TMatrixDSym cov = result->GetCovarianceMatrix();

	double *covArray = new double[ nP * nP ]; // number of parameters x number of parameters
	covArray = cov.GetMatrixArray();

	vector<double> cv;
	cv.assign( covArray, covArray + nP*nP );
	line += " cov=\"" + toString( cv ) + "\"";


 	return line;
 }

// TODO: finish the histogram toXml
string Common::toXml( TH1 * h ){

	string line1 = "<bins>";
	// bins frst
	for ( int i = 1; i < h->GetNbinsX() + 1; i++ ){
		line1 += dts( h->GetBinLowEdge( i ) ) + ", ";
	}
	line1 += dts( h->GetBinLowEdge( h->GetNbinsX() ) + h->GetBinWidth( h->GetNbinsX() ) );
	line1 += "</bins";

	return line1;
}


string Common::toXml(  TGraph * g, string linePrefix  ){

	string lx = "<x>";
	string ly = "<y>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double x, y;
		g->GetPoint( iP, x, y );
		lx += dts( x );
		ly += dts( y );

		if ( iP < g->GetN() - 1 ){
			lx += ", ";
			ly += ", ";
		}
	}
	lx += "</x>";
	ly += "</y>";

	return linePrefix + lx + "\n" + linePrefix + ly;
}

string Common::toXml( TGraphErrors * g ){
	string a = Common::toXml( (TGraph*) g );

	string lex = "<ex>";
	string ley = "<ey>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double ex, ey;
		ex = g->GetErrorX( iP );
		ey = g->GetErrorY( iP );
		lex += dts( ex );
		ley += dts( ey );

		if ( iP < g->GetN() - 1 ){
			lex += ", ";
			ley += ", ";
		}
	}

	return a + "\n" + lex + "\n" + ley;
}

string Common::toXml( TGraphAsymmErrors * g, string linePrefix ){
	string a = Common::toXml( (TGraph*) g, linePrefix );

	string lexh = "<exh>";
	string leyh = "<eyh>";
	string lexl = "<exl>";
	string leyl = "<eyl>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double exh, eyh;
		double exl, eyl;
		exh = g->GetErrorXhigh( iP );
		eyh = g->GetErrorYhigh( iP );
		lexh += dts( exh );
		leyh += dts( eyh );

		exl = g->GetErrorXlow( iP );
		eyl = g->GetErrorYlow( iP );
		lexl += dts( exl );
		leyl += dts( eyl );

		if ( iP < g->GetN() - 1 ){
			lexh += ", ";
			leyh += ", ";
			lexl += ", ";
			leyl += ", ";
		}
	}

	lexh += "</exh>";
	leyh += "</eyh>";
	lexl += "</exl>";
	leyl += "</eyl>";

	return a + "\n" + linePrefix + lexh + "\n" + linePrefix + leyh +"\n" + linePrefix + lexl + "\n" + linePrefix + leyl;
}

 TH1 * Common::fitCL( TF1 * f, string name, double cl, int nPoints, double x1, double x2 ){

 	// calculate instead
 	if ( -1.0 == x1  && -1.0 == x2 )
 		f->GetRange( x1, x2 );
 	INFO( tag, "Range (" << x1 <<", " << x2 << " )" );
 	
 	if ( nPoints < 2 )
 		nPoints = 100;
 	INFO( tag, "Plotting with " << nPoints << plural( nPoints, " point", " points" ) );

 	// histo to hold CL
 	TH1F * hCL = new TH1F( name.c_str(), name.c_str(), nPoints, x1, x2 ); 
 	(TVirtualFitter::GetFitter())->GetConfidenceIntervals(hCL, cl);

 	return hCL;
 }

 void Common::calcCholesky( int nP, double * fCov, double* fCovSqrt ){

	double *C = fCovSqrt;
	double *V = fCov;

	// calculate sqrt(V) as lower diagonal matrix
	for( int i = 0; i < nP; ++i ) {
		for( int j = 0; j < nP; ++j ) {
			C[i*nP+j] = 0;
		}
	}

	for( int j = 0; j < nP; ++j ) {
		// diagonal terms first
		double Ck = 0;
		for( int k = 0; k < j; ++k ) {
			Ck += C[j*nP+k] * C[j*nP+k];
		} // k
		C[j*nP+j] = sqrt( fabs( V[j*nP+j] - Ck ) );

		// off-diagonal terms
		for( int i = j+1; i < nP; ++i ) {
			Ck = 0;
			for( int k = 0; k < j; ++k ) {
				Ck += C[i*nP+k] * C[j*nP+k];
			} //k
			if( C[ j * nP + j ] != 0 ) 
				C[ i * nP + j ] = ( V[ i * nP + j ] - Ck ) / C[ j * nP + j ];
			else 
				C[ i * nP + j ] = 0;
		}// i
	} // j
} // calcCholesky


double Common::randomSqrtCov( double xx, TF1 * f, int nP, double * fCovSqrt ){
	double * z = new double[nP];
	double * x = new double[nP];
	double * p = new double[nP];

	for( int i = 0; i < nP; ++i ) {
		z[i] = gRandom->Gaus( 0.0, 1.0 );
		p[i] = f->GetParameter(i);
	}

	for( int i = 0; i < nP; ++i ) {
		x[i] = 0;
		for( int j = 0; j <= i; ++j ) {
			x[i] += fCovSqrt[i*nP+j] * z[j];
		} // j
	}

	for( int i = 0; i < nP; ++i ) {
		f->SetParameter( i, x[i] + p[i] );
	}

	double value = f->Eval(xx);
	for( int i = 0; i < nP; ++i ) {
		f->SetParameter( i, p[i] );
	}

	delete [] z;
	delete [] x;
	delete [] p;
	return value;
}


double Common::choleskyUncertainty( double xx, TFitResultPtr fitResult, TF1 * f, int nSamples ){
	int nP = f->GetNpar();

	TMatrixDSym cov = fitResult->GetCovarianceMatrix();
	double *covArray = new double[ nP * nP ]; // number of parameters x number of parameters
	covArray = cov.GetMatrixArray();


	choleskyUncertainty( xx, covArray, f, nSamples );
}


double Common::choleskyUncertainty( double xx, double * fCov, TF1 * f, int nSamples ){
	int nP = f->GetNpar();
	INFO( tag, "Num Params : " << nP  );
	
	double *fCovSqrt = new double[ nP * nP ];
	calcCholesky( nP, fCov, fCovSqrt );

	double yerr = 0;

	TH1D *hDistributionAtX = new TH1D("hDistributionAtX","",200,f->Eval(xx) - .2,f->Eval(xx) + .2);
	
	for (int n = 0; n < nSamples; n++ ) {
		double val = randomSqrtCov(xx,f,nP,fCovSqrt);
		hDistributionAtX->Fill( val );
	}
	yerr = hDistributionAtX->GetRMS();
	hDistributionAtX->Delete();

	return yerr;

}

TGraphErrors *Common::choleskyBands( TFitResultPtr fitResult, TF1 * f, int nSamples, int nPoints, Reporter* rp,  double x1, double x2 ){

	int nP = f->GetNpar();
	// INFO( tag, "Num Params : " << nP  );
	TMatrixDSym cov = fitResult->GetCovarianceMatrix();
	double *covArray = new double[ nP * nP ]; // number of parameters x number of parameters
	covArray = cov.GetMatrixArray();

	// for ( int i = 0; i < 9; i++ ){
	// 	INFO( tag, "[" << i << "] = " << covArray[ i ] );
	// }

	double *fCov = new double[ nP * nP ];
	fCov = cov.GetMatrixArray();
	double *fCovSqrt = new double[ nP * nP ];
	calcCholesky( nP, fCov, fCovSqrt );

	// for ( int i = 0; i < 9; i++ ){
	// 	INFO( tag, "[" << i << "] = " << fCovSqrt[ i ] );
	// }

	

	// calculate instead
	if ( -1.0 == x1  && -1.0 == x2 )
		f->GetRange( x1, x2 );

	double step = ( x2 - x1 ) / (double) nPoints;

	double x[ nPoints + 1 ];
	double y[ nPoints + 1 ];
	double yup[ nPoints + 1 ];
	double ydown[ nPoints + 1 ];
	double yerr[ nPoints + 1 ];
	vector<double> samples;

	int i = 0;
	for (double xx = x1; xx < x2; xx+= step) {
		x[i] = xx;
		TH1D *hDistributionAtX = new TH1D("hDistributionAtX","",200,f->Eval(x[i]) - .2,f->Eval(x[i]) + .2);
		for (int n = 0; n < nSamples; n++ ) {
			double val = randomSqrtCov(x[i],f,nP,fCovSqrt);
			hDistributionAtX->Fill( val );
			samples.push_back( val );
		}
		//hDistributionAtX->DrawCopy();

		y[i] 		= f->Eval(x[i]);
		yerr[i] 	= hDistributionAtX->GetRMS();
		
		// cross check - should always give the same result
		// but very innefficient
		//yerr[i] = choleskyUncertainty( xx, fitResult, f, nSamples );

		// rp.savePage();
		hDistributionAtX->Delete();
		i++;
	} 

	TGraphErrors * g = new TGraphErrors( i - 1, x, y, 0, yerr );

	return g;
}