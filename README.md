# ethereum-partitioning-experiments
Tools Used in ["Challenges and pitfalls of partitioning blockchains"](https://arxiv.org/abs/1804.07356)

## Building
Download and install [Boost](https://www.boost.org/) and [Metis](http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz) 

Make sure you have boost with Graph Library Support (BGL)

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
results are saved in `/tmp/edge_cut_evolution_partitions_<params>`

## Observing results

Feed the experiment to one of the scripts in `./scripts` folder, for example:
```
SAVEGRAPH=true ./plotEdgeCutEvolution.py /tmp/edge_cut_evolution_<params>.txt
```
A pdf with the same name as the experiment will be created, or omit the `SAVEGRAPH` variable for
a python visualization.

