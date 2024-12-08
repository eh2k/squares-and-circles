#!/bin/python3 -u

import rtmidi
import json
import os
import zlib
import time
import intelhex  # pip install intelhex - #https://python-intelhex.readthedocs.io/en/latest/part2-2.html
import os, io

midiout = rtmidi.MidiOut()

for i in range(0, midiout.get_port_count()):
    if midiout.get_port_name(i).startswith("S&C"):
        midiout.open_port(i)

midiin = rtmidi.MidiIn(queue_size_limit=1024 * 8)

for i in range(0, midiin.get_port_count()):
    if midiin.get_port_name(i).startswith("S&C"):
        midiin.open_port(i)

if not midiout.is_port_open() or not midiin.is_port_open():
    print("=== no S&C device conected! ===")
    exit(-1)

while True:
    msg = midiin.get_message()
    if not msg:
        break


def flush(midiin):
    while True:
        msg = midiin.get_message()
        if not msg:
            break


def chunk_bytes(bytes_object, chunk_size):
    chunks = (
        bytes_object[i : i + chunk_size]
        for i in range(0, len(bytes_object), chunk_size)
    )
    return chunks


def sendFLASHDATA(name, data0):
    flush(midiin)
    data = bytes(name, "utf-8")[:8] + data0
    crc32 = zlib.crc32(data)
    print(" Flashing", name, crc32, end="")
    midiout.send_message(
        [0xF3, 0x7E]
    )  # midi_out.send(mido.Message('song_select', song=0x7e))
    flush(midiin)
    vlen = len(data)
    ch = 0 + ((vlen & 0xF000) >> 12)
    rawValue = [0b11100000 | ch, vlen & 0x7F, vlen >> 7 & 0x7F]

    midiout.send_message(rawValue)

    while True:
        msg = midiin.get_message()
        if msg:
            ack, t = msg
            print("  ACK:", ack, end="")
            break

    print("  sending blob...", len(data), end="")
    i = 0
    time.sleep(1 / 10000)
    while i < len(data) - 1:
        int16 = data[i] | data[i + 1] << 8
        ch = 0 + ((data[i + 1] & 0xF0) >> 4)
        rawValue = [0b11100000 | ch, int16 & 0x7F, int16 >> 7 & 0x7F]
        midiout.send_message(rawValue)
        time.sleep(1 / 10000)
        i += 2

    flush(midiin)
    time.sleep(1 / 10000)

    rawValue = [0b11100000, crc32 & 0x7F, crc32 >> 7 & 0x7F]
    midiout.send_message(rawValue)

    while True:
        msg = midiin.get_message()
        if msg:
            data, t = msg
            print("  CRC_CHECK:", data)
            break


midiout.send_message([0xF3, ord("E")])

engines = ""

try:
    while True:
        msg = midiin.get_message()
        if msg:
            data, t = msg
            c = (data[2] << 7 | data[1]) - 8192
            if c == 0:
                break
            engines += chr(c)
except KeyboardInterrupt:
    print("")
finally:
    print("OK")

engines = json.loads(engines)

print(json.dumps(engines, indent=4))
# exit(0)

# del engines[len(engines)-1:]
midiout.send_message([0xF3, ord("U")])  # reset


def get_appid(binfile):
    with open(binfile, "rb") as f:
        data = f.read()
        l = int.from_bytes(data[4:6], byteorder="little")  # num_lot
        r = int.from_bytes(data[6:8], byteorder="little")  # num_rels
        a = int.from_bytes(data[8:12], byteorder="little")  # symt_size
        b = int.from_bytes(data[12:16], byteorder="little")  # code_size
        c = int.from_bytes(data[16:20], byteorder="little")  # data_size
        d = int.from_bytes(data[20:24], byteorder="little")  # bss_size
        sym_off = int(int(24) + (r * 2 * 4))
        name_off = (
            int.from_bytes(data[sym_off + 4 : sym_off + 8], "little", signed=False)
            & 0x0FFFFFFF
        ) + sym_off
        name = (
            data[name_off : data.index(0, name_off)].decode("utf-8").split("\0")[0]
        )
        return name


apps_json = os.path.dirname(__file__) + "/index.json"
with open(apps_json) as f:

    apps = json.load(f)
    j = 0
    for file in apps["apps"]:
        bin_file = os.path.dirname(apps_json) + "/" + str(file)
        if not os.path.exists(bin_file):
            continue
        app_id = get_appid(bin_file)  # os.path.splitext(file)[0]
        bin_size = os.path.getsize(bin_file)
        with open(bin_file, "rb") as f:
            crc32sum = zlib.crc32(f.read())

        engine = next(
            (e for e in engines if e["id"] == app_id),
            None,
        )

        if engine == None:

            offset = int(1024 * 1024 / 2)
            for e in engines:
                offset = int(e["addr"], 16) + int(e["size"])
            offset += 4096 - (offset % 4096)
            print("OFFSET %x" % offset)
            engine = {}
            engine["id"] = app_id
            engine["addr"] = "%x" % offset
            engine["size"] = "%s" % bin_size
            engine["crc32"] = "%x" % crc32sum
            engines.append(engine)
            print("NEW ->", file, engine)
            continue
            #exit(0)
        elif engine["crc32"] == "%x" % crc32sum:
            onext = int(engine["addr"], 16) + int(engine["size"])
            onext += 4096 - (onext % 4096)
            print(
                engine["addr"], engine["size"],
                os.path.splitext(file)[0],
                "%x" % crc32sum,
                "OK!",
                bin_size - int(engine["size"]),
                "MEM-KB: %d" % ((onext - int(1024 * 1024 / 2)) / 1024)
            )
            continue

        print("->", file, engine)

        print(
            os.path.splitext(file)[0], "%x" % crc32sum, bin_size - int(engine["size"])
        )
        offset = 0
        ih = intelhex.IntelHex()
        ih.loadbin(bin_file, offset=offset)
        offset += bin_size
        ih.puts(offset, crc32sum.to_bytes(4, "little"))
        offset += 4

        with io.BytesIO() as w:
            ih.tofile(w, format="bin")
            abx = w.getvalue()
            offset = int(engine["addr"], 16)  # (1024 * 1024)
            for chunk in chunk_bytes(abx, 4096):
                sendFLASHDATA(f"0x%6x" % offset, chunk)
                offset += len(chunk)

        j += 1

        offset += 4 - (offset % 4)

    if len(engines) > 0:
        offset = max((int(e["addr"], 16) + int(e["size"])) for e in engines)
        offset += 4096 - (offset % 4096)
        print("END 0x%x" % offset)
        for chunk in chunk_bytes(bytearray(b"\xff") * 4096, 4096):
            sendFLASHDATA(f"0x%6x" % offset, chunk)
            offset += len(chunk)

    if j >= 0:
        midiout.send_message([0xF3, 0x7F])  # reset

midiin.close_port()
midiout.close_port()
del midiin
del midiout
