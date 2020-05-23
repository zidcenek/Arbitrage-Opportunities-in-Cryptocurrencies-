CXX=g++
LD=g++
CXXFLAGS=-std=c++17 -pthread

all: selection

selection: main.o Arbitrage.o CurrencyPair.o FilesManager.o OutputFormat.o Triplet.o
	$(LD) $(CXXFLAGS)  -o $@ $^

clean:
	rm -f  main.o Arbitrage.o CurrencyPair.o FilesManager.o OutputFormat.o Triplet.o selection

%o: %cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


main.o: main.cpp Arbitrage.h CurrencyPair.h FilesManager.h Triplet.h OutputFormat.h
Arbitrage.o: Arbitrage.h CurrencyPair.h FilesManager.h Triplet.h OutputFormat.h
CurrencyPair.o: CurrencyPair.h
FilesManager.o: FilesManager.h Triplet.h
OutputFormat.o: OutputFormat.h CurrencyPair.h
Triplet.o: Triplet.h