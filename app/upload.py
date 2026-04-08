#!/usr/bin/env python3

import json
import zlib
import os, glob
import sys

try:
    import usb.core
    import usb.util
except ImportError:
    print("pyusb is required. Install with: pip install pyusb")
    sys.exit(1)


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

USB_VENDOR_ID = 0x16C0
USB_INTERFACE = 3
USB_ENDPOINT = 5
USB_TIMEOUT_MS = 5000


def _safe_usb_string(device, index):
    if not index:
        return ""
    try:
        return usb.util.get_string(device, index) or ""
    except usb.core.USBError:
        return ""


def _find_bulk_endpoint(interface, direction):
    for endpoint in interface:
        if usb.util.endpoint_direction(endpoint.bEndpointAddress) != direction:
            continue
        if usb.util.endpoint_type(endpoint.bmAttributes) != usb.util.ENDPOINT_TYPE_BULK:
            continue
        if endpoint.bEndpointAddress & 0x0F == USB_ENDPOINT:
            return endpoint
    for endpoint in interface:
        if usb.util.endpoint_direction(endpoint.bEndpointAddress) != direction:
            continue
        if usb.util.endpoint_type(endpoint.bmAttributes) == usb.util.ENDPOINT_TYPE_BULK:
            return endpoint
    return None


def open_device():
    devices = list(usb.core.find(find_all=True, idVendor=USB_VENDOR_ID) or [])
    if not devices:
        return None

    selected = None
    for device in devices:
        product = _safe_usb_string(device, device.iProduct)
        if product.startswith("S&C") or product.startswith("Squares&Circles"):
            selected = device
            break

    if selected is None:
        selected = devices[0]

    #selected.set_configuration()
    config = selected.get_active_configuration()
    interface = usb.util.find_descriptor(config, bInterfaceNumber=USB_INTERFACE)
    if interface is None:
        raise RuntimeError(f"USB interface {USB_INTERFACE} not found")

    if selected.is_kernel_driver_active(USB_INTERFACE):
        selected.detach_kernel_driver(USB_INTERFACE)

    usb.util.claim_interface(selected, USB_INTERFACE)

    ep_out = _find_bulk_endpoint(interface, usb.util.ENDPOINT_OUT)
    ep_in = _find_bulk_endpoint(interface, usb.util.ENDPOINT_IN)
    if ep_out is None or ep_in is None:
        usb.util.release_interface(selected, USB_INTERFACE)
        raise RuntimeError("Bulk IN/OUT endpoints not found")

    return selected, ep_out, ep_in


def close_device(device):
    if device is None:
        return
    try:
        usb.util.release_interface(device, USB_INTERFACE)
    except Exception:
        pass
    usb.util.dispose_resources(device)


device_bundle = open_device()

if device_bundle is None:
    print(" -> S&C Device not found")
    exit(1)
else:
    dev, ep_out, ep_in = device_bundle
    product = _safe_usb_string(dev, dev.iProduct)
    serial_number = _safe_usb_string(dev, dev.iSerialNumber)
    print(" -> S&C Device found:", product or "Unknown", serial_number)


def usb_write(ep_out, payload):
    ep_out.write(payload, timeout=USB_TIMEOUT_MS)


def usb_read_until_nul(ep_in, timeout_ms=USB_TIMEOUT_MS, chunk_size=512):
    data = bytearray()
    while True:
        chunk = ep_in.read(chunk_size, timeout=timeout_ms)
        for byte in chunk:
            if byte == 0:
                return data.decode(errors="replace")
            data.append(byte)


def get_blobs(ep_out, ep_in):
    usb_write(ep_out, b"blobs\0")
    buffer = usb_read_until_nul(ep_in)

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

def update_blob(ep_out, ep_in, filename):
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
    for i in range(0, len(blob), 512):
        chunk = blob[i : i + 512]
        usb_write(ep_out, chunk)
        r = usb_read_until_nul(ep_in)
        print(r)
        if r.startswith("ERR"):
            return False

    #r = usb_read_until_nul(ep_in)
    #print(r)

    #crc_ret = int.from_bytes(r[4:8], "little")
    #print("FLASH RESULT:", "%X" % crc32, "%X" % crc_ret)
    return True

def reset(ep_out):
    usb_write(ep_out, b"ui:reset\0")
    print("Reset command sent")
    #dev.read(endpoint_in.bEndpointAddress, 64, 1000).tobytes()

updated = False
map = {}

if device_bundle is None:
    print("-> S&C Device not connected")
    exit(1)

dev, ep_out, ep_in = device_bundle

try:
    print("-> S&C Device connected")

    print("-------------------------------------------------")

    for blob in get_blobs(ep_out, ep_in):
        map[blob["name"]] = blob
        if blob["crc32"] != blob["crc32_local"]:
            print(
                bcolors.WARNING,
                blob,
                bcolors.ENDC,
            )
            updated |= update_blob(ep_out, ep_in, blob["name"])
        #else:
        #    print(bcolors.OKBLUE, blob, bcolors.ENDC)

    # glob for .bin files in current directory
    for local_file in glob.glob("*/*.bin"):
        local_file = local_file.replace("\\", "/")
        if local_file.endswith(".bin") and local_file not in map:
            print(bcolors.WARNING, "NEW FILE:", local_file, bcolors.ENDC)
            #updated |= update_blob(local_file)

    if updated:
        reset(ep_out)
    else:
        print("all apps up to date")
finally:
    close_device(dev)