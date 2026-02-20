#!/usr/bin/env python3

import serial
import serial.tools.list_ports
import json
import zlib
import os, glob


class bcolors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"

print("-------------------------------------------------")

os.chdir(os.path.dirname(os.path.abspath(__file__)))

ports = serial.tools.list_ports.comports()
port = None

for port, desc, hwid in sorted(ports):
    if desc.startswith("S&C"):
        print("{}: {} [{}]".format(port, desc, hwid))
        port = port
        break


if port is None:
    print(" -> S&C Device not found")
    exit(1)
else:
    print(" -> S&C Device found on port", port)

def get_blobs(serialPort):
    serialPort.write(b"blobs\0")
    buffer = serialPort.read_until(b"\0")
    buffer = buffer.decode().strip("\0")

    try:
        blobs = json.loads(buffer)
    except Exception as e:
        print(buffer)
        print("Error parsing blobs:", e)
        return []

    for blob in blobs[:]:
        if os.path.exists(blob["name"]) == False:
            del blobs[blobs.index(blob)]
            continue
        with open(blob["name"], "rb") as f:
            blob["crc32_local"] = "%X" % zlib.crc32(f.read())
    
    #print("blobs:", json.dumps(blobs, indent=2))
    return blobs

def update_blob(serialPort, filename):
    blob = bytearray()
    blob.extend("WRI".encode()[:3] + bytes([0]))
    blob.extend(filename.encode() + bytes([0]))
    blob.extend(bytes(0x0 for _ in range(4 - (len(blob) % 4))))
    blob.extend(os.path.getsize(filename).to_bytes(4, "little"))
    crc32 = 0
    with open(filename, "rb") as f:
        crc32 =zlib.crc32(f.read())
        blob.extend(crc32.to_bytes(4, "little"))
        f.seek(0)
        blob.extend(f.read())
    blob.extend(bytes(0xFF for _ in range(64 - (len(blob) % 64))))
    #return 
    for i in range(0, len(blob), 64):
        chunk = blob[i : i + 64]
        serialPort.write(chunk)
        r = serialPort.read_until(b"\0").decode()
        print(r)
        if r.startswith("ERR"):
            return False

    r = serialPort.read_until(b"\0").decode()
    print(r)

    #crc_ret = int.from_bytes(r[4:8], "little")
    #print("FLASH RESULT:", "%X" % crc32, "%X" % crc_ret)
    return True

def reset(serialPort):
    cmd = bytearray()
    cmd.extend("ui:".encode() + bytes([127]))
    cmd.extend(bytes(0x0 for _ in range(64 - (len(cmd) % 64))))
    serialPort.write(cmd)
    print("Reset command sent")
    #dev.read(endpoint_in.bEndpointAddress, 64, 1000).tobytes()

updated = False
map = {}

# open serial by vendor id 0x16C0
with serial.Serial(port, 115200, timeout=0.1, exclusive=True) as serialPort:
    if serialPort is None:
        print("-> S&C Device not connected")
        exit(1)

    print("-> S&C Device connected")

    print("-------------------------------------------------")

    for blob in get_blobs(serialPort):
        map[blob["name"]] = blob
        if blob["crc32"] != blob["crc32_local"]:
            print(
                bcolors.WARNING,
                blob,
                bcolors.ENDC,
            )
            updated |= update_blob(serialPort, blob["name"])
        #else:
        #    print(bcolors.OKBLUE, blob, bcolors.ENDC)

    # glob for .bin files in current directory
    for local_file in glob.glob("*/*.bin"):
        local_file = local_file.replace("\\", "/")
        if local_file.endswith(".bin") and local_file not in map:
            print(bcolors.WARNING, "NEW FILE:", local_file, bcolors.ENDC)
            #updated |= update_blob(local_file)

    if updated:
        reset(serialPort)
    else:
        print("all apps up to date")