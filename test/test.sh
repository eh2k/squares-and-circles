cd $(dirname $0)

mkdir -p ../.test
#-std=c++2a 
INC=$(for i in ../.pio/libdeps/*/*/*/; do echo "-I $i"; done )
INC="$INC $(for i in ../.pio/libdeps/*/*/; do echo "-I $i"; done )"
SRC=$(find ../.pio/libdeps/ -name "*.c*" | grep -v -E "main|EEPROM|SPI|libmachine" )
SRC="$SRC $(find ../src/ -name "*.c*" | grep -v main | grep -v -E "midi|SAM/")"

set -ex

echo $SRC

g++ -m64 -I ../src/ -I ../.pio/libdeps/*/libmachine*/ $INC -D TEST -DFLASHMEM="" -DPROGMEM="" -DVERSION="\"0\"" \
    -Wformat=0 -fpermissive -Wnarrowing -D_GLIBCXX_USE_C99 $SRC ./test.cxx -o ../.test/test.exe

cd ../.test
rm *.wav || true
./test.exe
#aplay test.wav
rm test.exe
./uwedit.exe 0*.wav || true
#rm test.wav