#!/usr/bin/env python3

import glob
import json
import os
import sys, zlib, hashlib

workingdir = os.path.dirname(__file__) + "/../app/"

def md5sum(data):
    h  = hashlib.md5()
    h.update(data)
    return h.hexdigest()

def dump(bin_file):
    f = open(bin_file, "rb")
    data = f.read()
    f.close()
    #print(data[0:4]) # sign
    l = int.from_bytes(data[4:6], byteorder='little')#num_lot
    r = int.from_bytes(data[6:8], byteorder='little'); #num_rels
    a = int.from_bytes(data[8:12], byteorder='little'); #symt_size
    b = int.from_bytes(data[12:16], byteorder='little'); #code_size
    c = int.from_bytes(data[16:20], byteorder='little'); #data_size
    d = int.from_bytes(data[20:24], byteorder='little'); #bss_size

    h = 24 + (r * 8) + a
    ram_size = (l * 4) + c + d
    bin_size = h + b + c
    crc32sum = zlib.crc32(data[0:-1])
    print("%-35s %10d %10d  %08x" % (os.path.relpath(f.name, workingdir).replace(os.sep, '/'), bin_size, ram_size, crc32sum))
    return bin_size, ram_size, crc32sum
    #print("CRC32 %x" % crc32sum)
    #print("MD5 %s" % md5sum(data))

# the engines of an app: the module name (symbol 0 of the symbol table, type UDYNLINK_SYM_TYPE_NAME,
# machine/lib/udynlink/udynlink/udynlink.c get_sym_at), from ENGINE_NAME (build.sh --name), several separated by ';'.
# A name with "/@" (DRUMKITS/@!RC8) stands for one engine per flash blob with that magic (udynlink_engine.cpp add_app):
# kept as it is. A version after a name ("SEQ/Clock#v2", ENGINE_NAME) is not part of the name the firmware shows
# (machine::add()): dropped
def engine_names(bin_file):
    with open(bin_file, "rb") as f:
        data = f.read()
    num_rels = int.from_bytes(data[6:8], byteorder='little')
    symt = 24 + num_rels * 8 # after the header (6 words) and the relocations
    name_off = int.from_bytes(data[symt + 4:symt + 8], byteorder='little')
    if (name_off >> 28) & 3 != 3: # not the module name
        return []
    start = symt + (name_off & 0x0FFFFFFF)
    return [n.split("#")[0] for n in data[start:data.index(b"\0", start)].decode().split(";")]

# the exported constant NAME of an app in its code (__engine_aliases, app/engine_aliases.py), as the firmware reads it
# (machine/src/udynlink_engine.cpp app_constant()): the symbols after the header and the relocations, a symbol's
# value from the start of the code (after the symbol table); None without one
def app_constant(data, name):
    num_rels = int.from_bytes(data[6:8], byteorder='little')
    symt = 24 + num_rels * 8
    code = symt + int.from_bytes(data[8:12], byteorder='little')
    word = lambda off: int.from_bytes(data[off:off + 4], byteorder='little')
    for i in range(word(symt)):
        name_off, val = word(symt + 4 + i * 8), word(symt + 8 + i * 8)
        if (name_off >> 28) & 3 != 1 or not (name_off >> 28) & 4: # an exported symbol in the code
            continue
        start = symt + (name_off & 0x0FFFFFFF)
        if data[start:data.index(b"\0", start)].decode() == name:
            return data[code + val:data.index(b"\0", code + val)].decode()
    return None

# "aliases": the names of before (ENGINE_ALIAS) of each engine of the apps, by alias: {"M-OSC/Waveforms":
# "SYNTH/Braids"}; the web app finds the engine of a shared patch by them (js/patchview.js), whose ids are of its names
def engine_aliases(bin_file, names):
    with open(bin_file, "rb") as f:
        data = f.read()
    aliases = app_constant(data, "__engine_aliases") or app_constant(data, "__engine_alias") or ""
    out = {}
    for engine, list_ in zip(names, aliases.split(";")):
        for a in list_.split("|"):
            if a.split("=")[0]:
                out[a.split("=")[0]] = engine
    return out

