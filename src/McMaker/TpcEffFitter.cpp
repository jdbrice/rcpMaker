#include "McMaker/TpcEffFitter.h"
#include "Common.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TFile.h"

#include "Logger.h"
#include "Reporter.h"
#include "RooPlotLib.h"
using namespace jdb;


void CalcCholesky(int NP, double* fCov, double* fCovSqrt)
{
 double *C = fCovSqrt;
 double *V = fCov;
 // calculate sqrt(V) as lower diagonal matrix
 for( int i = 0; i < NP; ++i )
 {
   for( int j = 0; j < NP; ++j )
   {
     C[i*NP+j] = 0;
   }
 }
 for( int j = 0; j < NP; ++j )
 {
   // diagonal terms first
   double Ck = 0;
   for( int k = 0; k < j; ++k )
   {
     Ck += C[j*NP+k] * C[j*NP+k];
   } // k
   C[j*NP+j] = sqrt( fabs( V[j*NP+j] - Ck ) );

   // off-diagonal terms
   for( int i = j+1; i < NP; ++i )
   {
     Ck = 0;
     for( int k = 0; k < j; ++k )
     {
       Ck += C[i*NP+k] * C[j*NP+k];
     } //k
     if(C[j*NP+j]!=0 ) C[i*NP+j] = ( V[i*NP+j] - Ck ) / C[j*NP+j];
     else C[i*NP+j] = 0;
   }// i
 } // j
}

double RANDOM(double xx, TF1* F, int NP, double* fCovSqrt)
{

 double * z = new double[NP];
 double * x = new double[NP];
 double * p = new double[NP];

 for( int i = 0; i < NP; ++i )
 {
   z[i] = gRandom->Gaus( 0.0, 1.0 );
   p[i] = F->GetParameter(i);
 }

 for( int i = 0; i < NP; ++i )
 {
   x[i] = 0;
   for( int j = 0; j <= i; ++j )
   {
     x[i] += fCovSqrt[i*NP+j] * z[j];
   } // j
 }

 for( int i = 0; i < NP; ++i )
 {
   F->SetParameter(i,x[i]+p[i]);
 }
 double value = F->Eval(xx);
 for( int i = 0; i < NP; ++i )
 {
   F->SetParameter(i,p[i]);
 }

 delete [] z;
 delete [] x;
 delete [] p;
 return value;

}
















TpcEffFitter::TpcEffFitter( XmlConfig * _cfg, string _nodePath ){
	DEBUG( "( " << _cfg << ", " << _nodePath << " )" )
	cfg = _cfg;
	nodePath = _nodePath;
	outputPath = cfg->getString( nodePath + "output:path", "" );

	book = unique_ptr<HistoBook>( new HistoBook( outputPath + cfg->getString( nodePath +  "output.data", "TpcEff.root" ), cfg, "", "" ) );	
}



