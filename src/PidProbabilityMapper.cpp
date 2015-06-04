#include "PidProbabilityMapper.h"
#include "PidPhaseSpace.h"
#include "TSF/FitRunner.h"
#include "TH1D.h"

PidProbabilityMapper::PidProbabilityMapper( XmlConfig * _cfg, string _nodePath ) : HistoAnalyzer( _cfg, _nodePath ){

	logger->setClassSpace( "PidProbabilityMapper" );


	// set up the binning
	binsPt 		= unique_ptr<HistoBins>(	new HistoBins( cfg, nodePath + "PSBinning.pt" ) 			);
	binsEta 	= unique_ptr<HistoBins>(	new HistoBins( cfg, nodePath + "PSBinning.eta" ) 		);
	binsCharge 	= unique_ptr<HistoBins>(	new HistoBins( cfg, nodePath + "PSBinning.charge" ) 		);

}

PidProbabilityMapper::~PidProbabilityMapper(){


}

double PidProbabilityMapper::evaluateGauss( double x, double y, double m, double s ){
	return (y / (s *TMath::Sqrt( 2 * 3.1415926 ) )) * TMath::Exp(  -(x - m)*( x - m ) / ( 2 * s*s )  );
}

map<string, double> PidProbabilityMapper::pidWeights( int charge, int iCen, double pt, double eta, double zb, double zd ){

	map< string, double > weights;
	if (  TMath::Abs(charge) != 1 ){
		logger->error( __FUNCTION__ ) << "charge zero plc" << endl;
		return weights;
	}
	

	logger->debug( __FUNCTION__ ) << endl;

	int iPt = binsPt->findBin( pt );
	int iEta = binsEta->findBin( TMath::Abs(eta) );

	if ( iPt < 0 || iEta < 0  )
		return weights;

	logger->debug( __FUNCTION__ ) << "pt Bin : " << iPt << ", eta Bin : " << iEta << endl;

	vector<double> sigmas;
	vector<double> mus;
	vector<double> yields;

	string model = "zb";

	double eVal = zb;

	//if ( "zd" == model )
	//	eVal = zb;

	for ( string plc : PidPhaseSpace::species ){
		/*string name = TSF::FitRunner::sigmaName(plc, iCen, charge, iEta);
		string fName = plc + "_" + model + "Sigma/" + name;
		TH1D * h = ( TH1D * )inFile->Get( fName.c_str()   );
		
		sigmas.push_back( h->GetBinContent( iPt ) );

		name = TSF::FitRunner::muName(plc, iCen, charge, iEta);
		fName = plc + "_" + model + "Mu/" + name;
		h = ( TH1D * )inFile->Get( fName.c_str()   );
		mus.push_back( h->GetBinContent( iPt ) );
*/
		string name = TSF::FitRunner::yieldName(plc, iCen, charge, iEta);
		string fName = plc + "_yield/" + name;
		TH1D * h = ( TH1D * )inFile->Get( fName.c_str()   );
		if ( !h ){
			//logger->error( __FUNCTION__ ) << fName << " not found " << endl;
			return weights;
		}
		yields.push_back( h->GetBinContent( iPt ) );
	}

	logger->debug( __FUNCTION__ ) << "parameters loaded" << endl;

	
	double total = 0.0;

	
	int i = 0;
	for ( string plc : PidPhaseSpace::species ){

		
		total += yields[ i ];
		weights[ plc ] = yields[ i ];
		
		i++;
	}

	// normalize
	for ( string plc : PidPhaseSpace::species ){
		if ( total > 0 )
			weights[ plc ] = weights[ plc ] / total;
		else 
			weights[ plc ] = 0;
		logger->debug(__FUNCTION__) << "weight[ " << plc << " ] = " << weights[ plc ] << endl;
	}
	
	return weights;

}

