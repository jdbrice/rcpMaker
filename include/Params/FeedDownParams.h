#ifndef FEED_DOWN_PARAMS_H
#define FEED_DOWN_PARAMS_H

// ROOBARB
#include "XmlConfig.h"
#include "Logger.h"
#include "Utils.h"
using namespace jdb;

#include "TF1.h"


class FeedDownParams{
protected:

	XmlConfig * cfg;
	string nodePath;
	Logger log;

	float cl, ch;
	string formula;
	TF1 * fn;

	static int instances;

public:

	FeedDownParams( XmlConfig * cfg, string nodePath );
	~FeedDownParams() {}

	bool usable( double rm ) {
		if ( cl < rm && rm <= ch )
			return true;
		return false;
	}

	double weight( double pt ){
		DEBUG( "FeedDownParams( cl="<< cl <<", ch=" << ch << " " )
		if ( fn )
			return fn->Eval( pt );
		return -999;
	}

};

#endif