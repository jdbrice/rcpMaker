

#include "HistoBook.h"
#include "TKey.h"
#include "TObject.h"
#include "Logger.h"
#include "LoggerConfig.h"


namespace jdb{
	/**
	 * Creates a histobook and allows the root filename to be set. optionally read from an existing root
	 * file and include everything into the working space
	 * @param name  root filename
	 * @param input input filename
	 * @param inDir input starting directory
	 */
	HistoBook::HistoBook( string name, string input, string inDir, Logger * nLog ){

		if ( NULL == nLog ){
			log = new Logger( Logger::llDefault, "HistoBook" );
		}
		else {
			log = nLog;

		}

		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		log->info(__FUNCTION__) << "Output File : " << filename << " opened" << endl;


		// make the legend and draw it once to apply styles etc.
		// for some reason needed to make styling work on the first draw
		legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
		legend->SetFillColor( kWhite );
		legend->Draw();
		legend->Clear();

		globalStyle();

		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			log->info(__FUNCTION__) << "Loading : " << inDir << " from " << input << endl;
			TFile * fin = new TFile( input.c_str() );
			cd ( inDir );
			fin->cd( inDir.c_str() );
			loadRootDir( gDirectory, inDir );
		}

	}	// Constructor

	/**
	 * Constructor that allows th input of a config file
	 * @param name name of file to use for saving root data
	 * @param con  The config file to use for all config relates calls
	 */
	HistoBook::HistoBook( string name, XmlConfig * con, string input, string inDir, Logger * nLog){


		config = con;

		if ( NULL == nLog ){
			log = LoggerConfig::makeLogger( config, "Logger" );
			log->setClassSpace("HistoBook" );
		}
		else {
			log = nLog;
		}

		log->info(__FUNCTION__) << "" << endl;

		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		log->info(__FUNCTION__) << "Output File : " << filename << " opened" << endl;

		// make the legend and draw it once to apply styles etc.
		// for some reason needed to make styling work on the first draw
		legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
		legend->SetFillColor( kWhite );
		legend->Draw();
		legend->Clear();

		globalStyle();


		log->info(__FUNCTION__) << " Set Configuration " << endl;

		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			log->info(__FUNCTION__) << "Loading : " << inDir << " from " << input << endl;
			TFile * fin = new TFile( input.c_str() );
			cd ( inDir );
			fin->cd( inDir.c_str() );
			loadRootDir( gDirectory, inDir );
		}
	}	// Constructor

	/**
	 * Destructor
	 * Frees the legend and ensures that the data is written to file
	 * and the file is closed properly
	 */
	HistoBook::~HistoBook(){
		log->info(__FUNCTION__) << "" << endl;
		delete legend;

		save();
		file->Close();
		log->info(__FUNCTION__) << " Memory freed, data written, file closed " << endl;
	}	// Destructor

	void HistoBook::save() {
		log->info(__FUNCTION__) << " Save to File " << filename << endl;
		file->Write();
	}	// save

	/**
	 * Loads a directory into the histobook
	 */
	void HistoBook::loadRootDir( TDirectory* tDir, string path ){
		log->info(__FUNCTION__) << " Loading from directory " << path << endl;

		TList* list;

		if ( tDir ){
			list = tDir->GetListOfKeys();
		} else {
			log->info(__FUNCTION__) << " Bad Directory " << path << endl;
			return;
		}

		TIter next(list);
		TKey* key;
		TObject* obj;

		while ( (key = (TKey*)next()) ) {

			obj = key->ReadObj() ;

			if ( 0 == strcmp(obj->IsA()->GetName(),"TDirectoryFile") ){
				TDirectoryFile* dir = (TDirectoryFile*)obj;

				string nPath = path + dir->GetName();
				if ( path == (string) "" )
					nPath = path + dir->GetName();
				else
					nPath = path + "/" + dir->GetName();

				cd( nPath );
				loadRootDir( dir, nPath );
			} else if ( obj ){
				if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2") && (!obj->InheritsFrom("TH1"))) ) {
					// not a 1d or 2d histogram
				} else {
					// add it to the book

					add( obj->GetName(), (TH1*)obj->Clone( obj->GetName() ) );
				}

			}
		}

	} // loadRootDir

	void HistoBook::add( string name, TH1* h ){

		log->info(__FUNCTION__) << " Adding " << name << endl;

		string oName = name;
		if ( name.length() <= 1 || !h ){
			log->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Invalid name" << endl;
			return;
		}

		name = currentDir + name;

		// dont allow duplicated name overites
		if ( book[ name ] ){
			log->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Duplicate exists" << endl;
			return;
		}

		// save the histo to the map
		book[ name ] = h;

	} 	// add

	string HistoBook::cd( string sdir  ){

		log->info(__FUNCTION__) << " In Directory " << sdir << endl;
		string old = currentDir;

		char* csdir = (char*)sdir.c_str();
		file->cd();

		if ( file->GetDirectory( csdir ) ){
			file->cd( csdir );
		} else {
			log->info(__FUNCTION__) << " Creating Directory " << sdir << endl;
			file->mkdir( csdir );
			file->cd( csdir );
		}

		currentDir = sdir;

		return old;
	}	// cd


	void HistoBook::make( string nodeName ){
		if ( config )
			make( config, nodeName );
	}	//make
	void HistoBook::make( XmlConfig * config, string nodeName ){

		if ( config && config->nodeExists( nodeName ) ){

			string hName = config->tagName( nodeName );
			if ( "" == hName )
				hName = nodeName;

			// store the path in the config file
			configPath[ hName ] = nodeName;

			string type = config->getString( nodeName + ":type" );
			string hTitle = config->getString( nodeName + ":title", hName );


			HistoBins* bx;
			HistoBins* by;
			HistoBins* bz;
			if ( config->nodeExists( nodeName + ":xBins" ) )
				bx = new HistoBins( config, config->getString( nodeName + ":xBins" ) );
			else 
				bx = new HistoBins( config, nodeName, "X" );
			if ( config->nodeExists( nodeName + ":yBins" ) )
				by = new HistoBins( config, config->getString( nodeName + ":yBins" ) );
			else 
				by = new HistoBins( config, nodeName, "Y" );
			if ( config->nodeExists( nodeName + ":zBins" ) )
				bz = new HistoBins( config, config->getString( nodeName + ":zBins" ) );
			else 
				bz = new HistoBins( config, nodeName, "Z" );

			if ( "1D" == type || ( (bx->nBins() > 0) && (by->nBins() <= 0) && (bz->nBins() <= 0) )){

				if ( bx->nBins() >= 1 )
					make1D( hName, hTitle, bx->nBins(), bx->bins.data() );
				else 
					log->warn(__FUNCTION__) << "Invalid Bins given for " << hName << endl;

			} else if ( "2D" == type || ( (bx->nBins() > 0) && (by->nBins() > 0) && (bz->nBins() <= 0) )){

				if ( bx->nBins() >= 1 && by->nBins() >= 1 )
					make2D( hName, hTitle,
						bx->nBins(), bx->bins.data(), by->nBins(), by->bins.data() );
				else 
					log->warn(__FUNCTION__) << "Invalid Bins given for " << hName << endl;

			} else {
				log->warn(__FUNCTION__) << "Histogram " << hName << " was not made "<< endl;
			}



		}

	}	// make
	void HistoBook::makeAll( XmlConfig * con, string nodeName ){

		if ( !con )
			return;
		vector<string> paths = con->childrenOf( nodeName );
		log->info(__FUNCTION__) << " Found " << paths.size() << " histogram paths " << endl;
		for ( int i=0; i < paths.size(); i++ ){

			make( paths[ i ] );
		}
	}	//makeAll
	void HistoBook::makeAll( string nodeName ){
		if ( !config ){
			log->info( __FUNCTION__ ) << "No config" << endl;
			return;
		}

		makeAll( config, nodeName );
	}	//makeAll

	void HistoBook::clone( string existing, string create ){

		log->info(__FUNCTION__) << " Cloning " << existing << " into " << create << endl;
		if ( get( existing ) ){
			TH1* nHist = (TH1*)get( existing )->Clone( create.c_str() );
			// add the new one
			add( create, nHist );
		} else {
			log->warn(__FUNCTION__) << existing << " Does Not Exist " << endl;
		}
	}	//clone

	void HistoBook::make1F( string name, string title, uint nBins, double low, double hi  ){
		log->info(__FUNCTION__) << "TH1F( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" << endl;
		TH1F* h;
		h = new TH1F( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1F

	void HistoBook::make1D( string name, string title, uint nBins, double low, double hi  ){

		log->info(__FUNCTION__) << "TH1D( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" << endl;
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1D

	void HistoBook::make1D( string name, string title, uint nBins, const Double_t* bins  ){
		log->info(__FUNCTION__) << "TH1D( " << name << ", " << title << ", " << nBins << ", " << "[]" <<  " )" << endl;
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, bins );

		this->add( name, h );
	}	//make1D

	void HistoBook::make2D( string name, string title, uint nBinsX, double lowX, double hiX, uint nBinsY, double lowY, double hiY ){

		log->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" << endl;
		TH2D* h;

		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, uint nBinsX, const Double_t* xBins, uint nBinsY, double lowY, double hiY ){
		log->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, uint nBinsX, double lowX, double hiX, uint nBinsY, const Double_t* yBins  ){
		log->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY <<  "[] )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, uint nBinsX, const Double_t* xBins, uint nBinsY, const Double_t * yBins ){
		log->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", []" << " )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	TH1* HistoBook::get( string name, string sdir  ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] )
			log->warn(__FUNCTION__) << sdir + "/" + name  << " Does Not Exist " << endl; 

		return book[ ( sdir  + name  ) ];
	}	//get
	TH2* HistoBook::get2D( string name, string sdir  ){
		return ((TH2*)get( name, sdir ));
	}	//get2D
	TH3* HistoBook::get3D( string name, string sdir ){
		return (( TH3* ) get( name, sdir ));
	}	//get3D

	void HistoBook::fill( string name, double bin, double weight ){
		if ( get( name ) != 0)
			get( name )->Fill( bin, weight );
		else
			log->warn(__FUNCTION__) << name << " Does Not Exist, cannot fill " << endl;
	}	//fill

	HistoBook* HistoBook::exportAs( string filename ) {

		string outName = styling + ".png";
		if ( "" != filename )
			outName = filename;

		log->info(__FUNCTION__) << "Exporting Pad to " << outName << endl;

		gPad->SaveAs( outName.c_str() );
		return this;

	}	//exportAs

	void HistoBook::globalStyle(){

		gStyle->SetCanvasColor(kWhite);     // background is no longer mouse-dropping white
	  	gStyle->SetPalette(1,0);            // blue to red false color palette. Use 9 for b/w
	  	gStyle->SetCanvasBorderMode(0);     // turn off canvas borders
	  	gStyle->SetPadBorderMode(0);
	  	gStyle->SetPaintTextFormat("5.2f");  // What precision to put numbers if plotted with "TEXT"

	  	// For publishing:
	  	gStyle->SetLineWidth(2.);
	  	gStyle->SetTextSize(0.7);
	  	gStyle->SetLabelSize(0.05,"xy");
	  	gStyle->SetTitleSize(0.05,"xy");
	  	gStyle->SetTitleOffset(1.0,"x");
	  	gStyle->SetTitleOffset(1.5,"y");
	  	gStyle->SetPadTopMargin(0.1);
	  	gStyle->SetPadRightMargin(0.1);
	  	gStyle->SetPadBottomMargin(0.16);
	  	gStyle->SetPadLeftMargin(0.2);

	  	gStyle->SetFillColor(-1);
		gStyle->SetFillStyle(4000);

	}	//globalStyle

	HistoBook* HistoBook::style( string histName ){
		styling = histName;

		log->info(__FUNCTION__) << " Styling " << histName << endl;

		// set the default style if it is there
		if ( config && config->nodeExists( configPath[ histName ] + ".style" ) ){
			log->info(__FUNCTION__) << " Setting style from " << configPath[ histName ] << ".style" << endl;
			set( configPath[histName] + ".style" );
		} else if ( config && config->nodeExists( configPath[ histName ] + ":style" ) && config->nodeExists( config->getString( configPath[ histName ] + ":style" ) ) ){
			log->info(__FUNCTION__) << " Setting Style from " << config->getString( configPath[ histName ] + ":style" ) << endl;
			set( config->getString( configPath[ histName ] + ":style" ) );
		}

		return this;
	}

	HistoBook* HistoBook::set( string param, string p1, string p2, string p3, string p4 ){

		vector<string> l;
		l.push_back( p1 );
		l.push_back( p2 );
		l.push_back( p3 );
		l.push_back( p4 );
		set( param, l );

		return this;
	}

	HistoBook* HistoBook::set( string param, double p1, double p2, double p3, double p4  ){

		vector<string> list;
		stringstream sstr;
		sstr.str("");
		sstr << p1;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p2;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p3;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p4;
		list.push_back( sstr.str() );

		set( param, list );

		return this;
	}

	HistoBook* HistoBook::set( XmlConfig* config, string nodePath ){

		// get the list of attributes and set the style from that
		vector< pair< string, string > > list = config->getAttributes( nodePath );

		for ( int i = 0; i < list.size(); i++ ){

			vector<string> params = config->split( list[ i ].second, ',' );
			for ( int p = 0; p < params.size(); p++ ){
				params[ p ] = config->trim(params[ p ]);
			}


			set( list[ i ].first, params );

		}

		return this;
	}
	HistoBook* HistoBook::set( string opt, vector<string> params ){

		// force the param name to lowercase
		transform(opt.begin(), opt.end(), opt.begin(), ::tolower);

	    TH1* h = get( styling );
	    if ( h ){

		    if ( "title" == opt ){
		    	h->SetTitle( cParam(params, 0) );
		    } else if ( "x" == opt ){
		    	h->GetXaxis()->SetTitle( cParam(params, 0) );
		    } else if ( "y" == opt ){
		    	h->GetYaxis()->SetTitle( cParam(params, 0) );
		    } else if ( "legend" == opt ){
		    	legend->AddEntry( h, cParam(params, 0), cParam(params, 1, "lpf") );
				legend->Draw();
		    } else if ( "draw" == opt ){
		    	drawOption = cParam(params, 0);
		    } else if ( "linecolor" == opt ){
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetLineColor( c );
		    } else if ( "fillcolor" == opt ){
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetFillColor( c );
		    } else if ( "linewidth" == opt ){
		    	h->SetLineWidth( dParam( params, 0) );
		    } else if ( "domain" == opt ){
		    	double min = dParam( params, 0);
		    	double max = dParam( params, 1);
			    h->GetXaxis()->SetRangeUser( min, max );
		    } else if ( "dynamicdomain" == opt ){
		    	double thresh = dParam( params, 0);
		    	int min = (int)dParam( params, 1);
		    	int max = (int)dParam( params, 2);
		    	int axis = (int)dParam( params, 3);		// 1 = x, 2 = y

		    	if ( 1 != axis && 2 != axis )
		    		axis = 1;

		    	if ( thresh >= 0) {
		    		if ( -1 >= min )
		    			min = h->FindFirstBinAbove( thresh, axis );
		    		if ( -1 >= max )
		    			max = h->FindLastBinAbove( thresh, axis );
		    	}

		    	if ( 1 == axis )
			  	  h->GetXaxis()->SetRange( min, max );
			  	else if ( 2 == axis )
			  		h->GetYaxis()->SetRange( min, max );

		    }  else if ( "range" == opt ){

		    	double min = dParam( params, 0);
		    	double max = dParam( params, 1);

		    	h->GetYaxis()->SetRangeUser( min, max );
		    } else if ( "markercolor" == opt ) {
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetMarkerColor( c );
		    } else if ( "markerstyle" == opt ) {
		    	h->SetMarkerStyle( (int)dParam( params, 0) );
		    } else if ( "legend" == opt ){
		    	// p1 - alignmentX
		    	// p2 - alignmentY
		    	// p3 - width
		    	// p4 - height

		    	// make sure option is valid
		    	double p1 = dParam( params, 0);
		    	double p2 = dParam( params, 1);
		    	if ( !(legendAlignment::center == p1 || legendAlignment::left == p1 || legendAlignment::right == p1) )
		    		p1 = legendAlignment::best;
		    	if ( !(legendAlignment::center == p2 || legendAlignment::top == p2 || legendAlignment::bottom == p2) )
		    		p2 = legendAlignment::best;
		    	placeLegend( p1, p2, dParam( params, 3), dParam( params, 3) );
		    } else if ( "numberofticks" == opt ){
		    	// p1 - # of primary divisions
		    	// p2 - # of secondary divisions
		    	// p3 - axis : 0 or 1 = x, 2 = y
		    	double p1 = dParam( params, 0);
		    	double p2 = dParam( params, 1);
		    	double p3 = dParam( params, 2);

		    	if ( p2 == -1 )
		    		p2 = 0;

			    if ( 2 == (int)p3 )
			    	h->GetYaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
			    else
			    	h->GetXaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
		    } else if ( "logy" == opt ){
		    	gPad->SetLogy( (int)dParam( params, 0 ) );
		    } else if ( "logx" == opt ){
		    	gPad->SetLogx( (int)dParam( params, 0 ) );
		    } else if ( "logz" == opt ){
		    	gPad->SetLogz( (int)dParam( params, 0 ) );
		    }




		}

		return this;



	}
	HistoBook * HistoBook::set( string nodeName ){
		if ( config )
			set( config, nodeName );
		return this;
	}



	HistoBook* HistoBook::draw(string name, Option_t* opt ){


		// no parameters
		if ( name == ""){
			TH1* h = get( styling );
			if ( h ){
				// use the draw option set in its styling
				h->Draw( drawOption.c_str() );
				drawOption = "";
				log->info(__FUNCTION__) << "Drawing " << styling << endl;
			} else
				log->warn(__FUNCTION__) << styling << " does not exist " << endl;
		} else {
			TH1* h = get( name );
			if ( h ){
				h->Draw( opt );
				log->info(__FUNCTION__) << "Drawing " << name << endl;
			} else
				log->warn(__FUNCTION__) << name << " does not exist " << endl;
		}

		return this;
	}


	HistoBook* HistoBook::placeLegend( int alignmentX, int alignmentY, double width, double height ){


		log->info(__FUNCTION__) << "Placing Legend" << endl;
		double mR = 1 - gPad->GetRightMargin();
		double mL = gPad->GetLeftMargin();
		double mT = 1- gPad->GetTopMargin();
		double mB = gPad->GetBottomMargin();

		double x1, x2, y1, y2;

		if ( width <= 0 || width > 1 )
			width = .2;
		if ( height <= 0 || height > 1 )
			height = .2;

		// alignment best needs a current histo
		if ( !(get( styling )) ){
			if ( legendAlignment::best == alignmentX )
				alignmentX = legendAlignment::right;
			if ( legendAlignment::best == alignmentY )
				alignmentY = legendAlignment::top;
		} else {

			//TODO

		}


		if ( 	legendAlignment::left ==  alignmentX ){
			x1 =  mL ;
			x2 =  mL + width;
		}
		if ( 	legendAlignment::right ==  alignmentX ){
			x1 =  mR - width;
			x2 =  mR ;
		}
		if ( 	legendAlignment::center ==  alignmentX ){
			x1 =  0.55 - width/2.0;
			x2 =  0.55 + width/2.0;
		}
		if ( 	legendAlignment::top ==  alignmentY ){
			y1 =  mT - height;
			y2 = mT ;
		}
		if ( 	legendAlignment::bottom ==  alignmentY ){
			y1 =  mB ;
			y2 =  mB + height;
		}
		if ( 	legendAlignment::center ==  alignmentY ){
			y1 =  0.55 - height/2.0;
			y2 =  0.55 + height/2.0;
		}
		legend->SetX1NDC( x1 );
		legend->SetX2NDC( x2 );
		legend->SetY1NDC( y1 );
		legend->SetY2NDC( y2 );

		return this;
	}

} // jdb namespace