/*
map<string, double> PidProbabilityMapper::pidWeights0( int charge, int iCen, double pt, double eta, double zb, double zd ){

	map< string, double > weights;
	if (  TMath::Abs(charge) != 1 ){
		logger->error( __FUNCTION__ ) << "charge zero plc" << endl;
		return weights;
	}
	

	logger->debug( __FUNCTION__ ) << endl;

	int iPt = binsPt->findBin( pt );
	int iEta = binsEta->findBin( TMath::Abs(eta) );

	if ( iPt < 0 || iEta < 0  )
		return weights;

	logger->debug( __FUNCTION__ ) << "pt Bin : " << iPt << ", eta Bin : " << iEta << endl;

	vector<double> yields;

	for ( string plc : PidPhaseSpace::species ){
		string name = TSF::FitRunner::yieldName(plc, iCen, charge, iEta);
		string fName = plc + "_yield/" + name;
		TH1D * h = ( TH1D * )inFile->Get( fName.c_str()   );
		if ( !h )
			return weights;
		yields.push_back( h->GetBinContent( iPt+1 ) );
	}
	
	double total = 0.0;

	i = 0;
	for ( string plc : PidPhaseSpace::species ){
		total += yields[ i ];
		weights[ plc ] = yields[ i ];

		i++;
	}

	// normalize
	for ( string plc : PidPhaseSpace::species ){
		if ( total > 0 )
			weights[ plc ] = weights[ plc ] / total;
		else 
			weights[ plc ] = 0;
	}
	
	return weights;

}


map<string, double> PidProbabilityMapper::pidWeights( int charge, int iCen, double pt, double eta, double zb, double zd ){

	map< string, double > weights;
	if (  TMath::Abs(charge) != 1 ){
		logger->error( __FUNCTION__ ) << "charge zero plc" << endl;
		return weights;
	}
	

	logger->debug( __FUNCTION__ ) << endl;

	int iPt = binsPt->findBin( pt );
	int iEta = binsEta->findBin( TMath::Abs(eta) );

	if ( iPt < 0 || iEta < 0  )
		return weights;

	logger->debug( __FUNCTION__ ) << "pt Bin : " << iPt << ", eta Bin : " << iEta << endl;

	vector<double> sigmas;
	vector<double> mus;
	vector<double> yields;

	string model = "zb";

	double eVal = zd;

	if ( "zd" == model )
		eVal = zd;

	for ( string plc : PidPhaseSpace::species ){
		string name = TSF::FitRunner::sigmaName(plc, iCen, charge, iEta);
		string fName = plc + "_" + model + "Sigma/" + name;
		TH1D * h = ( TH1D * )inFile->Get( fName.c_str()   );
		if ( !h ){
			//logger->error( __FUNCTION__ ) << fName << " not found " << endl;
			return weights;
		}
		sigmas.push_back( h->GetBinContent( iPt ) );

		name = TSF::FitRunner::muName(plc, iCen, charge, iEta);
		fName = plc + "_" + model + "Mu/" + name;
		h = ( TH1D * )inFile->Get( fName.c_str()   );
		mus.push_back( h->GetBinContent( iPt ) );

		name = TSF::FitRunner::yieldName(plc, iCen, charge, iEta);
		fName = plc + "_yield/" + name;
		h = ( TH1D * )inFile->Get( fName.c_str()   );
		yields.push_back( h->GetBinContent( iPt ) );
	}

	logger->debug( __FUNCTION__ ) << "parameters loaded" << endl;

	
	double total = 0.0;

	bool near = false;
	int i = 0;
	for ( string plc : PidPhaseSpace::species ){
		if ( (eVal - mus[ i ]) / sigmas[ i ] < 3 ){
			near = true;
			break;
		}
		i++;
	}

	i = 0;
	for ( string plc : PidPhaseSpace::species ){

		if ( near ){
			double v = evaluateGauss( eVal, yields[ i ], mus[ i ], sigmas[ i ] );
			total += v;
			weights[ plc ] = v;
		} else {
			total += yields[ i ];
			weights[ plc ] = yields[ i ];
		}
		i++;
	}

	// normalize
	for ( string plc : PidPhaseSpace::species ){
		if ( total > 0 )
			weights[ plc ] = weights[ plc ] / total;
		else 
			weights[ plc ] = 0;
		logger->debug(__FUNCTION__) << "weight[ " << plc << " ] = " << weights[ plc ] << endl;
	}
	
	return weights;

}*/
