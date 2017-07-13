# Cache
A two-level (L1 and L2) cache simulator.The cache simulator will take several parameters describing the cache (block size, associativity, etc) along with a memory access trace file for an input 


1) Folder contains required c++ program to support cache implementation as cachesimulator.cpp
2) Here algorithm used to design cach is array of hash maps
3) Hash map are used to store cache structure which includes index, tag array , dirty bit and valid bit
4) Arrays of maps are used to create hash map according the number of ways defined in config file.
5) The code can be compiled using g++ cachesimulator.cpp -o cachesimulator.out
6) Run the code using  ./cachesimulator.out cacheconfig.txt trace.txt
7) trace.txt,  cacheconfig.txt , trace.txt.out, and screenshot of successfull compilation are all present in the zipped folder


