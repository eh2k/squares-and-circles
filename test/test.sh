cd $(dirname $0)

mkdir -p ../.test
#-std=c++2a 
INC=$(for i in ../.pio/libdeps/*/*/; do echo "-I $i"; done )
FILTER="fv1|marbles|main|EEPROM|SPI|machine|hemisphere|test"
SRC=$(find -L ../src/ ../lib/ -name "*.cc" -o -name "*.cxx" -o -name "*.cpp" | grep -v -E "$FILTER" )
SRC_C=$(find ../src/ ../lib/ -name "*.c" | grep -v -E "$FILTER" )
set -ex

#rsync -avh --existing ../stmlib/ ./src/stmlib/

g++ -pg -g -m64 -I ../lib/ -I ../lib/machine/include/ -I ../src/ -I ../.pio/libdeps/*/libmachine*/ $INC -D TEST -DFLASHMEM="" -DPROGMEM="" -DVERSION="\"0\"" \
    -Wformat=0 -fpermissive -Wnarrowing -D_GLIBCXX_USE_C99 ./test.cxx $SRC -o ../.test/test.exe

cd ../.test
rm *.wav || true
./test.exe
#aplay test.wav
#rm test.exe
./uwedit.exe 0*.wav || true
#rm test.wav