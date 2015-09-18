#ifndef COMMON_H
#define COMMON_H

// ROOT
#include "TF1.h"
#include "TH1.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMatrixDSym.h"
#include "TFitResult.h"
#include "TRandom.h"

// ROOBARB
#include "Utils.h"
#include "Logger.h"
#include "Reporter.h"
using namespace jdb;

// STL
#include <string>
#include <math.h>
#include <vector>
#include <numeric>
using namespace std;

class Common
{
public:
	static constexpr auto tag = "Common";
	Common();
	~Common();

	// List of species we consider
	static vector<string> species;
	static vector<int> charges;
	static vector<string> sCharges;

	// Pion Mass [GeV/c] = 0.1395702
	static constexpr double 		mass_pi 		= 0.1395702;
	// Kaon Mass [GeV/c] = 0.493667
	static constexpr double 		mass_kaon	 	= 0.493667;
	// Proton Mass [GeV/c] = 0.9382721
	static constexpr double 		mass_proton 	= 0.9382721;

	/**
	 * Gets the rapidity from pt, eta, mass
	 * @param  pt  transverse momentum
	 * @param  eta pseudorapidity
	 * @param  m   mass
	 * @return     rapidity
	 */
	static double rapidity( double pt, double eta, double m = 0.13957018 ){
		double a = sqrt( m*m + pt*pt*cosh( eta )*cosh( eta ) ) + pt * sinh( eta );
		double b = sqrt( m*m + pt*pt );
		return log( a / b );
	}

	/* Get the string part of the name based on charge
	 * 
	 * @charge charge as -1, 0, +1
	 * @return        charge part of the name string
	 */
	static string chargeString( int charge = 0 ) {
		if ( -1 >= charge )	// negative
			return "n";
		else if ( 1 <= charge ) //positive
			return "p";
		return "a";	// all
	}


	static double mass( string pType ){
		if ( "P" == pType )
			return mass_proton;
		if ( "K" == pType )
			return mass_kaon;
		if ( "Pi" == pType )
			return mass_pi;
		return -10.0;	
	}



	/* Builds the string for a species histogram
	 * 
	 * @centerSpecies the centering species
	 * @charge        the charge, -1, 0, +1
	 * @return        string for histogram name
	 */
	static string speciesName( string centerSpecies, int charge = 0, int cenBin = 9 ){
		return "dedx_tof_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin);
	}
	static string speciesName( string centerSpecies, int charge, int cenBin, int pBin ){
		return centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(pBin);
	}
	static string zbName( string centerSpecies, int charge, int cenBin, int pBin, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "zb_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(pBin) + ePart;
	}
	static string zdName( string centerSpecies, int charge, int cenBin, int pBin, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "zd_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(pBin) + ePart;
	}

	/* Momentum 
	 * @pt 		Transverse mmtm [GeV/c]
	 * @eta 	PseudoRapidity 
	 * @return  Momentum [GeV/c]
	 */
	static double p( double pt, double eta ){
		return pt * cosh( eta );
	}


	static string toString( vector<double> vals ){
		stringstream sstr;

		int i = 0; 
		for ( auto v : vals ){
			if ( i < vals.size() - 1 ) 
				sstr << v << ", ";
			else 
				sstr << v;
			i++;
		}
		return sstr.str();
	}

	/* Converts a Root function (TF1 or subclass) to 
	 * a list of xml attributes
	 * @function	function to export as XML attributes
	 * @return 		string representing the function as XML attributes
	 */
	static string toXml( TF1 * f );
	static string toXml( TF1 * f, TFitResultPtr result );

	/* Converts a Root histogram (TH1 or subclass) to 
	 * an xml data node
	 * @h 			histogram to export as XML attributes
	 * @return 		string representing the histogram as XML attributes
	 */
	static string toXml( TH1 * h );

	/* Converts a Root Graph (TGraph or subclass) to 
	 * an xml data node
	 * @g 			Graph to export as XML attributes
	 * @return 		string representing the Graph as XML attributes
	 */
	static string toXml( TGraph * g, string linePrefix ="" );

	/* Converts a Root Graph with symmetric errors (TGraph or subclass) to 
	 * an xml data node
	 * @g 			Graph to export as XML attributes
	 * @return 		string representing the Graph as XML attributes
	 */
	static string toXml( TGraphErrors * g );

