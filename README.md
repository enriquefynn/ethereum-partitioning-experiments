# ethereum-partitioning-experiments

Download and install [Boost](https://www.boost.org/) and [Metis](http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz) 

Make sure you have boost with graph library support (BGL)

## Building
```
mkdir ./build
cd build
cmake ..
```

If you cmake cannot find METIS, or you installed it in another location do `cmake -DMETISROOTDIR=<location> ..`


## Testing
```
cd ./build
./bin/unit_tests
```

## Running

Download [calls.tar.gz](https://dslab.inf.usi.ch/ethereum_trace/downloads/calls.tar.gz) from https://dslab.inf.usi.ch/ethereum_trace/

Save and uncompress the files.

To run the test, go inside ./build:
```
./bin/buildGraph <calls.txt> <config.txt>
```
