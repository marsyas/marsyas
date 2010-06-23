#PROJECT(ANN)

# source files for ANN
SET(ANN_SRCS
bd_fix_rad_search.cpp
bd_pr_search.cpp
bd_search.cpp
bd_tree.cpp
bd_tree.h
brute.cpp
kd_dump.cpp
kd_fix_rad_search.cpp
kd_fix_rad_search.h
kd_pr_search.cpp
kd_pr_search.h
kd_search.cpp
kd_search.h
kd_split.cpp
kd_split.h
kd_tree.cpp
kd_tree.h
kd_util.cpp
kd_util.h
perf.cpp
pr_queue.h
pr_queue_k.h
ANN.h
ANNperf.h
ANNx.h
)
#ADD_LIBRARY(ANN STATIC ${ANN_SRCS})
ADD_LIBRARY(ANN ${ANN_SRCS})


