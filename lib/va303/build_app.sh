#!/usr/bin/bash
# Builds one app as app/build.sh does (without the other apps, dump.py and apps.txt), from this tree, which may be a
# worktree without the udynlink submodule (then the main checkout's is used) and without its own .build (a link):
#   bash lib/va303/build_app.sh app/SYNTH/VA-303/VA-303.cpp
set -e
cd "$(dirname "$0")/../.."
export PATH="$PATH:$(realpath .build/xpack-arm-none-eabi-gcc-15.2.1-1.1/bin)"
SITE=$(ls -d .build/.venv/lib/python3.*/site-packages | tail -1)
export PYTHONPATH=$(realpath "$SITE")
MKMODULE=machine/lib/udynlink/scripts/mkmodule
[ -f "$MKMODULE" ] || MKMODULE=../../../machine/lib/udynlink/scripts/mkmodule
f=$1
X="${f%.*}"
MCPU=${MCPU:-cortex-m7}
BUILD_FLAGS=$(grep "build_flags:" $X.cpp | cut -d':' -f2- | xargs)
BUILD_FLAGS=${BUILD_FLAGS:-"-mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math"}
NAME="$(arm-none-eabi-g++ -E -P -w -mcpu=$MCPU -mthumb -fsingle-precision-constant -DNDEBUG $BUILD_FLAGS -I. -I./lib/ $f |
    sed -n 's/.*__engine_names_fit("\([^"]*\)").*/\1/p')"
NAME=${NAME//$'\n'/';'}
NAME=${NAME:-"$(basename "$(dirname "$(dirname "$X")")")/$(basename "$X")"}
echo "----- $NAME -----"
find "$(dirname $X.cpp)" -type f -name '*.o' -delete
python3 $MKMODULE $(realpath $X.cpp --relative-base=.) \
    --no-opt --mcpu=$MCPU \
    --build_flags="-fsingle-precision-constant -DNDEBUG -pedantic -fno-exceptions $BUILD_FLAGS -I. -I./lib/ " \
    --public-symbols="setup,process,draw,screensaver,restore,__ui_event_handler,__midi_event_handler,__engine_alias,__engine_aliases,__insert_fx" \
    --name="$NAME" > $X.log
sed -i "s|$(realpath .)|.|" $X.log
touch -d "$(date -R -r $X.cpp)" $X.bin
arm-none-eabi-objdump -Dztr --source $X.elf | arm-none-eabi-c++filt -t > $X.elf.txt
python3 -m elf_size_analyze -t arm-none-eabi- $(realpath $X.elf --relative-base=.) -F -R --no-color >> $X.log
find "$(dirname $X.cpp)" -type f -name '*.o' -delete
if grep "__aeabi_" $X.log; then echo "__aeabi_ calls!"; exit 1; fi
tail -3 $X.log
ls -la $X.bin
