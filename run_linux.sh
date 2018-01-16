#!/usr/bin/env sh
LD_PATH=libMETIS/bin/:${LD_PATH}:${LD_LIBRARY_PATH}
LD_PATH=/home/fynn/.local/lib:${LD_PATH}
LD_PATH=libcommon/bin/:${LD_PATH} 
LD_PATH=libutils/bin/:${LD_PATH} 
LD_PATH=libpartitioning/bin/:${LD_PATH} 
LD_PATH=libfuture/bin/:${LD_PATH} 

LD_LIBRARY_PATH=${LD_PATH} ./buildGraph/bin/buildGraph $1 $2
