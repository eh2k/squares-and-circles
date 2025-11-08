#!/usr/bin/env python3

import usb
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


os.chdir(os.path.dirname(os.path.abspath(__file__)))

dev = usb.core.find(idVendor=0x16C0)

if dev is None:
    print("---- S&C Device not connected ---")
    exit(1)

print("---- S&C Device connected ---")

# try:
#     print("resetting device")
#     dev.reset()
# except Exception as e:
#     print("reset", e)

if dev.is_kernel_driver_active(0):
    print("detaching kernel driver")
    dev.detach_kernel_driver(0)

#dev.set_configuration()
cfg = dev.get_active_configuration()

hid_interface = None
for intf in cfg:
    if intf.bInterfaceClass == 3:
        hid_interface = intf
        break

if hid_interface is None:
    raise ValueError("No HID interface found")

print(hid_interface)
# detach kernel driver for that interface if necessary and claim it
if dev.is_kernel_driver_active(hid_interface.bInterfaceNumber):
    try:
        dev.detach_kernel_driver(hid_interface.bInterfaceNumber)
    except usb.core.USBError:
        pass

usb.util.claim_interface(dev, hid_interface.bInterfaceNumber)

# find IN and OUT endpoints
endpoint_in = usb.util.find_descriptor(
    hid_interface,
    custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress)
    == usb.util.ENDPOINT_IN,
)
endpoint_out = usb.util.find_descriptor(
    hid_interface,
    custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress)
    == usb.util.ENDPOINT_OUT,
)

if endpoint_in is None or endpoint_out is None:
    raise ValueError("Could not find both IN and OUT endpoints for HID interface")

print(
    "HID interface",
    hid_interface.bInterfaceNumber,
    "IN",
    hex(endpoint_in.bEndpointAddress),
    "OUT",
    hex(endpoint_out.bEndpointAddress),
)

print("-------------------------------------------------")

def get_blobs():
    endpoint_out.write("blobs".encode() + bytes([0]))
    buffer = bytearray()
    while True:
        buffer.extend(dev.read(endpoint_in.bEndpointAddress, 64, 1000).tobytes())
        if buffer[-1] == 0:
            break
    n = buffer.index(0)
    buffer = buffer[:n]

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
    
    return blobs

def update_blob(filename):
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
        endpoint_out.write(chunk)
    r = dev.read(endpoint_in.bEndpointAddress, 64, 10000).tobytes()
    crc_ret = int.from_bytes(r[4:8], "little")
    print("FLASH RESULT:", "%X" % crc32, "%X" % crc_ret)
    return True

def reset():
    cmd = bytearray()
    cmd.extend("ui:".encode() + bytes([127]))
    cmd.extend(bytes(0x0 for _ in range(64 - (len(cmd) % 64))))
    endpoint_out.write(cmd)
    print("Reset command sent")
    #dev.read(endpoint_in.bEndpointAddress, 64, 1000).tobytes()

updated = False
map = {}
for blob in get_blobs():
    map[blob["name"]] = blob
    if blob["crc32"] != blob["crc32_local"]:
        print(
            bcolors.WARNING,
            blob,
            bcolors.ENDC,
        )
        updated |= update_blob(blob["name"])
    #else:
    #    print(bcolors.OKBLUE, blob, bcolors.ENDC)

# glob for .bin files in current directory
for local_file in glob.glob("*/*.bin"):
    if local_file.endswith(".bin") and local_file not in map:
        print(bcolors.WARNING, "NEW FILE:", local_file, bcolors.ENDC)
        updated |= update_blob(local_file)

if updated:
    reset()
else:
    print("all apps up to date")

