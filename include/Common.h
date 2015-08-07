#ifndef COMMON_H
#define COMMON_H

// ROOT
#include "TF1.h"

// ROOBARB
#include "Utils.h"
using namespace jdb;

// STL
#include <string>
#include <math.h>
#include <vector>
using namespace std;

class Common
{
public:
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
		return "dedx_tof_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(pBin);
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


	/* Converts a Root function (TF1 or subclass) to 
	 * a list of xml attributes
	 * @function	function to export as XML attributes
	 * @return 		string representing the function as XML attributes
	 */
	static string toXml( TF1 * f );



	/* Names used in the PID fitting steps
	 * @plc 	Particle Species
	 * @iCen 	Centrality bin
	 * @charge 	Particle charge
	 * @return 	A string containg the name for the given property
	 */
	static string yieldName( string plc, int iCen, int charge ){
		return "yield_" + plc + "_" + ts(iCen) + "_" + Common::chargeString( charge );
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


	
};


#endif