#!/bin/bash

rm OrderCacheTests
clear
argument=$1
if [ "$argument" == "" ]; then
    g++ --std=c++17 main.cpp OrderCache.cpp -o OrderCacheTests -pthread
	valgrind --tool=memcheck --leak-check=full ./OrderCacheTests
fi

if [ "$argument" == "self" ]; then
	g++ --std=c++17 self_test_main.cpp OrderFactory.cpp OrderCache.cpp -o OrderCacheTests -pthread
	valgrind --tool=memcheck --leak-check=full ./OrderCacheTests
fi