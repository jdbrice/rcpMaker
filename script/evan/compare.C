
#include "rcp.cpp"

void compare( string en = "11.5", string cpf = "/Users/danielbrandenburg/bnl/local/work/rcpMaker/products/11GeV/presentation/data.root" ){

	for ( int c = 0; c < 2; c++ ){
		for ( int p = 0; p < 3; p++ ){

			rcp( en, c, p, cpf );

		}
	}
}