# "blobs": the engines an app makes from flash blobs (a name "<CATEGORY>/@<magic>", DRUMKITS/@!RC8): each file of app/
# that begins with the magic (app/DRUMKITS/TR-808/TR-808.rc8), its engine named as the firmware names it,
# <CATEGORY>/<file name without its extension> (udynlink_engine.cpp add()):
# {"DRUMKITS/TR-808": {"file": "DRUMKITS/TR-808/TR-808.rc8", "app": "DRUMKITS/DrumSynth/DrumSynth.bin"}}
def blob_engines(engines):
    magics = {n[n.index("/@") + 2:].encode(): (app, n[:n.index("/@") + 1])
              for app, names in engines.items() for n in names if "/@" in n}
    blobs = {}
    for root, _, files in sorted(os.walk(workingdir)):
        for f in sorted(files):
            if f.endswith((".bin", ".elf", ".json")):
                continue
            path = os.path.join(root, f)
            with open(path, "rb") as fd:
                head = fd.read(16)
            for magic, (app, category) in magics.items():
                if head.startswith(magic):
                    stem, ext = os.path.splitext(f)
                    name = category + (stem if 0 < len(ext) < 6 else f)
                    blobs[name] = {"file": os.path.relpath(path, workingdir).replace(os.sep, '/'), "app": app}
    return blobs

def write_index():
    apps = sorted(
        os.path.relpath(os.path.join(root, f), workingdir).replace(os.sep, '/')
        for root, _, files in os.walk(workingdir)
        for f in files if f.endswith(".bin")
    )
    # "engines": the engines of each app as the module lists them (CATEGORY/Name), for the web app's emulator
    engines = {app: engine_names(os.path.join(workingdir, app)) for app in apps}
    aliases = {}
    for app in apps:
        aliases.update(engine_aliases(os.path.join(workingdir, app), engines[app]))
    with open(os.path.join(workingdir, "index.json"), "w") as f:
        json.dump({"apps": apps, "engines": engines, "blobs": blob_engines(engines), "aliases": dict(sorted(aliases.items()))},
                  f, indent=2)
        f.write("\n")

# python dump.py --index: only app/index.json (no table, no doc/engines.png)
if sys.argv[1:] == ["--index"]:
    write_index()
    sys.exit(0)

print("%-35s %10s %10s  %8s" % ("FILE_NAME", "BIN_SIZE", "RAM_SIZE", "CRC32"))
total_size = 0
for bin_file in sorted(glob.glob( workingdir + "*/*/*.bin", recursive=True)): # sort
    bin_size, ram_size, crc32sum = dump(bin_file)
    total_size += bin_size

print("%-35s %10d" % ("# TOTAL BIN_SIZE", total_size))

# write index.json
write_index()


# doc/engines.png: the screenshots of the apps side by side (like ImageMagick's montage -tile 5x -geometry +10+10)
MONTAGE_GROUPS = ["GND", "CV", "DRUMKITS", "DRUMS", "SYNTH", "XX-303", "NOISE", "FX", "MIDI", "FILTER", "SEQ"]

def montage(out_file, columns=5, border=10, background="#25282c"):
    try:
        from PIL import Image
    except ImportError:
        print("Pillow not installed: %s not updated" % out_file)
        return
    files = [f for g in MONTAGE_GROUPS for f in sorted(glob.glob(workingdir + g + "/*/*.png"))]
    images = [Image.open(f).convert("RGBA") for f in files]
    if not images:
        return
    cw = max(i.width for i in images) + 2 * border
    ch = max(i.height for i in images) + 2 * border
    rows = (len(images) + columns - 1) // columns
    sheet = Image.new("RGBA", (cw * columns, ch * rows), background)
    for n, img in enumerate(images):
        x = (n % columns) * cw + (cw - img.width) // 2
        y = (n // columns) * ch + (ch - img.height) // 2
        sheet.alpha_composite(img, (x, y))
    sheet.convert("RGB").save(out_file, optimize=True)

montage(os.path.join(workingdir, "..", "doc", "engines.png"))

# python dump.py FILE.png ...: scale screenshots to 256x128 (nearest neighbour), in place
for png in sys.argv[1:]:
    from PIL import Image
    Image.open(png).resize((256, 128), Image.NEAREST).save(png)
