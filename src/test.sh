
cd $(dirname $0)/..
set -ex


export GCC_PATH=$(realpath .)/.build/gcc_mingw64/bin/
export PATH="${PATH}:${GCC_PATH}"

COVERAGE="" #"-coverage"

g++ -pg -g -m64 -O0 -coverage -I. -I./lib/ -I./extern/ -fpermissive -DTEST src/test.cxx src/SYNTH/vSID.cpp -o test.exe
./test.exe
#rm test.exe;

if [[ $COVERAGE != "" ]]; then
    [ -d .build/lcov-1.16/ ] || curl -L https://github.com/linux-test-project/lcov/releases/download/v1.16/lcov-1.16.tar.gz | tar xvz -C .build/
    .build/lcov-1.16/bin/lcov --directory . --capture --output-file coverage.info
    .build/lcov-1.16/bin/genhtml --demangle-cpp --dark-mode -o .test coverage.info
fi
rm *.gcda *.gcno gmon.out coverage.info