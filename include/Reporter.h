#ifndef REPORTER_H
#define REPORTER_H

#include "allroot.h"
#include "Logger.h"

namespace jdb {

	class Reporter
	{
	public:
		Reporter( string filename, int w = 791, int h = 1024, Logger * pLog = NULL  );
		~Reporter();

		void newPage( int dx = 1, int dy = 1);
		void cd( int pad );
		void cd( int padX, int padY);
		void next();
		void savePage( string name = "" );
		void saveImage( string name = "" );

	protected:

		Logger * log;
		TCanvas* canvas;
		int currentPad;
		int dx, dy;

		string filename;
		static int instances;

	};
	
}



#endif