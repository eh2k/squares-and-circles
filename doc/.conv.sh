
montage -background '#25282c' -tile 5x -geometry +10+10 gnd_*.png cv_*.png drums_*.png mosc_*.png synth_*.png noise_*.png fx_*.png midi_*.png engines.png

for f in $1; do
    convert $f -sample 256x128 ${f%.*}.png
done
