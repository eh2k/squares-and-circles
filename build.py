from SCons.Script import DefaultEnvironment
import subprocess
import json
import shutil
import io
import zlib
import time
import intelhex #pip install intelhex - #https://python-intelhex.readthedocs.io/en/latest/part2-2.html
import os, json, io

def udynlink_size(file):
    with open(file, "rb") as f:
        f.read(4)
        l = int.from_bytes(f.read(2), byteorder='little')#num_lot
        r = int.from_bytes(f.read(2), byteorder='little'); #num_rels
        a = int.from_bytes(f.read(4), byteorder='little'); #symt_size
        b = int.from_bytes(f.read(4), byteorder='little'); #code_size
        c = int.from_bytes(f.read(4), byteorder='little'); #data_size
        d = int.from_bytes(f.read(4), byteorder='little'); #bss_size
        h = 24 + (r * 8) + a
        return(h + b + c)
    
def make_engines_hex(apps_json, ih = intelhex.IntelHex(), offset= 0):

    print(apps_json)
    if not os.path.exists(apps_json):
        print(apps_json, "not found!")
    else:
        with open(apps_json) as f:
            apps = json.load(f)
            i = 1
            for file in apps["apps"]: 
                bin_file=os.path.dirname(apps_json)+'/'+str(file)
                if not os.path.exists(bin_file):
                    continue
                bin_size=os.path.getsize(bin_file)
                
                ih.loadbin(bin_file,offset=offset)
                bin_offset = offset
                offset += bin_size
                with open(bin_file, "rb") as f:
                    crc32sum = zlib.crc32(f.read())
                    ih.puts(offset, crc32sum.to_bytes(4, 'little') )
                    offset += 4

                print(i, "0x%x" % bin_offset, bin_file, bin_size, udynlink_size(bin_file) % 4, "CRC32: %x" % crc32sum)
                i+=1
                offset += 4096 - (offset % 4096)
                
    ih.puts(offset, 0xffff.to_bytes(4, 'little') )
    return ih

env = DefaultEnvironment()
#print(env.Dump())
#env.Execute(f"bash $PROJECT_DIR/app/build.sh")

env.Append(
    LINKFLAGS=[
        "-specs=nano.specs",
        "-specs=nosys.specs",
    ]
)

def post_program_action(source, target, env):
    os.remove(target[0].get_abspath())
    apps_json=env.GetProjectOption("apps_json")
    ahx = make_engines_hex(apps_json)
    program_path = env.GetBuildPath("$PROJECT_DIR/.pio/build/$PIOENV/engines.hex")
    ahx.tofile(program_path, format='hex')
    loader_sha = env.GetBuildPath("$PROJECT_DIR/.pio/build/$PIOENV/loader.sha")
    with open(loader_sha, "w") as f:
        f.write(env.GetProjectOption("squares_and_circles_loader"))
    print(program_path, len(ahx))

def upload_hex(source, target, env):
    print("not implemented!")
    exit(0)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", post_program_action)
env.AddPreAction("upload", upload_hex)