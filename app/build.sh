#!/usr/bin/bash

set -e #-eo pipefail
SCRIPT_PATH="$(realpath $(dirname $0))"
cd ${SCRIPT_PATH}/..

if ! which arm-none-eabi-gcc; then
    echo arm-none-eabi-gcc not found!
    exit 1
fi

pip install jinja2 pyelftools elf_size_analyze --upgrade pip

if [[ "$1" == "--rebuild" ]]; then
    find "${SCRIPT_PATH}/" -type f -name "*.bin" -print0 -exec touch {} +
    rm -f ${SCRIPT_PATH}/apps.txt
fi

for f in $(find "${SCRIPT_PATH}" -mindepth 2 -maxdepth 2 -type f -name '*.cpp'); do

X="${f%.*}"
if [ -f $oo ] && [ "$(date -R -r $X.bin)" = "$(date -R -r $f)" ]; then
    continue
fi

#-fno-rtti 
NAME="$(grep "ENGINE_NAME:" $f | cut -d':' -f2 | sed 's/^ *//g')"
NAME=${NAME//$'\n'/';'} #replace \n with ''
NAME=${NAME:-"$(realpath --relative-to=${SCRIPT_PATH} $X)"}

echo ----- "$NAME" -----

mkmodule=$(ls lib/udynlink/scripts/mkmodule | head -n1)

BUILD_FLAGS=$(grep "build_flags:" $X.cpp | cut -d':' -f2- | xargs)

BUILD_FLAGS=${BUILD_FLAGS:-"-mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math"}

echo "BUILD_FLAGS:$BUILD_FLAGS"

SRC_FILES=$(grep "src_files" $X.cpp  | cut -d':' -f2 | xargs)
find "$(dirname $X.cpp)" -type f -name '*.o' -delete
#--no-opt \
#-felide-constructors -fno-rtti -std=gnu++14 -Wno-error=narrowing -fno-threadsafe-statics
$mkmodule $(realpath $X.cpp --relative-base=.) \
    --no-opt \
    --build_flags="-fsingle-precision-constant -DNDEBUG -pedantic -fno-exceptions $BUILD_FLAGS -I. -I./lib/ " \
    --public-symbols="setup,process,draw,screensaver,__ui_event_handler,__midi_event_handler" \
    --name="$NAME" $SRC_FILES> $X.log
sed -i "s|$(realpath .)|.|" $X.log
touch -d "$(date -R -r $X.cpp)" $X.bin

#cat $X.log | arm-none-eabi-c++filt -t > ${X}2.log

arm-none-eabi-objdump -Dztr --source $X.elf | arm-none-eabi-c++filt -t > $X.elf.txt
#arm-none-eabi-nm -l -t d -S -C --size-sort --synthetic --special-syms --with-symbol-versions --reverse-sort ./$X.elf > $X.log
which elf-size-analyze >/dev/null && elf-size-analyze -t arm-none-eabi- $(realpath $X.elf --relative-base=.) -F -R --no-color >> $X.log
${SCRIPT_PATH}/dump.py "$(realpath $X.bin --relative-base=.)" | tee -a $X.log
#rm $X.elf

if [[ "$1" == "--rebuild" ]]; then
    ${SCRIPT_PATH}/dump.py "$(realpath $X.bin --relative-base=.)" >> ${SCRIPT_PATH}/apps.txt
fi

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
pwd
find . -type f -name '*.o' -delete
find . -type f -name '*.bin' -exec stat --printf="%-20n\t%6s\n" -- {} \;
du -ch ./*/*.bin | grep total
#find . -type f -name '*.bin' -delete

if [[ "$1" == "--upload" || "$2" == "--upload" ]]; then
    ./upload.py
fi