	/* Converts a Root Graph with asymmetric errors (TGraphAsymmErrors or subclass) to 
	 * an xml data node
	 * @g 			Graph to export as XML attributes
	 * @return 		string representing the Graph as XML attributes
	 */
	static string toXml( TGraphAsymmErrors * g, string linePrefix ="" );



	/* Names used in the PID fitting steps
	 * @plc 	Particle Species
	 * @iCen 	Centrality bin
	 * @charge 	Particle charge
	 * @return 	A string containg the name for the given property
	 */
	static string yieldName( string plc, int iCen, int charge, string plc2 = "" ){
		if ( "" != plc2 )
			plc2 = "_" + plc2;

		return "yield_" + plc + "_" + ts(iCen) + "_" + Common::chargeString( charge ) + plc2;
	}
	static string sigmaName( string plc, int iCen, int charge ){
		return "sigma_" + plc + "_" + ts(iCen) + "_" + Common::chargeString( charge );
	}
	static string muName( string plc, int iCen, int charge ){
		return "mu_" + plc + "_" + ts(iCen) + "_" + Common::chargeString( charge );
	}
	static string fitName( int iPt, int iCen, int charge ){
		return "fit_" + ts(iPt) + "_" + ts(iCen) + "_" + Common::chargeString( charge );
	}
	static string effName( string plc, int iCen, int charge ){
		return "eff_" + plc + "_" + ts(iCen) + "_" + Common::chargeString( charge );
	}

	/* Draws the given function with its 95% confidence level
	 * and returns the histogram containing the confidence level region
	 * @f 		Function to draw
	 */
	static TH1 * fitCL( TF1 * f, string name, double cl = 0.95, int nPoints = 100, double x1 = -1.0, double x2 = -1.0 );


	// TODO: template instead of  double
	static double mean( std::vector<double> &v ){
		double sum = std::accumulate(std::begin(v), std::end(v), 0.0);
		double m =  sum / v.size();
		return m;
	}

	// TODO: template instead of  double
	static double stdev( std::vector<double> &v ){
		
		if ( v.size() < 1 )
			return 0.0;

		double m = mean( v );

		double accum = 0.0;
		std::for_each (std::begin(v), std::end(v), [&](const double d) {
		    accum += (d - m) * (d - m);
		});

		return sqrt( accum / v.size() );
	}

	/* Computes the Cholesky parameters for the given function
	 *
	 * @nP 			Number of parameters
	 * @fCov 		Covariance matrix - passed in
	 * @fCovSqrt	Sqrt(Cov) matrix - passed out
	 *
	 */
	static void calcCholesky( int nP, double * fCov, double* fCovSqrt );
	
	/* Calculates random variations in a function from the sqrt of the cov matrix
	 *
	 * @xx 			x value at which to evaluate function
	 * @f 			TF1 * to function
	 * @nP 			number of parameters
	 * @fCovSqrt 	sqrt(cov) given by calcCholesky
	 *
	 * @return 		function evaluated within a random gaussian distribution at the given point
	 */
	static double randomSqrtCov( double xx, TF1 * f, int nP, double * fCovSqrt );

	static double choleskyUncertainty( double xx, TFitResultPtr fitResult, TF1 * f, int nSamples );
	static double choleskyUncertainty( double xx, double * fCov, TF1 * f, int nSamples );

	static TGraphErrors *choleskyBands( TFitResultPtr fitResult, TF1 * f, int nSamples = 50, int nPoints = 100, Reporter * rp = nullptr, double x1 = -1.0, double x2 = -1.0 );


	static string plc_label( string plc, string charge = "na" ){
		if ( "Pi" == plc && "p" == charge )
			return "#pi^{+}";
		if ( "Pi" == plc && "n" == charge )
			return "#pi^{-}";
		if ( "K" == plc && "p" == charge )
			return "K^{+}";
		if ( "K" == plc && "n" == charge )
			return "K^{-}";
		if ( "P" == plc && "p" == charge )
			return "p";
		if ( "P" == plc && "n" == charge )
			return "#bar{p}";

		if ( "Pi" == plc )
			return "#pi";
		if ( "K" == plc )
			return "K";
		if ( "P" == plc )
			return "p";

		return "";
	}



};


#endif