CPP=g++
CPPOPT=-Wall -std=c++11
#CPPOPT=-Wall -std=c++14 # If C++14 is available
#CPPOPT=-Wall -std=c++11 -g # For debug

default: othello_sample othello_runner

othello_sample: othello_sample.cpp
	$(CPP) $(CPPOPT) -o $@ $<

othello_runner: othello_runner.cpp
	$(CPP) $(CPPOPT) -o $@ $<

clean:
	rm -f othello_sample othello_sample.exe
	rm -f othello_runner othello_runner.exe
