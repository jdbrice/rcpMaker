#include "Correction/SpectraCorrecter.h"


SpectraCorrecter::SpectraCorrecter( XmlConfig * _cfg, string _nodePath ){

	cfg = _cfg;
	nodePath = _nodePath;

	INFO( "" )
	if ( cfg == nullptr )
		ERROR( "Invalid config" )
	else {
		setupCorrections();
		plc = cfg->getString( nodePath + "input:plc", "UNKNOWN" );
	}
}

SpectraCorrecter::~SpectraCorrecter(){

}

void SpectraCorrecter::setupCorrections(){
	INFO("")
	vector<string> cfgPaths = { "FeedDown", "TpcEff", "TofEff" };
	
	for ( string cPath : cfgPaths ){
		for ( string plc : Common::species ){
			for ( string c : Common::sCharges ){
				if ( cfg->exists( nodePath + cPath + "." + plc + "_" + c ) ){
					for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){
						string path = nodePath + cPath + "." + plc + "_" + c + "." + cPath + "Params[" + ts(cb) + "]";
						DEBUG( "Loading " << path )
						if ( cb != cfg->getInt( path + ":bin" ) )
							ERROR( "Centrality Bin Mismatch" )

						if ( "FeedDown" == cPath )
							feedDown[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigFunction>( new ConfigFunction( cfg, path ) );
						if ( "TpcEff" == cPath )
							tpcEff[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigFunction>( new ConfigFunction( cfg, path ) );
						if ( "TofEff" == cPath )
							tofEff[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigGraph>( new ConfigGraph( cfg, path ) );
					}
				} else {
					WARN( "Cannot find " + cPath + " params for " << plc + "_" + c )
				}

			} // c
		} // plc
	} // cPath
}


double SpectraCorrecter::tpcEffWeight( string plc, double pt, int iCen, int charge ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tpcEff.count( name ) )
		return 1.0 / tpcEff[ name ]->eval( pt );
	else
		ERROR( "Cannot find tpcEff correction for " << name )
	return 1.0;
}
double SpectraCorrecter::tofEffWeight( string plc, double pt, int iCen, int charge ){
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( tofEff.count( name ) )
		return 1.0 / tofEff[ name ]->eval( pt );
	else
		ERROR( "Cannot find tofEff correction for " << name )
	return 1.0;
}
double SpectraCorrecter::feedDownWeight( string plc, double pt, int iCen, int charge ){
	if ( "K" == plc ) // no feed down correction for K
		return 1.0;
	string name = plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen);
	if ( feedDown.count( name ) )
		return 1.0 - feedDown[ name ]->eval( pt );
	else
		ERROR( "Cannot find feedDown correction for " << name )
	return 1.0;
}