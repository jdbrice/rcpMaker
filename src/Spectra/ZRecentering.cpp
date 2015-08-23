#include "Spectra/ZRecentering.h"

ZRecentering::ZRecentering( double dedxSigma, double tofSigma, string bTable, int bMethod ){
	this->dedxSigma = dedxSigma;
	this->tofSigma = tofSigma;

	dedxGen = new Bichsel( bTable, bMethod);
	tofGen = new TofGenerator();

	species = { "E", "Pi", "K", "P", "D" };

	// in GeV / c^2
	eMass 		= 0.000510998;
	piMass 		= 0.1395702;
	kaonMass 	= 0.493667;
	protonMass 	= 0.9382721;
	deuteronMass= 1.875612928 ; // from nist http://physics.nist.gov/cgi-bin/cuu/Value?mdc2mev
}
ZRecentering::~ZRecentering(){
	delete dedxGen;
	delete tofGen;
}

TofGenerator * ZRecentering::tofGenerator() { return tofGen; }
Bichsel * ZRecentering::dedxGenerator() { return dedxGen; }

double ZRecentering::mass( string pType ){
	if ( "D" == pType )
		return deuteronMass;
	if ( "P" == pType )
		return protonMass;
	if ( "K" == pType )
		return kaonMass;
	if ( "Pi" == pType )
		return piMass;
	if ( "E" == pType )
		return eMass;

	ERROR( " UNKOWN plc : " << pType )
	return -10.0;	
}
vector<string> ZRecentering::otherSpecies( string center ){
	vector<string> res;
	for ( string plc : species ){
		if ( plc != center )
			res.push_back( plc );
	}
	return res;
}
vector<string> ZRecentering::allSpecies(){
	return species;
}
vector<double> ZRecentering::centeredTofMeans( string center, double p, vector<string> others ){

	double cMean = tofGen->mean( p, mass( center ) );
	
	vector<double> res;
	for ( string plc : others ){
		double m = (tofGen->mean( p, mass( plc ) ) - cMean);
		res.push_back( m );
	}

	return res;
}
vector<double> ZRecentering::centeredTofMeans( string center, double p ){

	double cMean = tofGen->mean( p, mass( center ) );
	
	vector<double> res;
	for ( string plc : species ){
		double m = (tofGen->mean( p, mass( plc ) ) - cMean);
		res.push_back( m );
	}

	return res;
}
map<string, double> ZRecentering::centeredTofMap( string center, double p ){

	double cMean = tofGen->mean( p, mass( center ) );
	
	map<string, double> res;
	for ( string plc : species ){
		double m = (tofGen->mean( p, mass( plc ) ) - cMean);
		res[ plc ] = m;
	}

	return res;
}


vector<double> ZRecentering::centeredDedxMeans( string center, double p, vector<string> others ){
	
	const double cMean = dedxGen->meanLog( p, mass( center ), -1, 1000 );

	vector<double> res;
	for ( string plc : others ){
		double m = dedxGen->meanLog( p, mass( plc ), -1, 1000 ) - cMean;
		res.push_back( m );
	}

	return res;
}
vector<double> ZRecentering::centeredDedxMeans( string center, double p ){
	
	const double cMean = dedxGen->meanLog( p, mass( center ), -1, 1000 );
	
	vector<double> res;
	for ( string plc : species ){
		double m = dedxGen->meanLog( p, mass( plc ), -1, 1000 ) - cMean;
		res.push_back( m );
	}
	return res;
}

map<string, double> ZRecentering::centeredDedxMap( string center, double p ){
	
	const double cMean = dedxGen->meanLog( p, mass( center ), -1, 1000 );
	
	map<string, double> res;
	for ( string plc : species ){
		double m = dedxGen->meanLog( p, mass( plc ), -1, 1000 ) - cMean;
		res[ plc ] = m ;
	}
	return res;
}

double ZRecentering::rDedx( string centerSpecies, double dedx, double p ){ 

	double mean = dedxGen->meanLog( p, mass( centerSpecies ), -1, 1000 );
	double nDedx = log( dedx );
	double nSig = nDedx - mean ;

	return nSig;

	return -999.0;
}

