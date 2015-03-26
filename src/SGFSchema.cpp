#include "SGFSchema.h"


SGFSchema::SGFSchema( XmlConfig * _cfg, string _np ){

	np = _np;
	cfg = _cfg;
	initialize();
}

SGFSchema::~SGFSchema(){

}


void SGFSchema::initialize() {
	makeVariables();
	makeModels();
	makeSimultaneous();
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

	//if ( nullptr != allData )
	//	delete allData;
	allData = new RooDataHist( "allData", "combined Data", RooArgSet( (*rrv[ "zb" ]->rrv), (*rrv[ "zd" ]->rrv) ),
								Index( *rSample ), Import( rdhSingle ) );


}


void SGFSchema::forceROI(){

	for ( auto p : gaussIndVar ){
		// first 	= gauss model name
		// second 	= indep var name 

	}



}

