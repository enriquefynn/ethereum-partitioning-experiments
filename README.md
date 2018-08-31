# ethereum-partitioning-experiments

Download and install [Boost](https://www.boost.org/) and [Metis](http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz) 

Make sure you have boost with graph library support (BGL)

## Building
```
mkdir ./build
cd build
cmake ..
```

If you installed metis in other location use `cmake -DMETISROOTDIR=<location> ..`


## Testing
```
cd ./build
./bin/unit_tests
```

## Running
```
TODO
```
