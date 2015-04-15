#include "SGFSchema.h"
#include "PidPhaseSpace.h"

SGFSchema::SGFSchema( XmlConfig * _cfg, string _np ){

	np = _np;
	cfg = _cfg;
	initialize();

	allData = nullptr;
}

SGFSchema::~SGFSchema(){

}


void SGFSchema::initialize() {
	makeVariables();
	makeModels();
	makeSimultaneous();

	for ( string plc : PidPhaseSpace::species ){
		defaultYield[ plc ] = cfg->getDouble( np + ".Yield:" + plc, 1000 );
	}


}

void SGFSchema::makeVariables(){
	l.info(__FUNCTION__)<< endl;

	vector<string> rrvPaths = cfg->childrenOf( np, "RooRealVar" );
	for ( string path : rrvPaths ){
		l.info(__FUNCTION__ ) << cfg->getString( path +":name" ) << endl;
		XmlRooRealVar *xrrv = new XmlRooRealVar( cfg, path );
		rrv[ xrrv->getName() ] = xrrv;
		//xrrv->rrv->Print();
	}

}

void SGFSchema::makeModels(){

	l.info(__FUNCTION__)<< endl;

	vector<string> paths = cfg->childrenOf( np, "Model" );
	for ( string path : paths ){

		l.info(__FUNCTION__ ) << path << endl;
		makeModel( path );

	}
}

void SGFSchema::makeModel( string mnp ){

	string modelName = cfg->getString( mnp + ":name" );
	string sample = cfg->getString( mnp + ":sample" );

	l.info(__FUNCTION__) << "Making Model : " << modelName << endl;

	string modelNameSplit = "";
	if ( "" != modelName )
		modelNameSplit = "_";

	RooArgList ralGauss; // RooGaussains
	RooArgList ralYield; // yields
	vector<string> paths = cfg->childrenOf( mnp, "RooGaussian" );
	for ( string path : paths ){

		string xName = cfg->getString( path + ":var" );
		string mName = cfg->getString( path + ":mu" );
		string sName = cfg->getString( path + ":sigma" );
		string yName = cfg->getString( path + ":yield" );

		/**
		 * Make sure the variables exist
		 */
		if ( nullptr == rrv[xName] ){
			l.error(__FUNCTION__) << xName << " DNE " << endl;
			continue;
		}
		if ( nullptr == rrv[mName] ){
			l.error(__FUNCTION__) << mName << " DNE " << endl;
			continue;
		}
		if ( nullptr == rrv[sName] ){
			l.error(__FUNCTION__) << sName << " DNE " << endl;
			continue;
		}
		if ( nullptr == rrv[yName] ){
			l.error(__FUNCTION__) << yName << " DNE " << endl;
			continue;
		}


		string name = modelName + modelNameSplit + cfg->getString( path + ":name" );
		l.info(__FUNCTION__ ) << "Making " << name << endl;

		// Create the RooGaussian
		RooGaussian * rg = new RooGaussian( name.c_str(), name.c_str(), 
								(*rrv[ xName ]->rrv), (*rrv[ mName ]->rrv), (*rrv[ sName ]->rrv) );

		ralGauss.add( (*rg) );
		ralYield.add( (*rrv[yName]->rrv) );
		gausses[ name ] = rg;
		gaussIndVar[ name ] = xName;
		//rg->Print();
	}

	// Make the RooAddPdf
	RooAddPdf * rap = new RooAddPdf( modelName.c_str(), modelName.c_str(), ralGauss, ralYield );
	models[ modelName ] = rap;
	modelSample[ modelName ] = sample;
	//rap->Print();
}


/**
 * Must be called after all samples and models are defined
 */
void SGFSchema::makeSimultaneous(){

	// make sample first
	rSample = new RooCategory( "sample", "sample" );
	for ( auto v : modelSample ){
		l.info(__FUNCTION__) << "Adding sample : " << v.second << endl;
		rSample->defineType( v.second.c_str() );
	}
	simModel = new RooSimultaneous( "simPdf", "simPdf", (*rSample) );

	for ( auto v : models ){
		simModel->addPdf( (*v.second), modelSample[ v.first ].c_str() );
	}
}

