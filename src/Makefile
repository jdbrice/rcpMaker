
# project name and executable name
project = app
# source suffix
source = .cpp 
# object files to make
objects = app.o HistoBook.o ChainLoader.o Analyzer.o Reporter.o
libObjects = HistoBook.o ChainLoader.o Analyzer.o Reporter.o

sources = $(wildcard *.cpp)
headers = $(wildcard ../include/*.h)

# ROOT libs and includes
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTGLIBS     	= $(shell root-config --glibs)

# Custom Project includes and libs
includes		= -I../include -I. -I$(ROOTSYS)/include -I$(ROOTDEV)/include 
libraries 			= 	$(ROOTLIBS) -lGui -lHtml -lGuiHtml $(CERNLIBS) $(SYSLIBS) \


cxx 		= g++ -std=c++0x
flags 		= -Wall -g $(ROOTCFLAGS) $(includes) -Wno-write-strings 
compile 	= $(cxx) $(flags) -c 
ldFlags  	= $(ROOTLDFLAGS) -g

all: $(project)


$(project): $(objects)
		ar rcs jdbREF.a $(libObjects)
		$(cxx) $(ldFlags) -o  $(project) app.o jdbREF.a $(ROOTGLIBS) $(libraries)
		@cp $(project) ../bin
#$(cxx) $(ldFlags) -o  $(project) $(objects) $(ROOTGLIBS) $(libraries)

%.o: %$(source)
		$(compile)  $<

clean:
		@rm -f $(objects) $(project)
		@rm -f ../bin/$(project)
		@rm -f jdbREF.a
		@echo "clean done"

full:
	clean
	$(project)

#doc:
#	@echo "Generating documentation..." 	\					
#	cldoc generate $(CXXFLAGS) --			\
#		--report 							\
#		--merge docs 						\
#		--output html 						\
#		$(sources) $(headers)
