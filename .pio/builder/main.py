from SCons.Script import DefaultEnvironment, Default
import shutil, os
import subprocess
import json
import shutil
import io
import zlib
import time
import intelhex  # pip install intelhex - #https://python-intelhex.readthedocs.io/en/latest/part2-2.html
import os, json, io

env = DefaultEnvironment()

# if env.get("PROGNAME", "program") == "program":
#     env.Replace(PROGNAME="firmware")

# print(env.Dump())

env.AddPlatformTarget(
    "build", None, env.GetBuildPath(f"$PROJECT_DIR/app/build.sh"), "Upload"
)
env.AddPlatformTarget(
    "upload", "build", env.GetBuildPath(f"$PROJECT_DIR/app/upload.py"), "Upload"
)


def udynlink_size(file):
    with open(file, "rb") as f:
        f.read(4)
        l = int.from_bytes(f.read(2), byteorder="little")  # num_lot
        r = int.from_bytes(f.read(2), byteorder="little")
        # num_rels
        a = int.from_bytes(f.read(4), byteorder="little")
        # symt_size
        b = int.from_bytes(f.read(4), byteorder="little")
        # code_size
        c = int.from_bytes(f.read(4), byteorder="little")
        # data_size
        d = int.from_bytes(f.read(4), byteorder="little")
        # bss_size
        h = 24 + (r * 8) + a
        return h + b + c


def make_engines_hex(apps_json, ih=intelhex.IntelHex(), offset=int(1024 * 1024 / 2)):

    print(apps_json)
    if not os.path.exists(apps_json):
        print(apps_json, "not found!")
    else:
        with open(apps_json) as f:
            apps = json.load(f)
            i = 1
            for file in apps["apps"]:
                bin_file = os.path.dirname(apps_json) + "/" + str(file)
                if not os.path.exists(bin_file):
                    continue
                bin_size = os.path.getsize(bin_file)

                ih.loadbin(bin_file, offset=offset)
                bin_offset = offset
                offset += bin_size
                with open(bin_file, "rb") as f:
                    crc32sum = zlib.crc32(f.read())
                    ih.puts(offset, crc32sum.to_bytes(4, "little"))
                    offset += 4

                print(
                    i,
                    "0x%x" % bin_offset,
                    bin_file,
                    bin_size,
                    udynlink_size(bin_file) % 4,
                    "CRC32: %x" % crc32sum,
                )
                i += 1
                offset += 4096 - (offset % 4096)

    ih.puts(offset, 0xFFFF.to_bytes(4, "little"))
    return ih


def post_program_action(source, target, env):
    apps_json = env.GetProjectOption("apps_json")
    ahx = make_engines_hex(apps_json)
    program_path = env.GetBuildPath("$PROJECT_DIR/.pio/build/$PIOENV/engines.hex")
    ahx.tofile(program_path, format="hex")
    loader_sha = env.GetBuildPath("$PROJECT_DIR/.pio/build/$PIOENV/loader.sha")
    with open(loader_sha, "w") as f:
        f.write(env.GetProjectOption("squares_and_circles_loader"))
    print(program_path, len(ahx))


env.AddPostAction("build", post_program_action)

Default(["build"])
