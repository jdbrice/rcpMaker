#ifndef SIGMA_HISTORY_H
#define SIGMA_HISTORY_H

#include <vector>
using namespace std;

#include "Logger.h"
using namespace jdb;

namespace TSF{

	class HistorySet
	{
	protected:
		double _mean = 0, _std = 0;
		vector<double> vals;
	public:
		static constexpr auto tag = "HistorySet";
		HistorySet(){
			DEBUG( tag, "()" )
		}
		HistorySet( const HistorySet &other ){
			this->_mean = other._mean;
			this->_std = other._std;
			this->vals = other.vals;
		}
		~HistorySet(){
			DEBUG( tag, "()" )
		}

		void add( double value ){
			INFO( tag, "( value=" << value << " )" )
			
			vals.push_back( value );
			
			// update these values
			calcMean();
			calcStd();

		}

		void calcMean(){
			double total = 0;
			for ( double v : vals ){
				total += v;
				INFO( tag, "val = " << v )
			}

			if ( vals.size() > 0 )
				_mean = total / vals.size();

			INFO( tag, "Mean = " << _mean );
		}

		void calcStd(){

			double total = 0;
			for ( double v : vals ){
				total += ( v - _mean ) * ( v - _mean );
			}
			if ( vals.size() > 1 )
				_std = total / (vals.size() - 1.0);

			_std = sqrt( _std );

			INFO( tag, "Std dev = " << _std );

		}

		double mean(){
			return _mean;
		}
		double std(){
			return _std;
		}
		
	};


}

#endif