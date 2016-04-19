

#include "fitParamSet.C"

void makeAll(){
	for ( string plc : { "Pi", "K", "P" } ){
		for ( string c : { "p", "n" } ){
			for ( int iPar : { 0, 1, 2 } ){

				fitParamSet( plc, c, iPar );

			}
		}
	}
}