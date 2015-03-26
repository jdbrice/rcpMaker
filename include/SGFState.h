#ifndef SGF_STATE_H
#define SGF_STATE_H



class SGFState
{
public:
	SGFState(){


	}
	~SGFState();



	/**
	 * inv Beta and dEdx variables
	 */
	map< string, RooRealVar * > rrv;

	
};





#endif