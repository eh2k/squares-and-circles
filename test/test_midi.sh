
set -e

int=$(amidi -l | tail -n -1 | cut -d' ' -f 3)
amidi -p $int -S "F34D"  # set CH 1,2,3,4

RANDOM=1
I=0
N=52
while true; do
    for i in $(seq 0 100); do 
        PROG1=$((1 + $RANDOM % $N))
        PROG2=$((1 + $RANDOM % $N))
        PROG3=$((1 + $RANDOM % $N))
        PROG4=$((1 + $RANDOM % $N))
        #if [[ $PROG == "x35" || $PROG == "33" || $PROG == "39" || $PROG == "40" || $PROG == "x41" ]]; then continue; fi
        
        echo "$I: $C -> $PROG1, $PROG2, $PROG3, $PROG4"
        amidi -p $int -S "$(printf "C%x%02x" 0 $PROG1)"
        amidi -p $int -S "$(printf "C%x%02x" 1 $PROG2)"
        amidi -p $int -S "$(printf "C%x%02x" 2 $PROG3)"
        amidi -p $int -S "$(printf "C%x%02x" 3 $PROG4)"
        
        I=$(($I + 1))
        #sleep 0.001
    done
    amidi -p $int -S "F372" #NEXT (press L)
done