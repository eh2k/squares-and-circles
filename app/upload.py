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
    print(" Flashing", name, crc32)
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
            print("  ACK:", ack)
            break

    print("  sending blob...", len(data))
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

apps_json = os.path.dirname(__file__) + "/index.json"
with open(apps_json) as f:
    end = 0
    apps = json.load(f)
    j = 0
    for file in apps["apps"]:
        bin_file = os.path.dirname(apps_json) + "/" + str(file)
        if not os.path.exists(bin_file):
            continue
        bin_size = os.path.getsize(bin_file)
        with open(bin_file, "rb") as f:
            crc32sum = zlib.crc32(f.read())
            engine = next(
                (e for e in engines if e["id"].startswith(os.path.splitext(file)[0])),
                None,
            )

            if engine != None:
                end = int(engine["addr"], 16) + int(engine["size"])

            if engine != None and engine["crc32"] == "%x" % crc32sum:
                print(
                    engine["addr"],
                    os.path.splitext(file)[0],
                    "%x" % crc32sum,
                    "OK!",
                    bin_size - int(engine["size"]),
                )
                continue

        print("->", file, engine)

        if engine == None:

            offset = max((int(e["addr"], 16) + int(e["size"])) for e in engines)
            offset += 4096 - (offset % 4096)
            engine = {}
            engine["addr"] = "%x" % offset
            engine["size"] = "%s" % bin_size
            print("TODO - add new engine...", "0x%x" % offset)
            # continue

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

    if True:
        offset = end #max((int(e["addr"], 16) + int(e["size"])) for e in engines)
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
