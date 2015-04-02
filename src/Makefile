
# project name and executable name
project = rcp
# source suffix
source = .cpp 
# object files to make
objects = rcp.o InclusiveSpectra.o PidPhaseSpace.o PidParamMaker.o SimultaneousGaussians.o SimultaneousPid.o SGFSchema.o SGFRunner.o FemtoDstMaker.o FemtoDst.o ProdPicoDst.o
# ParamSpectra.o  PidParamMaker.o SimultaneousPid.o

sources = $(wildcard *.cpp)
headers = $(wildcard ../include/*.h)

# ROOT libs and includes
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTGLIBS     	= $(shell root-config --glibs)

# Custom Project includes and libs
includes		= -I../include -I. -I$(ROOTSYS)/include -I$(JDB_LIB)/include/jdb -I$(JDB_LIB)/include 
libraries 		= 	$(ROOTLIBS) -lRooFitCore -lRooFit $(CERNLIBS) $(SYSLIBS)
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
		$(compile)  $<

clean:
		@rm -f $(objects) $(project)
		@rm -f ../bin/$(project)
		@echo "clean done"

full:
	clean
	$(project)