/**
 * Combines the data histograms into a RooDataHist
 * @dataHist 	map of histos: key=sample, value=histogram
 */
void SGFSchema::updateData( map<string, TH1D * > &dataHist, string var ) {

	for ( auto k : dataHist ){

		RooDataHist * tRDH = new RooDataHist( k.first.c_str(), k.first.c_str(), 
									RooArgSet( (*rrv[ var ]->rrv) ), 
									k.second );
		if ( nullptr != rdhSingle[ k.first ]  )
			delete rdhSingle[ k.first ];
		rdhSingle[ k.first ] = tRDH;
	}

}

void SGFSchema::combineData(){

	if ( nullptr != allData )
		delete allData;
	allData = new RooDataHist( "allData", "combined Data", RooArgSet( (*rrv[ "zb" ]->rrv), (*rrv[ "zd" ]->rrv) ),
								Index( *rSample ), Import( rdhSingle ) );


}


void SGFSchema::limitYield( string plc, string var1, string var2 ){
	
	double eyMax = 100.0;
	double yMax = 1.0;
	double yMin = 0.01;
	// also update the yield limits
	double cYield = var( "yield_" + plc )->getVal();
	var( "yield_" + plc )->setMax( cYield * yMax );
	var( "yield_" + plc )->setMin( cYield * yMin );

	for ( string ePlc : PidPhaseSpace::species ){

		cYield = var( var1 + "_" + ePlc + "_yield_" + plc )->getVal();
		
		var( var1 + "_" + ePlc + "_yield_" + plc )->setMax( cYield * eyMax );
		var( var1 + "_" + ePlc + "_yield_" + plc )->setMin( cYield * yMin );

		cYield = var( var2 + "_" + ePlc + "_yield_" + plc )->getVal();
		
		var( var2 + "_" + ePlc + "_yield_" + plc )->setMax( cYield * eyMax );
		var( var2 + "_" + ePlc + "_yield_" + plc )->setMin( cYield * yMin );
	}


}

void SGFSchema::setInitial( string sVar, string plc, double _mu, double _sigma, double _dmu, double _dsigma ){

	// variable name 
	const string sMu = "_mu_";
	const string sSigma = "_sigma_";
	string muName = sVar + sMu + plc;	// like zb_mu_Pi
	string sigmaName = sVar + sSigma + plc;

	rrv[ muName ]->rrv->setVal( _mu );
	rrv[ muName ]->rrv->setMin( _mu - _sigma * _dmu );
	rrv[ muName ]->rrv->setMax( _mu + _sigma * _dmu );
	
	rrv[ sigmaName ]->rrv->setVal( _sigma );
	rrv[ sigmaName ]->rrv->setMin( _sigma * ( 1.0 - _dsigma ) );
	rrv[ sigmaName ]->rrv->setMax( _sigma * ( 1.0 + _dsigma ) );

}


void SGFSchema::fixSigma( string var, string plc, double _sigma ){
	Logger::log.info( __FUNCTION__ ) << "Fixing " << plc << " sigma to " << _sigma << endl;

	const string sSigma = "_sigma_";
	string sigmaName = var + sSigma + plc;
	rrv[ sigmaName ]->rrv->setConstant( kFALSE );
	rrv[ sigmaName ]->rrv->setRange( _sigma - .01, _sigma + .01 ); // just ensure bounds aren't causing a problem
	rrv[ sigmaName ]->rrv->setVal( _sigma );	// set the value
	rrv[ sigmaName ]->rrv->setConstant( kTRUE );	// fix the sigma again
	
}

void SGFSchema::fixMu( string var, string plc, double _mu ){
	Logger::log.info( __FUNCTION__ ) << "Fixing " << plc << " mu to " << _mu << endl;

	const string sMu = "_mu_";
	string muName = var + sMu + plc;
	rrv[ muName ]->rrv->setConstant( kFALSE );
	rrv[ muName ]->rrv->setRange( _mu - .01, _mu + .01 ); // just ensure bounds aren't causing a problem
	rrv[ muName ]->rrv->setVal( _mu );	// set the value
	rrv[ muName ]->rrv->setConstant( kTRUE );	// fix the val again
	
}


void SGFSchema::setROI( string var, double low, double high ){

	

}

