#include "Correction/SpectraCorrecter.h"


SpectraCorrecter::SpectraCorrecter( XmlConfig _config, string _nodePath ){

	config = _config;
	nodePath = config.basePath( _nodePath );

	DEBUG( classname(), "config=<" << config.getFilename() << ">, nodepath=<" << nodePath << ">" );
	
	setupCorrections();
}

SpectraCorrecter::~SpectraCorrecter(){

}

void SpectraCorrecter::setupCorrections(){
	DEBUG(classname(), "")
	vector<string> cfgPaths = { "FeedDown", "TpcEff", "TofEff" };
	
	for ( string cPath : cfgPaths ){
		for ( string plc : Common::species ){
			for ( string c : Common::sCharges ){

				string basepath = nodePath + "." + cPath + "." + plc + "_" + c;

				if ( config.exists( basepath ) ){
					
					for ( int cb : config.getIntVector( nodePath + ".CentralityBins" ) ){

						string path = nodePath + "." + cPath + "." + plc + "_" + c + "." + cPath + "Params[" + ts(cb) + "]";
						DEBUG( classname(), "Loading " << path );
						DEBUG( classname(), "for CenBin : " << config.getInt( path + ":bin" ) << " expect : " << cb );

						// check that node[0] contains centrality bin0! as it should
						if ( cb != config.getInt( path + ":bin" ) ){
							ERROR( classname(), "Centrality Bin Mismatch" );
						}

						string key = plc + "_" + c + "_" + ts(cb);

						if ( "FeedDown" == cPath )
							feedDown[ key ] = unique_ptr<XmlFunction>( new XmlFunction( &config, path ) );
						if ( "TpcEff" == cPath )
							tpcEff[ key ] = unique_ptr<XmlFunction>( new XmlFunction( &config, path ) );
						if ( "TofEff" == cPath )
							tofEff[ key ] = unique_ptr<XmlBinnedData>( new XmlBinnedData( config, path ) );

						TRACE( classname(), cPath << " Params for " << key);

					} // for cb

				} else {
					WARN( classname(), "Cannot find " + cPath + " params for " << plc + "_" + c );
				}

			} // c
		} // plc
	} // cPath
}


double SpectraCorrecter::tpcEffCorr( string plc, double pt, int iCen, int charge ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tpcEff.count( name ) ){
		return tpcEff[ name ]->eval( pt );
	}
	return 1.0;
}

double SpectraCorrecter::tpcEffWeight( string plc, double pt, int iCen, int charge, double sysNSigma ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tpcEff.count( name ) ){

		if ( sysNSigma != 0 ){
			double sys = Common::choleskyUncertainty( pt, tpcEff[ name ]->getCov().data(), tpcEff[ name ]->getTF1().get(), 500 );
			double val = tpcEff[ name ]->eval( pt );
			val += sys * sysNSigma;
		
			return 1.0 / val;
		}
		return 1.0 / tpcEff[ name ]->eval( pt );
	}
	else
		ERROR( classname(), "Cannot find tpcEff correction for " << name );
	return 1.0;
}

double SpectraCorrecter::tofEffCorr( string plc, double pt, int iCen, int charge ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tofEff.count( name ) ){
		return tofEff[ name ]->eval( pt );
	}
	return 1.0;
}

double SpectraCorrecter::tofEffWeight( string plc, double pt, int iCen, int charge, double sysNSigma ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tofEff.count( name ) )
		return 1.0 / tofEff[ name ]->eval( pt );
	else
		ERROR( classname(), "Cannot find tofEff correction for " << name );
	return 1.0;
}
double SpectraCorrecter::feedDownWeight( string plc, double pt, int iCen, int charge, double sysNSigma ){
	
	if ( "K" == plc ) // no feed down correction for K
		return 1.0;


	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( feedDown.count( name ) ){
		if ( sysNSigma != 0 ){
			double sys = Common::choleskyUncertainty( pt, feedDown[ name ]->getCov().data(), feedDown[ name ]->getTF1().get(), 500 );
			double val = feedDown[ name ]->eval( pt ) + sys * sysNSigma;
			DEBUG( classname(), "systematics on feedDown fd = " << val );
			return 1.0 - val;
		}
		return 1.0 - feedDown[ name ]->eval( pt );
	} else
		ERROR( classname(), "Cannot find feedDown correction for " << name );
	return 1.0;
}