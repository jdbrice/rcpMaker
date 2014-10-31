

#include "ChainLoader.h"
#include <fstream>

using namespace std;

namespace jdb{

	void ChainLoader::load( 
							TChain * chain, 	// the chain object to fill
							string ntdir, 	// the directory in which to look for ntuples
							int maxFiles 
							) {
		//cout << " [ChainLoader] searching " << ntdir << " for ntuples" << endl;

		if (maxFiles <= 0)
			maxFiles = 100000;

		uint nFiles = 0;
		DIR *dir;
		struct dirent *ent;
		bool go = true;
		if ( (dir = opendir ( ntdir.c_str() ) ) != NULL) {
			
			while ( go && (ent = readdir ( dir) ) != NULL) {

		    	if ( strstr( ent->d_name, "root") ){
		    		
		    		if ( nFiles >= maxFiles ) {
		    			go = false;
		    			break;
		    		}

		    		char fn[ 1024 ];
		    		sprintf( fn, "%s%s", ntdir.c_str(), ent->d_name );
		    		chain->Add( fn );

		    		//cout << "[ChainLoader] Adding file " << fn << " to chain" << endl;
		    		nFiles++;

		    	}
		  	}
		  	
		  	cout << "[ChainLoader] " << (nFiles) << " files loaded into chain" << endl;

		  	closedir (dir);
		}

	}	



	void ChainLoader::loadList(  TChain * chain, string listFile, int maxFiles ){
		
		uint nFiles = 0;

		string line;
		ifstream fListFile( listFile.c_str());
		if ( fListFile.is_open() ){

			while ( getline( fListFile, line ) ){
				chain->Add( line.c_str() );
				nFiles++;

				if ( maxFiles >= 1 && nFiles >= maxFiles ){
					break;
				}

			}
			fListFile.close();
				

		} else {
			cout << " Could not open " << listFile << endl;
		}

		cout << "[ChainLoader] " << (nFiles) << " files loaded into chain" << endl;


	}
}
