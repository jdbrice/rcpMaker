
# project name and executable name
project = rcp
# source suffix
source = .cpp 

# Source files to make
sources = 	Engine.cpp 							\
			Common.cpp 							\
			Spectra/InclusiveSpectra.cpp 		\
			Spectra/ZRecentering.cpp 			\
			Adapter/ProdPicoDst.cpp 			\
			Adapter/RcpPicoDst.cpp 				\
			TSF/FitSchema.cpp 					\
			TSF/Fitter.cpp 						\
			TSF/FitRunner.cpp 					\
			McMaker/EnergyLoss.cpp 				\
			McMaker/TpcEffFitter.cpp 			\
			McMaker/TofEffFitter.cpp 			\
			McMaker/FeedDownMaker.cpp			\
			Params/FeedDownParams.cpp 			\
			Params/EnergyLossParams.cpp			\
			Spectra/PidHistoMaker.cpp

#			McMaker/TofEffMaker.cpp 			\
#			FeedDownMaker.cpp  					\

#			PidYieldPresenter.cpp 				\

# TODO: Fix broken files

objects = $(patsubst %.cpp,%.o, $(sources))

ClassHeaders = $(wildcard ../include/*/*.h)

# ROOT libs and includes
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTGLIBS     	= $(shell root-config --glibs)

# Custom Project includes and libs
includes		= -I../include -I. -I$(ROOTSYS)/include -I$(JDB_LIB)/include/jdb -I$(JDB_LIB)/include 
libraries 		= $(ROOTLIBS) -lRooFitCore -lRooFit -lMinuit $(CERNLIBS) $(SYSLIBS)
staticLibs 		= $(JDB_LIB)/lib/libJDB.a

cxx 		= g++ -std=c++11
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
