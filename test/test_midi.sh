
set -e

int=$(amidi -l | tail -n -1 | cut -d' ' -f 3)
amidi -p $int -S "F34D"  # set CH 1,2,3,4

RANDOM=1
I=0
N=44
while true; do
    for i in $(seq 0 100); do 
        C=$(($RANDOM % 4))
        PROG=$((1 + $RANDOM % $N))
        if [[ $PROG == "x35" || $PROG == "33" || $PROG == "39" || $PROG == "40" || $PROG == "41" ]]; then continue; fi
        
        echo "$I: $C -> $PROG"
        amidi -p $int -S "$(printf "C%x%02x" $C $PROG)"
        
        I=$(($I + 1))
        #sleep 0.001
    done
    amidi -p $int -S "F372" #NEXT (press L)
done