void TpcEffFitter::make(){
	DEBUG("")

	gStyle->SetOptFit( 111 );
	string params_file =  cfg->getString( nodePath + "output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}

	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;



	vector< int> cbins = cfg->getIntVector( nodePath + "CentralityBins" );
	Reporter rp( cfg, nodePath + "Reporter." );

	DEBUG( "Starting plc loop" )
	for ( string plc : Common::species ){
		if ( "E" == plc || "D" == plc )
			continue;
		for ( string c : Common::sCharges ){

			out << "\t<" << plc << "_" << c << ">" << endl;

			string fnMc = cfg->getString( nodePath + "input:url" ) + "TpcEff_" + plc + "_" + c + "_mc" + ".root";
			TFile * fmc = new TFile( fnMc.c_str(), "READ" );
			string fnRc = cfg->getString( nodePath + "input:url" ) + "TpcEff_" + plc + "_" + c + "_rc" + ".root";
			TFile * frc = new TFile( fnRc.c_str(), "READ" );

			DEBUG( "Mc File = " << fmc )
			DEBUG( "Rc File = " << frc )
			if ( !fmc->IsOpen() || !frc->IsOpen() )
				continue;
			

			// build an efficiency for each centrality
			for ( int b : cbins ){

				TH1 * hMc = (TH1*)fmc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );
				TH1 * hRc = (TH1*)frc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );

				INFO( tag, "N bins MC = " << hMc->GetNbinsX() );
				INFO( tag, "N bins RC = " << hRc->GetNbinsX() );

				for ( int i = 0; i <= hMc->GetNbinsX() + 1; i++ ){
					if ( hMc->GetBinContent( i ) < hRc->GetBinContent( i ) ){
						// set to 100%
						if ( i > 5 )
							hMc->SetBinContent( i, hRc->GetBinContent( i ) );
						else{
							hRc->SetBinContent( i, 0 );
							hMc->SetBinContent( i, 0 );
						}
					}
				}
					

				hMc->Sumw2();
				hRc->Sumw2();

				TGraphAsymmErrors g;

				g.SetName( (plc + "_" + c + "_" + ts(b)).c_str() );
				g.BayesDivide( hRc, hMc );

				book->add( plc + "_" + c + "_" + ts(b),  &g );

				// do the fit
				TF1 * fitFunc = new TF1( "effFitFunc", "[0] * exp( - pow( [1] / x, [2] ) )", 0.0, 5.0 );
				fitFunc->SetParameters( .85, 0.05, 5.0, -0.05 );
				fitFunc->SetParLimits( 0, 0.5, 1.0 );
				fitFunc->SetParLimits( 1, 0.0, 0.5 );
				fitFunc->SetParLimits( 2, 0.0, 10 );

				TFitResultPtr fitPointer = g.Fit( fitFunc, "BNRSWW" );



				// TMatrixDSym cov = fitPointer->GetCovarianceMatrix();
				// double *covArray = new double[9]; // number of parameters x number of parameters
				// covArray = cov.GetMatrixArray();
				// for ( int i = 0; i < 9; i++ ){
				// 	INFO( tag, "[" << i << "] = " << covArray[ i ] );
				// }

				// int NP = 3; //define number of parameters
				// double* fCov = new double[NP*NP];
				// fCov = cov.GetMatrixArray();
				// double* fCovSqrt = new double[NP*NP];
				// CalcCholesky(NP, fCov, fCovSqrt);

				// for ( int i = 0; i < 9; i++ ){
				// 	INFO( tag, "[" << i << "] = " << fCovSqrt[ i ] );
				// }

				// Double_t x[2000];
				// Double_t y[2000];
				// Double_t yup[2000];
				// Double_t ydown[2000];
				// Double_t yerr[2000];

				// fitFunc->SetParLimits( 0, 0, 0 );
				// fitFunc->SetParLimits( 1, 0.0, 0.0 );
				// fitFunc->SetParLimits( 2, 0.0, 0.0 );


				// int i = 0;
				// for (double xx = 0.1; xx < 4.8; xx+= 0.05) {
				//    x[i] = xx;
				//    TH1D *hDistributionAtX = new

				//    TH1D("hDistributionAtX","",200,fitFunc->Eval(x[i]) - .2,fitFunc->Eval(x[i]) + .2);
				//    for (int j=0; j<5000; j++) {
				//      hDistributionAtX->Fill(RANDOM(x[i],fitFunc,NP,fCovSqrt));
				//    }
				//    //hDistributionAtX->DrawCopy();
				   
				//    y[i] = fitFunc->Eval(x[i]);
				//    yup[i] = fitFunc->Eval(x[i]) + hDistributionAtX->GetRMS();
				//    yerr[i] = hDistributionAtX->GetRMS();
				//    ydown[i] = fitFunc->Eval(x[i]) - hDistributionAtX->GetRMS();

				//   // rp.savePage();
				//    hDistributionAtX->Delete();
				//    i++;
				//  } 


				 // TGraphErrors * band = new TGraphErrors( i - 1, x, y, 0, yerr );
				TGraphErrors * band = Common::choleskyBands( fitPointer, fitFunc, 5000, 200, &rp );

				RooPlotLib rpl;
				rp.newPage();
				rpl.style( &g ).set( "title", plc + "_" + c + " : centrality bin = " + ts( b ) ).set( "yr", 0, 1.1 ).set( "optfit", 111 )
					.set( "xr", 0, 5 )
					.set("y", "Efficiency").set( "x", "p_{T} [GeV/c]" ).draw();

					gStyle->SetStatY( 0.5 );
					gStyle->SetStatX( 0.85 );
					
					fitFunc->SetLineColor( kRed );
					fitFunc->Draw("same");	

				
				// TH1 * band = Common::fitCL( fitFunc, "bands", 0.99 );
				band->SetFillColorAlpha( kRed, 0.5 );
				band->Draw( "same e3" );


				rp.savePage();

				exportParams( b, fitFunc, out );

			} // loop centrality bins

			out << "\t</" << plc << "_" << c << ">" << endl;

		}
	}

	out << "</config>" << endl;
	out.close();


}

void TpcEffFitter::exportParams( int bin, TF1 * f, ofstream &out ){
	out << "\t\t<TpcEffParams bin=\"" << bin << "\" ";
	out << Common::toXml( f );
	out << "/>" << endl;
}