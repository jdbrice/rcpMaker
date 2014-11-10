#ifndef REPORTER_H
#define REPORTER_H

#include "TCanvas.h"
#include "Logger.h"
#include "XmlConfig.h"

namespace jdb {

	class Reporter
	{
	public:
		Reporter( string filename, int w = 791, int h = 1024 );
		Reporter( XmlConfig * config, string np, string prefix = "" );
		~Reporter();

		void newPage( int dx = 1, int dy = 1);
		void cd( int pad );
		void cd( int padX, int padY);
		void next();
		void savePage( string name = "" );
		void saveImage( string name = "" );

	protected:

		XmlConfig * cfg;
		string nodePath;
		Logger * logger;
		TCanvas* canvas;
		int currentPad;
		int dx, dy;

		string filename;
		static int instances;

	};
	
}



#endif