double ZRecentering::nlDedx( string centerSpecies, double dedx, double p, double avgP ){

	double dedxLog = log( dedx );

	// mean for this species
	//double mu = dedxGen->meanLog( p, mass( centerSpecies ), -1, 1000 );
	const double muAvg = dedxGen->meanLog( avgP, mass( centerSpecies ), -1, 1000 );

	double n1 = 0;
	double d1 = 0;

	for ( string plc : species ){

		const double iMu = dedxGen->meanLog( p, mass( plc ), -1, 1000 );
		const double iMuAvg = dedxGen->meanLog( avgP, mass( plc ), -1, 1000 );
		
		// may change to a functional dependance 
		double sigma = dedxSigma; 

		double iL = lh( dedxLog, iMu, sigma );
		double w = dedxLog + iMuAvg - iMu;
		

		n1 += (iL * w);
		d1 += iL;

	}

	if ( 0 == n1 || 0 == d1){
		return dedxLog - muAvg;
	}

	double nDedx = (n1/d1) - (muAvg);

	return nDedx;

}




double ZRecentering::rTof( string centerSpecies, double beta, double p  ){

	double deltaInvBeta = ( 1.0 / beta ) - tofGen->mean( p, mass( centerSpecies ) );

	return deltaInvBeta;
}

double ZRecentering::nlTof( string centerSpecies, double beta, double p, double avgP ){

	const double sigma = tofSigma; 
	const double tof = 1.0 / beta;
	
	// mean for this species
	//const double mu =  tofGen->mean( p, mass( centerSpecies ) );
	const double muAvg =  tofGen->mean( avgP, mass( centerSpecies ) );
	
	double n1 = 0;
	double d1 = 0;

	for ( string plc : species ){
		
		double iMu =  tofGen->mean( p, mass( plc ) ) ;
		double iMuAvg =  tofGen->mean( avgP, mass( plc ) ) ;
		
		
		double iL = lh( tof, iMu, sigma );
		
		double w = tof + iMuAvg - iMu;
		
		n1 += (iL * w);
		d1 += iL;

	}

	if ( 0 == n1 || 0 == d1){
		return tof - muAvg;
	}
	
	double nTof = (n1/d1) - ( muAvg );
	
	return nTof;

}

double ZRecentering::nl2Tof( string centerSpecies, double beta, double p, double avgP ){

	const double sigma = tofSigma; 
	const double tof = 1.0 / beta;
	

	if ( 0 >= beta ){
		return -100;
	}
	// mean for this species
	//const double mu =  tofGen->mean( p, mass( centerSpecies ) );
	const double muAvg =  tofGen->mean( avgP, mass( centerSpecies ) );
	const double mu =  tofGen->mean( p, mass( centerSpecies ) );
	
	double n1 = 0;
	double d1 = 0;
	double n2 = 0;
	double d2 = 0;

	for ( string plc : species ){
		
		double iMu =  tofGen->mean( p, mass( plc ) ) ;
		double iMuAvg =  tofGen->mean( avgP, mass( plc ) ) ;
		
		double iL = lh( tof, iMu, sigma );
		double iL2 = lh( mu, iMu, sigma );
		
		double w = tof + iMuAvg - iMu;
		
		n1 += (iL * w);
		d1 += iL;

		n2 += ( iL2 * w );
		d2 += iL2;

	}

	double p1 = 0;
	double p2 = 0;
	
	if ( d1 != 0 )
		p1 = n1 / d1;
	// else 
	// 	ERROR("oh no1 : " << tof)
	
	if ( d2 != 0 )
		p2 = n2 / d2;
	else
		ERROR("oh no2")

	double nTof = p1 - ( tof + muAvg - mu  );
	
	return nTof + tof;

}


double ZRecentering::lh( double x, double mu, double sigma ){

	double a = sigma * sqrt( 2 * TMath::Pi() );
	double b = ( x - mu );
	double c = 2 * sigma*sigma;
	double d = (1/a) * exp( -b*b / c );

	return d;
}

double ZRecentering::lh2( double x, double mu, double sigma ){


	double b = ( x - mu );
	double c = 9 * sigma*sigma;
	double d = exp( -b*b / c ) * 1e9;

	return d;
}