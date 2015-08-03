#ifndef COMMON_H
#define COMMON_H


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
	static string speciesName( string centerSpecies, int charge, int cenBin, int ptBin ){
		return "dedx_tof_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(ptBin);
	}
	static string zbName( string centerSpecies, int charge, int cenBin, int ptBin, int etaBin = 0, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "zb_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(ptBin) + ePart;
	}
	static string zdName( string centerSpecies, int charge, int cenBin, int ptBin, int etaBin = 0, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "zd_" + centerSpecies + "_" + Common::chargeString(charge) + "_" + ts(cenBin) + "_" + ts(ptBin) + ePart;
	}

	/* Momentum 
	 * @pt 		Transverse mmtm [GeV/c]
	 * @eta 	PseudoRapidity 
	 * @return  Momentum [GeV/c]
	 */
	static double p( double pt, double eta ){
		return pt * cosh( eta );
	}
	
};


#endif