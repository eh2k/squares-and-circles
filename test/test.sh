set -e

cd $(dirname $0)

mkdir -p ../.test
#-std=c++2a 
g++ -m64 -I ../src/ -I ../.pio/libdeps/*/libmachine*/ -D TEST -Wformat=0 -Wnarrowing $(find ../src/ | grep ".cc") ./test.cxx -o ../.test/test.exe

cd ../.test
rm *.wav || true
./test.exe
#aplay test.wav
rm test.exe
./uwedit.exe 0*.wav || true
#rm test.wav