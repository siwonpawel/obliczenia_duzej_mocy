clang++ -O3 --std=c++11 -stdlib=libc++ main.cpp -o 2kt

./2kt1
./2kt2
./2kt4
./2kt6
./2kt8

clang++ -fopenmp -O3 --std=c++11 -stdlib=libc++ lab03.cpp -o lab03 && ./lab03

export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"

clang++ -O3 --std=c++11 -stdlib=libc++ lab03.cpp -o lab03