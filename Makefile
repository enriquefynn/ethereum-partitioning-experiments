.PHONY: all clean

all:
	make -C libutils all
	make -C libcommon all
	make -C libMETIS all
	make -C libpartitioning all
	make -C buildGraph all

clean:
	make -C libutils clean
	make -C libcommon clean
	make -C libMETIS clean
	make -C libpartitioning clean
	make -C buildGraph clean


# INCLUDE=/home/fynn/.local/include
# INCLUDE_LIB=/home/fynn/.local/lib/
# CPPFLAGS=-Wall -std=c++11 -O3
# CC=c++
# OBJECTS=METIS_methods.o utils.o config.o

# all: buildGraph

# config.o: config.cpp config.h
# 	${CC} -c ${CPPFLAGS} -I${INCLUDE} -L${INCLUDE_LIB} config.cpp

# utils.o: utils.cpp utils.h
# 	${CC} -c ${CPPFLAGS} -I${INCLUDE} -L${INCLUDE_LIB} utils.cpp

# METIS_methods.o: METIS_methods.cpp METIS_methods.h config.h
# 	${CC} -c ${CPPFLAGS} -I${INCLUDE} -L${INCLUDE_LIB} METIS_methods.cpp 

# buildGraph: utils.o buildGraph.cpp METIS_methods.o config.o
# 	${CC} ${CPPFLAGS} -I${INCLUDE} -L${INCLUDE_LIB} -lmetis ${OBJECTS} buildGraph.cpp -o buildGraph

# clean:
# 	rm -Rf *.o buildGraph
