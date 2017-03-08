
# project name and executable name
project = rcp
# source suffix
source = .cpp 

# Source files to make
sources = 	Engine.cpp 							\
			Common.cpp 							\
			Adapter/ProdPicoDst.cpp 			\
			Adapter/RcpPicoDst.cpp 				\
			Spectra/InclusiveSpectra.cpp 		\
			Spectra/ZRecentering.cpp 			\
			Spectra/PidDataMaker.cpp			\
			Spectra/TofEffSpectra.cpp			\
			Spectra/PidHistoMaker.cpp			\
			McMaker/FeedDownMaker.cpp			\
			McMaker/EnergyLoss.cpp 				\
			McMaker/TpcEffFitter.cpp 			\
			McMaker/TofEffFitter.cpp 			\
			McMaker/FeedDownFitter.cpp			\
			McMaker/TpcEffMaker.cpp				\
			Params/FeedDownParams.cpp 			\
			Params/EnergyLossParams.cpp			\
			TSF/FitSchema.cpp 					\
			TSF/Fitter.cpp 						\
			TSF/FitRunner.cpp 					\
			TSF/TpcFitRunner.cpp 				\
			Correction/SpectraCorrecter.cpp		\
			Correction/ApplyPostCorr.cpp		\
			Spectra/ZdDataMaker.cpp
			# Present/PidYieldPresenter.cpp 		\
			
			


objects = $(patsubst %.cpp,%.o, $(sources))

ClassHeaders = $(wildcard ../include/*/*.h)

# ROOT libs and includes
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTGLIBS     	= $(shell root-config --glibs)

# Custom Project includes and libs
includes		= -I../include -I. -I$(ROOTSYS)/include -I$(JDB_LIB)/include/Extra -I$(JDB_LIB)/include 
libraries 		= $(ROOTLIBS) -lMinuit $(CERNLIBS) $(SYSLIBS) 
staticLibs 		= $(JDB_LIB)/lib/libRooBarbCore.a $(JDB_LIB)/lib/libRooBarbConfig.a $(JDB_LIB)/lib/libRooBarbTasks.a $(JDB_LIB)/lib/libRooBarbRootAna.a $(JDB_LIB)/lib/libRooBarbExtra.a

cxx 		= g++ -std=c++11 -DJDB_LOG_LEVEL=60
flags 		= -Wall -O3 $(ROOTCFLAGS) $(includes)
compile 	= $(cxx) $(flags) -c 
ldFlags  	= $(ROOTLDFLAGS) 

all: $(project)

$(project): $(objects)
		$(cxx) $(ldFlags) -o  $(project) $(objects) $(staticLibs) $(ROOTGLIBS) $(libraries)
		@mv $(project) ../bin

%.o: %$(source)
		$(compile)  $< -o $@

clean:
		@rm -f $(objects) $(project)
		@rm -f ../bin/$(project)
		@echo "clean done"

full:
	clean
	$(project)

doc:
	@echo "Generating Documentation";				
	clDoc generate $(includes) $(flags) -- 	--report --output ../html --merge ../html/mergedocs $(ClassHeaders)
	python /Users/danielbrandenburg/bnl/local/work/boiler/html/sanitize_std.py ../html/xml/
