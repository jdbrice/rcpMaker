#ifndef EFF_PARAMS_H
#define EFF_PARAMS_H
// STL
#include <memory>
#include <utility>

// ROOBARB
#include "XmlConfig.h"
using namespace jdb;

//ROOT
#include "TF1.h"



class EffParams
{
protected:
	shared_ptr<XmlConfig> cfg;
	string nodePath;

	vector<TF1*> eff;
	vector< pair<int, int> > cenLimits;
	vector< pair<float, float> > yLimits;
	string name;

public:
	EffParams(string _name, shared_ptr<XmlConfig> _cfg, string _nodePath ){
		cout << "[EffParams." << __FUNCTION__ << "] Loading From " << _nodePath << endl;
		cfg = _cfg;
		nodePath = _nodePath;
		name = _name;

		// load yourself in
		load();
	}
	~EffParams(){

		for ( TF1 * k : eff ){
			delete k;
		}

	}

	void load(){

		vector<string> kids = cfg->childrenOf( nodePath, "EffParams" );

		for ( string k : kids ){
			int cl = cfg->getInt( k +":cl" );
			int ch = cfg->getInt( k +":ch" );
			cenLimits.push_back( make_pair( cl, ch ) );

			double yl = cfg->getDouble( k +":yl" );
			double yh = cfg->getDouble( k +":yh" );
			yLimits.push_back( make_pair( yl, yh ) );

			double p0 = cfg->getDouble( k+":p0" );
			double p1 = cfg->getDouble( k+":p1" );
			double p2 = cfg->getDouble( k+":p2" );

			cout << "cl=" << cl <<", ch=" << ch << ", yl=" << yl <<", yh=" << yh << ", p0=" << p0 << ", p1=" << p1 <<", p2=" << p2 << endl;

			TF1 * f = new TF1( ("eff_func_"+ts((int)yLimits.size())).c_str(), "[0] * exp( - pow( [1] / x, [2] ) )", 0, 5 );
			f->SetParameters( p0, p1, p2);

			eff.push_back( f );
		}

	}

	int find( int cl ){

		int i = 0;
		for ( auto k : cenLimits ){

			if ( cl >= k.first && cl <= k.second ){
				// if ( "tof" == name)
				// cout << "Eff for " << k.first << ", " << k.second << endl;
				return i;			
			}
			i++;
		}
		return -1;

	}

	double reweight( double pt, int cl ){

		int index = find( cl );
		if ( 0 <= index ){
			double val = eff[ index ]->Eval( pt );
			// if ( "tof" == name )
			// 	cout << "Eval( " << pt <<" ) = " << val << endl;
			return val;
		}

		return 0.0;
	}


	
};

#endif