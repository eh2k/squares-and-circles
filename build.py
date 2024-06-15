from SCons.Script import DefaultEnvironment
import subprocess
import json
import shutil
import io
import mido #~/.platformio/penv/bin/pip install mido python-rtmidi
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

                print("0x%x" % bin_offset, bin_file, bin_size, udynlink_size(bin_file) % 4, "CRC32: %x" % crc32sum)
    
                offset += 4 - (offset % 4)
                
    ih.puts(offset, 0xffff.to_bytes(4, 'little') )
    return ih

env = DefaultEnvironment()
#print(env.Dump())
env.Execute(f"bash $PROJECT_DIR/app/build.sh")

env.Append(
    LINKFLAGS=[
        "-specs=nano.specs",
        "-specs=nosys.specs",
    ]
)

midi_out = None
midi_in = None

def midi_init():
    global midi_in, midi_out
    try:
        for mo in mido.get_output_names():
            if "S&C" in mo or "Squares&Circles" in mo or "Teensy MIDI" in mo:
                midi_out = mido.open_output(mo)
                break

        for mi in mido.get_input_names():
            print(mi)
            if "S&C" in mi or "Squares&Circles" in mi or "Teensy MIDI" in mi:
                midi_in = mido.open_input(mi)
                break
    except:
        pass


def chunk_bytes(bytes_object, chunk_size):
    chunks = (bytes_object[i:i+chunk_size] for i in range(0, len(bytes_object), chunk_size))
    return chunks

def flush(input):
    for m in input.iter_pending():
        m #print("FLUSH", m)

def sendFLASHDATA(name, data0):
    flush(midi_in)
    data = bytes(name, 'utf-8')[:8] + data0
    crc32=zlib.crc32(data)
    print("Flashing", name, crc32)
    midi_out.send(mido.Message('song_select', song=0x7e))
    flush(midi_in)
    vlen = len(data)
    ch = 0 + ((vlen & 0xF000) >> 12)
    rawValue = [0b11100000 | ch,  vlen & 0x7f, vlen >> 7 & 0x7f]
    msg0 = mido.Message.from_bytes(rawValue)
    midi_out.send(msg0)
    msg1 = midi_in.receive(block=True)
    if msg1.pitch != 1:
        print("INIT FAILED", msg1.pitch, msg1)
        exit(-1) #no memory
    i=0
    time.sleep(1/10000)
    while i < len(data):
        int16 = data[i] | data[i + 1] << 8
        ch = 0 + ((data[i + 1] & 0xF0) >> 4)
        rawValue = [0b11100000 | ch, int16 & 0x7f, int16 >> 7 & 0x7f]
        midi_out.send(mido.Message.from_bytes(rawValue))
        time.sleep(2/10000)
        i += 2

    flush(midi_in)
    time.sleep(1/10000)

    rawValue = [0b11100000, crc32 & 0x7f, crc32 >> 7 & 0x7f]
    midi_out.send(mido.Message.from_bytes(rawValue))
    msg1 = midi_in.receive(block=True)
    if msg1.pitch != 1:
        print("CRC CHECK FAILED", msg1.pitch, msg1)
        exit(-1) #crc failed

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

    midi_init()

    print(midi_in)
    print(midi_out)

    if midi_out == None:
        exit(-1)

    program_path = env.GetBuildPath("$PROJECT_DIR/.pio/build/$PIOENV/engines.hex")
    ahx = intelhex.IntelHex(program_path)

    with io.BytesIO() as w:
        ahx.tofile(w, format='bin')
        abx=w.getvalue()
        offset = (1024 * 1024)
        for chunk in chunk_bytes(abx, 4096):
            sendFLASHDATA(f"0x%6x" % offset, chunk)
            offset += len(chunk)
        print(len(abx))
        midi_out.send(mido.Message('song_select', song=0x7f))

    exit(0)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", post_program_action)
env.AddPreAction("upload", upload_hex)