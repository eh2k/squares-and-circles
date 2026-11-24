#!/usr/bin/bash

set -e #-eo pipefail
SCRIPT_PATH="$(realpath $(dirname $0))"
cd ${SCRIPT_PATH}/..

BUILD_DIR=${BUILD_DIR:-"./.build"}

# The xpack arm-none-eabi-gcc, downloaded once into ${BUILD_DIR}/ (--toolchain: only that, also for the firmware
# build). Of the Linux archive (307 MB, 1.1 GB unpacked) only the parts the builds use are unpacked (~210 MB): of its
# multilibs thumb/v7e-m+dp/hard (Cortex-M7 with FPU) and thumb/v7e-m/nofp (Cortex-M4/M7 without FPU), not the others
# nor the default one (crt*.o, lib*.a above thumb/: ARM mode); gdb without python, no Fortran, LTO and gold. The apps
# (mkmodule links with -nostdlib) need none of the libraries. A part missing from the archive (another GCC_VERSION)
# stops the build; a tool or multilib left out that a build wants needs an entry here. The Windows zip is unpacked
# whole. The lines GCC_VERSION .. the end of GCC_PARTS are the key of the gcc cache of the GitHub workflows: a change
# there downloads it anew.
GCC_VERSION=15.2.1-1.1
GCC_XPACK=xpack-arm-none-eabi-gcc-${GCC_VERSION}
GCC_URL=https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v${GCC_VERSION}
GCC_LIB=${GCC_XPACK}/lib/gcc/arm-none-eabi/${GCC_VERSION%-*}
GCC_PARTS=(
    --wildcards --no-wildcards-match-slash
    --exclude='arm-none-eabi-gdb-*' --exclude='arm-none-eabi-gstack*' --exclude=arm-none-eabi-lto-dump --exclude=lto1
    --exclude=arm-none-eabi-gfortran --exclude=f951 --exclude='libgfortran*' --exclude='*ld.gold'
    ${GCC_XPACK}/{README.md,distro-info,bin,libexec,lib/bfd-plugins}
    ${GCC_LIB}/{include,include-fixed,thumb/v7e-m+dp/hard,thumb/v7e-m/nofp}
    ${GCC_XPACK}/arm-none-eabi/{bin,include,lib/ldscripts,'lib/*.specs',lib/thumb/v7e-m+dp/hard,lib/thumb/v7e-m/nofp}
)
# On Linux x64 first these parts alone, packed (31 MB, tar.xz, its directory ${GCC_XPACK}/ as in the xpack archive):
# an asset of the release 0.0.0 of eh2k/squares-and-circles, made from the xpack archive with GCC_PARTS (bash
# app/build.sh --toolchain into an empty BUILD_DIR, then tar --sort=name --owner=0 --group=0 --numeric-owner -c
# ${GCC_XPACK} | xz -9e), the apps built with it the same as with the xpack archive. Taken only with this SHA-256;
# not there, another hash or no sha256sum/xz: the xpack archive as below. Another GCC_VERSION finds none (the name);
# another GCC_PARTS needs a new asset and its hash here (or GCC_SC_SHA256 empty: the xpack archive)
GCC_SC=${GCC_XPACK}-linux-x64-sc.tar.xz
GCC_SC_URL=https://github.com/eh2k/squares-and-circles/releases/download/0.0.0/${GCC_SC}
GCC_SC_SHA256=c2bf4c946130217f8c965b271079263a9b5ad7950d0400aed93baf08263b7e62

if [[ ! -d ${BUILD_DIR}/${GCC_XPACK} ]] ; then
    mkdir -p ${BUILD_DIR}
    case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
        GCC_ZIP=${BUILD_DIR}/${GCC_XPACK}-win32-x64.zip
        [[ -f ${GCC_ZIP} ]] || curl -fL ${GCC_URL}/${GCC_XPACK}-win32-x64.zip -o ${GCC_ZIP}
        unzip -q ${GCC_ZIP} -d ${BUILD_DIR}
        ;;
    *)
        # unpacked beside and moved into place when complete: an aborted download leaves no half gcc behind
        GCC_TGZ=${BUILD_DIR}/${GCC_XPACK}-linux-x64.tar.gz
        GCC_TMP=${BUILD_DIR}/.${GCC_XPACK}.$$
        rm -rf ${GCC_TMP} && mkdir -p ${GCC_TMP}
        # the packed parts (GCC_SC above), not when an xpack archive was put into ${BUILD_DIR}/ by hand: into a file
        # first, unpacked only with the right hash
        if [[ ! -f ${GCC_TGZ} && -n ${GCC_SC_SHA256} && "$(uname -s)" == Linux && "$(uname -m)" == x86_64 ]] &&
           which sha256sum xz > /dev/null; then
            echo "app/build.sh: ${GCC_SC} (31 MB)"
            if curl -fL ${GCC_SC_URL} -o ${GCC_TMP}/${GCC_SC} &&
               echo "${GCC_SC_SHA256}  ${GCC_TMP}/${GCC_SC}" | sha256sum -c --quiet - &&
               tar xJf ${GCC_TMP}/${GCC_SC} -C ${GCC_TMP} &&
               [[ -x ${GCC_TMP}/${GCC_XPACK}/bin/arm-none-eabi-gcc ]]; then
                [[ -d ${BUILD_DIR}/${GCC_XPACK} ]] || mv ${GCC_TMP}/${GCC_XPACK} ${BUILD_DIR}/
            else
                echo "app/build.sh: ${GCC_SC} not taken, the xpack archive instead"
            fi
            rm -rf ${GCC_TMP} && mkdir -p ${GCC_TMP}
        fi
        # the xpack archive: streamed into tar, no archive is kept (one put into ${BUILD_DIR}/ by hand is taken
        # instead of the download)
        if [[ ! -d ${BUILD_DIR}/${GCC_XPACK} ]]; then
            echo "app/build.sh: ${GCC_XPACK}-linux-x64.tar.gz (307 MB), unpacking the parts the builds use"
            if ! ( set -o pipefail
                   if [[ -f ${GCC_TGZ} ]]; then cat ${GCC_TGZ}
                   else curl -fL ${GCC_URL}/${GCC_XPACK}-linux-x64.tar.gz; fi |
                       tar xz -C ${GCC_TMP} "${GCC_PARTS[@]}" ); then
                rm -rf ${GCC_TMP}
                echo "app/build.sh: ${GCC_XPACK} could not be downloaded or unpacked"
                exit 1
            fi
            [[ -d ${BUILD_DIR}/${GCC_XPACK} ]] || mv ${GCC_TMP}/${GCC_XPACK} ${BUILD_DIR}/
        fi
        rm -rf ${GCC_TMP}
        ;;
    esac
