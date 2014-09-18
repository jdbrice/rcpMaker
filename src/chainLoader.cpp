

#include "ChainLoader.h"

namespace jdb{

	void ChainLoader::load( 
							TChain * chain, 	// the chain object to fill
							const char* ntdir, 	// the directory in which to look for ntuples
							uint maxFiles 
							) {
		//cout << " [ChainLoader] searching " << ntdir << " for ntuples" << endl;

		if (maxFiles == 0)
			maxFiles = 1000;

		uint nFiles = 0;
		DIR *dir;
		struct dirent *ent;
		bool go = true;
		if ( (dir = opendir ( ntdir ) ) != NULL) {
			
			while ( go && (ent = readdir ( dir) ) != NULL) {

		    	if ( strstr( ent->d_name, "root") ){
		    		
		    		if ( nFiles >= maxFiles ) {
		    			go = false;
		    			break;
		    		}

		    		char fn[ 1024 ];
		    		sprintf( fn, "%s%s", ntdir, ent->d_name );
		    		chain->Add( fn );

		    		//cout << "[ChainLoader] Adding file " << fn << " to chain" << endl;
		    		nFiles++;

		    	}
		  	}
		  	
		  	cout << "[ChainLoader] " << (nFiles) << " files loaded into chain" << endl;

		  	closedir (dir);
		}

	}	
}
