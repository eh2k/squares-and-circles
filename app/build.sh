#!/usr/bin/bash

set -e #-eo pipefail
SCRIPT_PATH="$(realpath $(dirname $0))"
cd ${SCRIPT_PATH}/..

BUILD_DIR=${BUILD_DIR:-"./.build"}


[[ -d ${BUILD_DIR}/.venv ]] || python3 -m venv ${BUILD_DIR}/.venv

[[ -f ${BUILD_DIR}/.venv/bin/activate ]] && source ${BUILD_DIR}/.venv/bin/activate
[[ -f ${BUILD_DIR}/.venv/Scripts/activate ]] && source ${BUILD_DIR}/.venv/Scripts/activate

pip install -r app/requirements.txt

GCC_VERSION=15.2.1-1.1
GCC_XPACK=xpack-arm-none-eabi-gcc-${GCC_VERSION}
GCC_URL=https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v${GCC_VERSION}

if [[ ! -d ${BUILD_DIR}/${GCC_XPACK} ]] ; then
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR}
    if which winver; then
        [[ -f ./${GCC_XPACK}-win32-x64.zip ]] || curl -fLO ${GCC_URL}/${GCC_XPACK}-win32-x64.zip
        unzip ${GCC_XPACK}-win32-x64.zip -d .
    else
        [[ -f ./${GCC_XPACK}-linux-x64.tar.gz ]] || curl -fLO ${GCC_URL}/${GCC_XPACK}-linux-x64.tar.gz
        cat ${GCC_XPACK}-linux-x64.tar.gz  | tar xvz -C . > /dev/null
    fi 
    cd -
fi

export GCC_PATH=$(realpath ${BUILD_DIR}/${GCC_XPACK}/bin/)
export PATH="${PATH}:${GCC_PATH}"

if [[ -d ./machine/lib/udynlink ]]; then
    mkmodule=$(realpath ./machine/lib/udynlink/scripts/mkmodule)
else
    if [[ ! -d ${BUILD_DIR}/udynlink ]]; then
        git clone https://github.com/eh2k/udynlink.git ${BUILD_DIR}/udynlink/
    fi

    mkmodule=$(realpath ${BUILD_DIR}/udynlink/scripts/mkmodule)
fi

# target cpu for -mcpu, e.g. MCPU=cortex-m4 bash app/build.sh (needs a mkmodule with --mcpu)
MCPU=${MCPU:-cortex-m7}
MCPU_ARG=""
if grep -q -- "--mcpu" "$mkmodule"; then
    MCPU_ARG="--mcpu=$MCPU"
else
    echo "$mkmodule has no --mcpu option, can't build for $MCPU (update udynlink)"
    exit 1
fi

if ! which arm-none-eabi-gcc; then
    echo arm-none-eabi-gcc not found!
    exit 1
fi

if [[ "$1" == "--rebuild" ]]; then
    find "${SCRIPT_PATH}" -type f -name "*.bin" -print0 -exec touch {} +
    rm -f ${SCRIPT_PATH}/apps.txt
fi

# app/<GROUP>/<Name>/<Name>.cpp: the source, the .bin and the README.md of an app live in one directory
for f in $(find "${SCRIPT_PATH}" -mindepth 3 -maxdepth 3 -type f -name '*.cpp' | sort | while read -r c; do
             [[ "$(basename "$c" .cpp)" == "$(basename "$(dirname "$c")")" ]] && echo "$c"; done); do

X="$(realpath --relative-to=. "${f%.*}")"
OUT="$X"
NAMEX="$(basename "$(dirname "$(dirname "$X")")")/$(basename "$X")"
if [ -f "$OUT.bin" ] && [ "$(date -R -r $OUT.bin)" = "$(date -R -r $f)" ]; then
    continue
fi

BUILD_FLAGS=$(grep "build_flags:" $X.cpp | cut -d':' -f2- | xargs)
BUILD_FLAGS=${BUILD_FLAGS:-"-mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math"}

#-fno-rtti
# the names of the ENGINE_NAME("...") lines (squares-and-circles-api.h), ';' between them; without: CATEGORY/FileName.
# From the preprocessor: an ENGINE_NAME that an #ifdef leaves out is not one (SYNTH/plaits.cpp PLAITS_SPEECH)
NAME="$(arm-none-eabi-g++ -E -P -w -mcpu=$MCPU -mthumb -fsingle-precision-constant -DNDEBUG $BUILD_FLAGS -I. -I./lib/ $f |
    sed -n 's/.*__engine_names_fit("\([^"]*\)").*/\1/p')"
NAME=${NAME//$'\n'/';'}
NAME=${NAME:-"$NAMEX"}

echo ----- "$NAME" -----

echo "BUILD_FLAGS:$BUILD_FLAGS"

SRC_FILES=$(grep "src_files" $X.cpp  | cut -d':' -f2 | xargs)
find "$(dirname $X.cpp)" -type f -name '*.o' -delete
# mkmodule leaves *.o / *.o.temp.o next to shared sources; stale ones break the next module
for s in $SRC_FILES; do rm -f "${s%.*}.o" "${s%.*}.o.temp.o" "${s%.*}.o_prologue.o"; done
#--no-opt \
#-felide-constructors -fno-rtti -std=gnu++14 -Wno-error=narrowing -fno-threadsafe-statics
#-mtune=cortex-m7 
$mkmodule $(realpath $X.cpp --relative-base=.) \
    --no-opt $MCPU_ARG \
    --build_flags="-fsingle-precision-constant -DNDEBUG -pedantic -fno-exceptions $BUILD_FLAGS -I. -I./lib/ " \
    --public-symbols="setup,process,draw,screensaver,__ui_event_handler,__midi_event_handler,__engine_alias" \
    --name="$NAME" $SRC_FILES> $X.log
sed -i "s|$(realpath .)|.|" $X.log
touch -d "$(date -R -r $X.cpp)" $X.bin


#cat $X.log | arm-none-eabi-c++filt -t > ${X}2.log

arm-none-eabi-objdump -Dztr --source $X.elf | arm-none-eabi-c++filt -t > $X.elf.txt
#arm-none-eabi-nm -l -t d -S -C --size-sort --synthetic --special-syms --with-symbol-versions --reverse-sort ./$X.elf > $X.log
python -m elf_size_analyze -t arm-none-eabi- $(realpath $X.elf --relative-base=.) -F -R --no-color >> $X.log

grep "__aeabi_" $X.log && exit 1


#xxd -i $X.bin > ./$X.bin.h
#sed -i "s/unsigned char/const uint8_t/g" ./$X.bin.h
#sed -i "s/\[\]/\[\] FLASHMEM __attribute__((aligned(32)))/g" ./$X.bin.h

# arm-none-eabi-gcc -fPIE -msingle-pic-base -mcpu=cortex-m4 -mthumb -Wl,--unresolved-symbols=ignore-in-object-files -fdump-lang-class -I. -I../lib/ $X.cpp
# arm-none-eabi-objdump --disassemble-all --no-addresses ./$X.elf > $X.s
# arm-none-eabi-objdump --disassemble-all ./$X.elf > $X.adr.s
#/home/master/git/udynlink/tests/xpack-qemu-arm-7.2.5-1/bin/qemu-system-gnuarmeclipse -board STM32F429I-Discovery -image $X.elf -nographic

done

cd ${SCRIPT_PATH}
find . -type f -name '*.o' -delete
./dump.py | tee apps.txt;

if [[ "$1" == "--upload" || "$2" == "--upload" ]]; then
    python upload.py
fi