fi

export GCC_PATH=$(realpath ${BUILD_DIR}/${GCC_XPACK}/bin/)
export PATH="${PATH}:${GCC_PATH}"

# --toolchain: only the gcc above, no .venv, no udynlink (the firmware build, the GitHub workflows)
if [[ "$1" == "--toolchain" ]]; then
    exit 0
fi

[[ -d ${BUILD_DIR}/.venv ]] || python3 -m venv ${BUILD_DIR}/.venv

[[ -f ${BUILD_DIR}/.venv/bin/activate ]] && source ${BUILD_DIR}/.venv/bin/activate
[[ -f ${BUILD_DIR}/.venv/Scripts/activate ]] && source ${BUILD_DIR}/.venv/Scripts/activate

# the python packages: app/requirements.txt those of the app build (mkmodule: Jinja2, pyelftools; elf-size-analyze for
# the .log), in the dev repository also app/requirements-dev.txt (pyusb, intelhex, Pillow: test_usb, app/upload.py,
# machine/make_firmware_full.py, the images of app/dump.py). pip only when these files or the python changed since
# its last install (their checksum in the venv, .requirements): a build does not wait for pip
REQS=(app/requirements.txt)
[[ -f app/requirements-dev.txt ]] && REQS+=(app/requirements-dev.txt)
REQ_SUM="$(python --version 2>&1) $(cat "${REQS[@]}" | cksum)"
if [[ "$(cat ${BUILD_DIR}/.venv/.requirements 2>/dev/null)" != "${REQ_SUM}" ]]; then
    pip install $(printf -- '-r %s ' "${REQS[@]}")
    echo "${REQ_SUM}" > ${BUILD_DIR}/.venv/.requirements
fi

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

# --setup: only the tools above (.venv, arm-none-eabi-gcc, udynlink), no app built: the SessionStart hook of the
# release's .claude/settings.json runs it at the start of a Claude Code session in the cloud
if [[ "$1" == "--setup" ]]; then
    echo "app/build.sh: the build tools are ready"
    exit 0
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
PREPROCESSED="$(arm-none-eabi-g++ -E -P -w -mcpu=$MCPU -mthumb -fsingle-precision-constant -DNDEBUG $BUILD_FLAGS -I. -I./lib/ $f)"
NAME="$(echo "$PREPROCESSED" | sed -n 's/.*__engine_names_fit("\([^"]*\)").*/\1/p')"
NAME=${NAME//$'\n'/';'}
NAME=${NAME:-"$NAMEX"}

echo ----- "$NAME" -----

echo "BUILD_FLAGS:$BUILD_FLAGS"

SRC_FILES=$(grep "src_files" $X.cpp  | cut -d':' -f2 | xargs)
# the ENGINE_ALIAS and ENGINE_INSERT_FX lines (squares-and-circles-api.h) as the constants __engine_alias,
# __engine_aliases and __insert_fx, a C file of their own compiled with the app (none without an alias or a mark)
ALIAS_SRC="${BUILD_DIR}/aliases/${X//\//_}.c"
echo "$PREPROCESSED" | python app/engine_aliases.py "$ALIAS_SRC"
[ -f "$ALIAS_SRC" ] && SRC_FILES="$SRC_FILES $ALIAS_SRC"
find "$(dirname $X.cpp)" -type f -name '*.o' -delete
# mkmodule leaves *.o / *.o.temp.o next to shared sources; stale ones break the next module
for s in $SRC_FILES; do rm -f "${s%.*}.o" "${s%.*}.o.temp.o" "${s%.*}.o_prologue.o"; done
#--no-opt \
#-felide-constructors -fno-rtti -std=gnu++14 -Wno-error=narrowing -fno-threadsafe-statics
#-mtune=cortex-m7 
$mkmodule $(realpath $X.cpp --relative-base=.) \
    --no-opt $MCPU_ARG \
    --build_flags="-fsingle-precision-constant -DNDEBUG -pedantic -fno-exceptions $BUILD_FLAGS -I. -I./lib/ " \
    --public-symbols="setup,process,draw,screensaver,restore,__ui_event_handler,__midi_event_handler,__engine_alias,__engine_aliases,__insert_fx" \
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

# --upload: app/upload.py, the changed and new apps to the attached modules over USB; not in the release (its apps go
# onto the module over the web app's MCP)
if [[ "$1" == "--upload" || "$2" == "--upload" ]]; then
    if [[ -f upload.py ]]; then
        python upload.py
    else
        echo "--upload: no app/upload.py here; an app goes onto the module over the web app's MCP (load_app)"
        exit 1
    fi
fi
