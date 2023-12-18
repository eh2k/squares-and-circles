

for f in *.dsp; do
faust ./$f -lang c > ./$f.h
done