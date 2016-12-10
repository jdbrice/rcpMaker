#ifndef RCP_HISTO_FILE_MAKER_H
#define RCP_HISTO_FILE_MAKER_H

class RcpHistoFileMaker : public HistoAnalyzer
{
public:
	virtual const char* classname() const{ return "RcpHistoFileMaker"; }
	RcpHistoFileMaker() {}
	~RcpHistoFileMaker() {}

	virtual void initialize(){
		HistoAnalyzer::initialize();
	}

	virtual void make(){

		book->cd();
		string cen = config.getString( "cen" );
		string per = config.getString( "per" );

		for ( string energy : {"7.7", "11.5", "14.5", "19.6", "27.0", "39.0", "62.4"} ){
			for ( string plc : { "Pi", "K", "P" } ){
				for ( string charge : { "p", "n" } ){
					makeRcp( energy, plc, charge, cen, per );
				}
			}
		}


	}

	string yieldName( string _energy, string _plc, string _charge, string _cen, string _err ){
		_energy.replace( _energy.find("."), 1, "p" );
		return "Yield_" + _energy + "_" + _plc + "_" + _charge + "_" + _cen + "_" + _err + "_normed";
	}

	string rcpName( string _energy, string _plc, string _charge, string _cen, string _per, string _err ){
		_energy.replace( _energy.find("."), 1, "p" );
		return "Rcp_" + _energy + "_" + _plc + "_" + _charge + "_" + _cen + "_over_" + _per + "_" + _err;
	}

	double nCollFor( string _energy, string _cen ){
		DEBUG( classname(), "( _energy = " << _energy << ", _cen = " << _cen << " )" );
		_energy.replace( _energy.find("."), 1, "p" );
		vector<double> nColls = config.getDoubleVector( "nColl.Energy_" + _energy );

		if ( abs(atoi( _cen.c_str() )) > 6 ){
			ERROR( classname(), "Invalid centrality" );
			return 1.0;
		}
		double nColl = nColls[ atoi( _cen.c_str() ) ];
		return nColl;
	}

	double nCollSysFor( string _energy, string _cen ){
		DEBUG( classname(), "( _energy = " << _energy << ", _cen = " << _cen << " )" );
		_energy.replace( _energy.find("."), 1, "p" );
		vector<double> nColls = config.getDoubleVector( "nCollSys.Energy_" + _energy );

		if ( abs(atoi( _cen.c_str() )) > 6 ){
			ERROR( classname(), "Invalid centrality" );
			return 1.0;
		}
		double nColl = nColls[ atoi( _cen.c_str() ) ];
		return nColl;
	}



	virtual void makeRcp( string _energy, string _plc, string _charge, string _cen, string _per ){
		DEBUG( classname(), rcpName( _energy, _plc, _charge, _cen, _per, "" ) );
		
		TH1 * cen_stat    = get1D( yieldName( _energy, _plc, _charge, _cen, "stat" ) );
		TH1 * cen_sys     = get1D( yieldName( _energy, _plc, _charge, _cen, "sys" ) );
		TH1 * per_stat    = get1D( yieldName( _energy, _plc, _charge, _per, "stat" ) );
		TH1 * per_sys     = get1D( yieldName( _energy, _plc, _charge, _per, "sys" ) );

		INFO( classname(), "cen_stat " << cen_stat );
		INFO( classname(), "cen_sys " << cen_sys );
		INFO( classname(), "per_stat " << per_stat );
		INFO( classname(), "per_sys " << per_sys );

		
		string n_rcp_stat = rcpName( _energy, _plc, _charge, _cen, _per, "stat" );
		string n_rcp_sys  = rcpName( _energy, _plc, _charge, _cen, _per, "sys" );
		book->addClone( n_rcp_stat, cen_stat );
		book->addClone( n_rcp_sys, cen_sys );



		TH1 * rcp_stat    = book->get( n_rcp_stat );
		TH1 * rcp_sys     = book->get( n_rcp_sys );

		INFO( classname(), "rcp_stat " << rcp_stat );
		INFO( classname(), "rcp_sys " << rcp_sys );

		rcp_stat->Divide( per_stat );
		rcp_sys->Divide( per_sys );

		INFO( classname(), "Divided" );

		double nCollFactor = nCollFor( _energy, _per ) / nCollFor( _energy, _cen );
		rcp_stat->Scale( nCollFactor );
		rcp_sys->Scale( nCollFactor );

		INFO( classname(), "Completed RCP" );

		if ( "62.4" == _energy && "Pi" == _plc && "p" == _charge ){
			book->addClone( "nColl_sys", cen_sys );
			book->get( "nColl_sys" )->Reset();

			double sys = nCollSysFor( _energy, _cen ) / nCollFor( _energy, _cen ) + nCollSysFor( _energy, _per ) / nCollFor( _energy, _per );
			book->setBin( "nColl_sys", config.getInt(nodePath + ".nCollSysBin", 27 ), 1.0, sys );
		}

	}
	
};


#endif	