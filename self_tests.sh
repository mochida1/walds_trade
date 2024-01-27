#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Removing old files${NC}" 
rm OrderCacheTests_bin
rm GTests
rm RandomTests
rm SelfTests
clear
argument=$1
if [ "$argument" == "" ]; then
	echo -e "${YELLOW}COMPILING PROVIDED TESTS${NC}"

    g++ -ggdb --std=c++17 main.cpp OrderCache.cpp -o OrderCacheTests_bin -pthread
	if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation failed!${NC}"
		exit 1
    fi
	g++ -ggdb --std=c++17 OrderCacheTests.cpp OrderCache.cpp -o GTests -I/usr/local/include -L/usr/local/lib -lgtest -lgtest_main -pthread
	if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation failed!${NC}"
		exit 1
    fi
	g++ -ggdb --std=c++17 random_tests_main.cpp OrderFactory.cpp OrderCache.cpp -o RandomTests -pthread
	if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation failed!${NC}"
		exit 1
    fi
	g++ -ggdb --std=c++17 self_test_main.cpp OrderFactory.cpp OrderCache.cpp -o SelfTests -pthread
	if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation failed!${NC}"
		exit 1
    fi
	echo -e "${GREEN}COMPILING DONE${NC}"

	echo -e "${YELLOW}RUNNING MEMMORY TESTS${NC}"
	# valgrind --tool=memcheck --leak-check=full ./OrderCacheTests_bin > /dev/null
	if [ $? -ne 0 ]; then
        echo "${RED}MEMORY CHECK FAILED!${NC}"
		exit 1
    fi
	# valgrind --tool=memcheck --leak-check=full ./GTests > /dev/null
	if [ $? -ne 0 ]; then
        echo -e "${RED}MEMORY CHECK FAILED!${NC}"
		exit 1
    fi
	# valgrind --tool=memcheck --leak-check=full ./RandomTests > /dev/null
	if [ $? -ne 0 ]; then
        echo -e "${RED}MEMORY CHECK FAILED!${NC}"
		exit 1
    fi
	# valgrind --tool=memcheck --leak-check=full ./SelfTests > /dev/null
	if [ $? -ne 0 ]; then
        echo -e "${RED}MEMORY CHECK FAILED!${NC}"
		exit 1
    fi
	echo -e "${GREEN}VALGRINDING DONE${NC}"

	./OrderCacheTests_bin
	./GTests
	./RandomTests
	./SelfTests
fi

if [ "$argument" == "self" ]; then
	rm SelfTests
	g++ -ggdb --std=c++17 self_test_main.cpp OrderFactory.cpp OrderCache.cpp -o SelfTests -pthread
	./SelfTests
fi

if [ "$argument" == "BM" ]; then
	rm Benchmark
	rm Benchmark_O3
	rm Benchmark_Ofast
	echo -e "${YELLOW}Compiling all binaries...{NC}"
	g++ -ggdb --std=c++17 benchmark_main.cpp Benchmark.cpp OrderFactory.cpp OrderCache.cpp -o Benchmark -pthread
	# g++ -O3 --std=c++17 benchmark_main.cpp Benchmark.cpp OrderFactory.cpp OrderCache.cpp -o Benchmark_O3 -pthread
	# g++ -Ofast --std=c++17 benchmark_main.cpp Benchmark.cpp OrderFactory.cpp OrderCache.cpp -o Benchmark_Ofast -pthread
	echo -e "${YELLOW}Done!{NC}"

	echo -e "${YELLOW}BENCHMARKING ${RED}WITHOUT${YELLOW} COMPILER OPTIMIZATIONS${NC}"
	./Benchmark

	# echo -e "${YELLOW}BENCHMARKING ${GREEN}WITH${YELLOW} COMPILER OPTIMIZATIONS${NC}"
	# ./Benchmark_O3

	# echo -e "${YELLOW}BENCHMARKING ${NC}WITH Ofast${YELLOW} COMPILER OPTIMIZATIONS${NC}"
	# ./Benchmark_Ofast
fi

if [ "$argument" == "clean" ]; then
echo -e "${YELLOW}Removing old files${NC}" 
rm OrderCacheTests_bin
rm GTests
rm RandomTests
rm Benchmark
rm Benchmark_O3
rm Benchmark_Ofast
echo -e "${GREEN}DONE!${NC}" 
fi