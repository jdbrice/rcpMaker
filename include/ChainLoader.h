
#ifndef CHAINLOADER_H
#define CHAINLOADER_H

#include "dirent.h"
#include "TChain.h"
#include "Logger.h"



namespace jdb{

	class ChainLoader{

	public:
		//static void load( TChain * chain, char* ntdir, uint maxFiles = 1000 );
		static void load( TChain * chain, string ntdir, int maxFiles = -1 );
		static void loadList( TChain * chain, string listFile, int maxFiles = -1 );
	};


}


#endif

