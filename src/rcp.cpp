

#include "XmlConfig.h"
using namespace jdb;

#include "LBNLPicoDst.h"


#include <iostream>
#include "InclusiveSpectra.h"
#include "ParamSpectra.h"
#include "PidPhaseSpace.h"
#include "PidParamMaker.h"
#include "SimultaneousGaussians.h"
#include "SimultaneousPid.h"

#include "XmlRooRealVar.h"
#include "SGFSchema.h"

#include <exception>

int main( int argc, char* argv[] ) {

	if ( argc >= 2 ){

		try{
			XmlConfig config( argv[ 1 ] );
			//config.report();

			string fileList = "";
			string jobPrefix = "";

			if ( argc >= 4 ){
				fileList = argv[ 2 ];
				jobPrefix = argv[ 3 ];
			}

			string job = config.getString( "jobType" );

			if ( "InclusiveSpectra" == job ){
				InclusiveSpectra is( &config, "InclusiveSpectra.", fileList, jobPrefix );
				is.make();
			} else if ( "ParamSpectra" == job ){
				//ParamSpectra ps( &config, "ParamSpectra.", fileList, jobPrefix );
				//ps.make();
			} else if ( "PidPhaseSpace" == job ){
				PidPhaseSpace pps( &config, "PidPhaseSpace.", fileList, jobPrefix  );
				pps.make();
			} else if ( "MakePidParams" == job ){
				PidParamMaker ppm( &config, "PidParamMaker." );
				ppm.make();
			} else if ( "SimultaneousPid" == job ){
				SimultaneousPid sg( &config, "SimultaneousPid." );
				sg.make();
			} else if ( "test" == job ){
				
				Logger::setGlobalColor( true );
				Logger::setGlobalLogLevel( Logger::llAll );
				
				TFile * inFile = new TFile(config.getString( "input:url").c_str(), "READ" );

				SGFSchema sgfs( &config, "Schema" );
				
				int ptBin = 4;
				// get the histo paths
				map< string, TH1D* > zb;
				map< string, TH1D* > zd;
				zb[ "zb_All" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( "K", 0, 1, ptBin, 0 )).c_str() );
				zd[ "zd_All" ] 	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( "K", 0, 1, ptBin, 0 )).c_str() );
				
				// dEdx enhanced distributions
				zd[ "zd_K" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( "K", 0, 1, ptBin, 0, "K" )).c_str() );
				zd[ "zd_Pi" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( "K", 0, 1, ptBin, 0, "Pi" )).c_str() );
				zd[ "zd_P" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( "K", 0, 1, ptBin, 0, "P" )).c_str() );

				// 1/beta enhanced distributions
				zb[ "zb_Pi" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( "K", 0, 1, ptBin, 0, "Pi" )).c_str() );
				zb[ "zb_K" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( "K", 0, 1, ptBin, 0, "K" )).c_str() );
				zb[ "zb_P" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( "K", 0, 1, ptBin, 0, "P" )).c_str() );

				sgfs.updateData( zb, "zb" );
				sgfs.updateData( zd, "zd" );
				
				sgfs.combineData();

				RooDataHist * d = sgfs.data( "" );
				RooCategory * cat = sgfs.cat();

				Reporter rp( "rpTest.pdf", 600, 600 );
				rp.newPage();
					RooRealVar * rrv = sgfs.var( "zd" ); 
					RooPlot * f = rrv->frame( );

					sgfs.simModel->fitTo( *d );
					
					d->plotOn(f, Cut("sample==sample::zd_All"));
					
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_All"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_All"), ProjWData(*cat,*d), Components( "zd_All_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_All"), ProjWData(*cat,*d), Components( "zd_All_gauss_K" ), LineColor( kBlack ) );
					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );

					gPad->SetLogy(1);
					 
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zd" ); 
					f = rrv->frame( );

					d->plotOn(f, Cut("sample==sample::zd_K"));
					
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_K"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_K"), ProjWData(*cat,*d), Components( "zd_K_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_K"), ProjWData(*cat,*d), Components( "zd_K_gauss_K" ), LineColor( kBlack ) );
					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );

					gPad->SetLogy(1);
					 
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zd" ); 
					f = rrv->frame( );

					d->plotOn(f, Cut("sample==sample::zd_Pi"));
					
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_Pi"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_Pi"), ProjWData(*cat,*d), Components( "zd_Pi_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_Pi"), ProjWData(*cat,*d), Components( "zd_Pi_gauss_K" ), LineColor( kBlack ) );
					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );

					gPad->SetLogy(1);
					 
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zd" ); 
					f = rrv->frame( );

					d->plotOn(f, Cut("sample==sample::zd_P"));
					
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_P"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_P"), ProjWData(*cat,*d), Components( "zd_P_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zd_P"), ProjWData(*cat,*d), Components( "zd_P_gauss_K" ), LineColor( kBlack ) );
					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );

					gPad->SetLogy(1);
					 
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zb" ); 
					f = rrv->frame( );
					
					d->plotOn(f, Cut("sample==sample::zb_All"));
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_All"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_All"), ProjWData(*cat,*d), Components( "zb_All_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_All"), ProjWData(*cat,*d), Components( "zb_All_gauss_K" ), LineColor( kBlack ) );

					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );
					gPad->SetLogy(1);
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zb" ); 
					f = rrv->frame( );
					
					d->plotOn(f, Cut("sample==sample::zb_Pi"));
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_Pi"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_Pi"), ProjWData(*cat,*d), Components( "zb_Pi_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_Pi"), ProjWData(*cat,*d), Components( "zb_Pi_gauss_K" ), LineColor( kBlack ) );

					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );
					gPad->SetLogy(1);
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zb" ); 
					f = rrv->frame( );
					
					d->plotOn(f, Cut("sample==sample::zb_K"));
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_K"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_K"), ProjWData(*cat,*d), Components( "zb_K_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_K"), ProjWData(*cat,*d), Components( "zb_K_gauss_K" ), LineColor( kBlack ) );

					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );
					gPad->SetLogy(1);
					f->Draw(); 
				rp.savePage();
				rp.newPage();
					rrv = sgfs.var( "zb" ); 
					f = rrv->frame( );
					
					d->plotOn(f, Cut("sample==sample::zb_P"));
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_P"), ProjWData(*cat,*d) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_P"), ProjWData(*cat,*d), Components( "zb_P_gauss_Pi" ), LineColor( kRed ) );
					sgfs.simModel->plotOn( f, Slice(*cat,"zb_P"), ProjWData(*cat,*d), Components( "zb_P_gauss_K" ), LineColor( kBlack ) );

					f->SetMinimum( .1 );
					f->SetMaximum( 1000000 );
					gPad->SetLogy(1);
					f->Draw(); 
				rp.savePage();

				rp.newPage();
				((TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( "K", 0, 1, ptBin, 0, "K" )).c_str() ))->Draw();;
				rp.savePage();
				inFile->Close();

			}





		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
