g++ -c bls.cpp
g++ -c tokeniser.cpp
g++ -c NBC_handler.cpp
g++ -c execMaster.cpp
g++ -o bls bls.o tokeniser.o execMaster.o NBC_handler.o -lreadline
