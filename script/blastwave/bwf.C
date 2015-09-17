#ifndef BLAST_WAVE_FIT
#define BLAST_WAVE_FIT

#include "SpectraLoader.h"
#include "Utils.h"
#include "Logger.h"


#include <sys/stat.h>

#include "RooPlotLib.h"

bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string file_name( string energy, string plc, string charge, string iCen ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/";
	return base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + iCen + ".dat";
}

// Integral evaluation step sizes
double R = 1.0;
double dr = 1e-2;

double bw_integral(double beta_T, double T, double n, double pt, double mt)
{
  
	double s = 0.0;
	for( double r = dr/2; r < R; r += dr){
		double beta_r = (beta_T*(n+2)/2) * TMath::Power((r/R),n);
		double rho = TMath::ATanH(beta_r);

		s += r * dr * TMath::BesselK1( (mt*cosh(rho)) / T ) * TMath::BesselI0( ( pt * sinh( rho ) ) / T );
	}

	return s;
}

double fcn(int &npar, double *gin, double &f, double *par, int flag){

}

vector<double> dx, dy, dye;

void bwf( string plc="Pi", string charge="p", string cen = "0", energy="14.5" ){


	SpectraLoader sl( file_name( energy, plc, charge, cen ) );

	dx = sl.pt;
	dy = sl.value;
	dye = sl.stat;	

	





}


#endif