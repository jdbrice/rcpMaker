
#ifndef DEDX_BICHSEL_H
#define DEDX_BICHSEL_H

// RooBarb
#include "Logger.h"
using namespace jdb;

#include <string>
#include <math.h>
#include "TH1D.h"
#include "TFile.h"


using namespace std;

class Bichsel
{
protected:
	string tableFile;
	int method;
	TFile* table;

	TH1D *hP, *hPi, *hK, *hE, *hD;


	double eMass;
	double piMass;
	double kaonMass;
	double protonMass;
	double deuteronMass;

public:
	Bichsel( string tableFile = "dedxBichsel.root", int method = 0 ){

		eMass = 0.000510998;
		piMass = 0.1395702;
		kaonMass = 0.493667;
		protonMass = 0.9382721;
		deuteronMass= 1.875612928;

		this->tableFile = tableFile;
		this->method = method;
		table = new TFile( tableFile.c_str(), "READ" );

		getTables();

	}
	~Bichsel(){

	}

	double mean( double p, double m, int method = -1 ){

		if ( method >= 0 ){
			getTables();
		}

		// use the mass to determine the particle species to use
		// This is to maintain the same usage 
		TH1D * h = tableFor( m );

		if ( h ){
			
			int bin = h->GetXaxis()->FindBin( p );
			return ( h->GetBinContent( bin ) );

		} else 
			ERROR( " error no table found " )

		return -999.999;
	}

	/**
	 * Returns the Log10( mean(...) )
	 * @param  p      momentum
	 * @param  m      mass
	 * @param  method method for calculating dedx
	 * @return        Log10( mean )
	 */
	double mean10( double p, double m, int method = -1, double scale = 1 ){
		return log10( mean(p, m, method ) * scale );
	}

	double meanLog( double p, double m, int method = -1, double scale = 1 ){
		return log( mean(p, m, method ) * scale );
	}

	double getFromTable( string plc, double p );
	
	void getTables(){

		if ( 1 == method  ){
			hD = (TH1D*)table->Get( "t70D" );
			hP = (TH1D*)table->Get( "t70P" );
			hK = (TH1D*)table->Get( "t70K" );
			hPi = (TH1D*)table->Get( "t70Pi" );
			hE = (TH1D*)table->Get( "t70E" );
		} else if ( 2 == method ){
			hD = (TH1D*)table->Get( "polD" );
			hP = (TH1D*)table->Get( "polP" );
			hK = (TH1D*)table->Get( "polK" );
			hPi = (TH1D*)table->Get( "polPi" );
			hE = (TH1D*)table->Get( "polE" );
		} else {
			hD = (TH1D*)table->Get( "mpmD" );
			hP = (TH1D*)table->Get( "mpmP" );
			hK = (TH1D*)table->Get( "mpmK" );
			hPi = (TH1D*)table->Get( "mpmPi" );
			hE = (TH1D*)table->Get( "mpmE" );
		}
	}

	TH1D* tableFor( double mass ){
		double epsilon = 0.01;		
		if ( abs(mass - piMass) < epsilon  ){
			return hPi;
		} else if ( abs(mass - kaonMass) < epsilon  ){
			return hK;
		} else if ( abs(mass - protonMass) < epsilon  ){
			return hP;
		} else if ( abs(mass - eMass) < epsilon  ){
			return hE;
		} else if ( abs(mass - deuteronMass) < epsilon  ){
			return hD;
		}

		return hP;
	}

	static const int MPM = 0;
	static const int T70 = 1;
	static const int POL = 2;
	
};

#endif