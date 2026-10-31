#!/usr/bin/env python3
"""Upload the apps built here to every attached S&C module (OCT40, OCT41, DSM0) over USB (app/build.sh --upload).

  python3 app/upload.py [-d DEV] [-n] [--no-new] [--no-reset]

  -d DEV        only the modules whose USB product/serial string contains DEV, or "dsm0" ("dsm"), "teensy"
  -n            only show what would be written
  --no-new      only update the apps the module has, install none
  --no-reset    no "ui:reset" afterwards

The apps are app/<GROUP>/<Name>/<Name>.bin (and app/<GROUP>/<Name>.bin), on the module <GROUP>/<Name>.bin. Every
module lists its blobs with their CRC32s ("blobs"): an app here with another CRC32 is written in place, one the module
does not have is appended after its apps (WRI over the vendor interface, machine/src/usb_com.cpp). The firmware takes
an app in place only on as many 4 KB sectors as its place has, and a new one only as far as the flash up to the blob
index has room, a sector kept for each patch slot not saved yet (machine::app_blob_fits()); what it does not take
stays as it is on the module. The web app's [Update all] writes the chain of apps again for those, so does a firmware
image with the apps (machine/make_OCT40.sh / make_OCT41.sh FULL). Nothing else is touched: no app is removed, the
drumkits, ROMs and patches stay. WRI unloads the four engines; "ui:reset" afterwards loads the patch again and
registers the new apps.

Needs pyusb (app/requirements.txt). Without a udev rule for the modules (Linux) their nodes in /dev/bus/usb must be
writable: the flasher's "udev-rules" prints one (flasher/README.md).
"""

import argparse
import glob
import json
import os
import sys
import zlib

try:
    import usb.core
    import usb.util
except ImportError as e:
    sys.exit("%s: pyusb is required (pip install pyusb, app/requirements.txt)" % e)

APP_DIR = os.path.dirname(os.path.abspath(__file__))
WARNING, OK, ENDC = ("\033[93m", "\033[92m", "\033[0m") if sys.stdout.isatty() else ("", "", "")
VENDORS = {0x0483: "dsm0", 0x16C0: "teensy"}
VENDOR_CLASS = (0xFF, 0x6A)  # the vendor bulk interface of the firmware
SECTOR = 4096
TIMEOUT_MS = 5000
ELSEWHERE = "the web app's [Update all] writes the apps again, or a firmware image with the apps"


class UploadError(Exception):
    pass


def local_apps():
    """name on the module -> file here: app/<GROUP>/<Name>/<Name>.bin is the app <GROUP>/<Name>.bin."""
    apps = {os.path.relpath(f, APP_DIR).replace(os.sep, "/"): f
            for f in sorted(glob.glob(os.path.join(APP_DIR, "*", "*.bin")))}
    for f in sorted(glob.glob(os.path.join(APP_DIR, "*", "*", "*.bin"))):
        group, name, fn = os.path.relpath(f, APP_DIR).split(os.sep)
        if fn == name + ".bin":
            apps[group + "/" + fn] = f
    return apps


def _str(dev, idx, attr):
    # the kernel's copy where there is one (Linux): a module that is copying its apps can let the request time out
    try:
        port = "%d-%s" % (dev.bus, ".".join(str(p) for p in dev.port_numbers))
        return open("/sys/bus/usb/devices/%s/%s" % (port, attr)).read().strip()
    except (OSError, TypeError):
        pass
    try:
        return usb.util.get_string(dev, idx) or "" if idx else ""
    except (usb.core.USBError, ValueError):
        return ""


class Module:
    """A module's vendor interface: a command is a string with a NUL, a reply runs up to the next NUL."""

    def __init__(self, dev, ifnum):
        self.dev, self.ifnum = dev, ifnum
        self.kind = VENDORS[dev.idVendor]
        self.product = _str(dev, dev.iProduct, "product")
        self.serial = _str(dev, dev.iSerialNumber, "serial")
        self.id = self.product.split()[-1] if self.product.split() else "?"  # "S&C <id>"
        # the OCT41's firmware puts 0x14 in the first byte of its id, the OCT40's is the Teensy's MAC (04:E9:E5)
        self.name = "DSM0" if self.kind == "dsm0" else "OCT41" if self.id.startswith("14") else "OCT40"
        self.rx = bytearray()

    def describe(self):
        return "%s %s (bus %d dev %d)" % (self.name, self.id, self.dev.bus, self.dev.address)

    def open(self):
        intf = self.dev.get_active_configuration()[(self.ifnum, 0)]
        usb.util.claim_interface(self.dev, self.ifnum)
        ep = lambda d: usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(
            e.bEndpointAddress) == d)
        self.ep_out, self.ep_in = ep(usb.util.ENDPOINT_OUT), ep(usb.util.ENDPOINT_IN)
        self.drain()

    def close(self):
        try:
            usb.util.release_interface(self.dev, self.ifnum)
            usb.util.dispose_resources(self.dev)
        except usb.core.USBError:
            pass  # gone after the reset

    def drain(self):
        # leftovers of an aborted transfer would be taken as the next reply
        self.rx = bytearray()
        try:
            while self.ep_in.read(512, 50):
                pass
        except usb.core.USBTimeoutError:
            pass

    def send(self, data):
        self.ep_out.write(data.encode() + b"\0" if isinstance(data, str) else data, TIMEOUT_MS)

    def read_nul(self, timeout=TIMEOUT_MS):
        # what came after the reply is kept for the next: two replies can come in one transfer (WRI's "OK"s)
        while 0 not in self.rx:
            self.rx += bytes(self.ep_in.read(512, timeout))
        k = self.rx.index(0)
        data, self.rx = bytes(self.rx[:k]), self.rx[k + 1:]
        return data.decode(errors="replace")

    def query_json(self, cmd, timeout=TIMEOUT_MS):
        self.send(cmd)
        text = self.read_nul(timeout)
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            raise UploadError("%s: %r" % (cmd, text[:200]))


def modules(selector):
    """The attached modules with the vendor interface (a Teensy's HalfKay and the STM32's ROM DFU have the same vendor
    ids, but not it), those of -d DEV only."""
    s = (selector or "").lower()
    s = "dsm0" if s == "dsm" else s
    found = []
    for dev in usb.core.find(find_all=True):
        if dev.idVendor not in VENDORS:
            continue
        try:
            ifnums = [i.bInterfaceNumber for cfg in dev for i in cfg
                      if (i.bInterfaceClass, i.bInterfaceSubClass) == VENDOR_CLASS]
        except usb.core.USBError:
            continue
        if not ifnums:
            continue
        m = Module(dev, ifnums[0])
        if s and s != m.kind and s not in m.product.lower() and s not in m.serial.lower():
            continue
        found.append(m)
    return sorted(found, key=lambda m: (m.name, m.id))


def blob_header(name, data):
    """The header usb_com.cpp/read_blob_header() reads: BLOB (sent as WRI), the name padded to 4 bytes (4 more when
    it already is), the size and the CRC32."""
    head = bytearray(b"WRI\0" + name.encode() + b"\0")
    head += bytes(4 - len(head) % 4)
    return head + len(data).to_bytes(4, "little") + zlib.crc32(data).to_bytes(4, "little")


def sectors(total):
    """The sectors the boot steps over for a blob of the chain (a + 4096 - a % 4096: one more when it ends on one)."""
    return (total + SECTOR - total % SECTOR) // SECTOR


def free_sectors(blobs):
    """About the sectors the firmware takes new apps on (machine::app_blob_fits()): from the end of the chain of apps up
    to the lowest blob of the blob index (the patches, ROM copies: on a sector each), less one for each patch slot not
    saved yet; None without a chain. Copies the boot ignores after the chain are not in "blobs": the firmware may have
    less."""
    ptr = lambda b: int(b["ptr"], 16)
    index = next((b for b in blobs if b["name"] == ".index"), None)
    chain = [b for b in blobs if not b["name"].startswith(".") and ptr(b) % SECTOR]  # an app starts after its header
    if index is None or not chain:
        return None
    head = lambda b: len(blob_header(b["name"], b""))
    end = max(ptr(b) - head(b) + sectors(head(b) + int(b["len"])) * SECTOR for b in chain)
    below = [b for b in blobs if not b["name"].startswith(".") and ptr(b) % SECTOR == 0 and ptr(b) < ptr(index)]
    index_begin = min([ptr(index)] + [ptr(b) for b in below])
    saved = sum(1 for b in below if b["name"] in ["Patch-%d" % i for i in range(1, 9)])
    return (index_begin - end) // SECTOR - (8 - saved)  # Patch-1..8, a sector each (machine_internals.h)


def upload(m, name, data, expect):
    """WRI of the app NAME; the module's report, UploadError when it refuses it or does something else. expect: how
    the report ends, "updated!" in place, "stored!" for an app appended."""
    head = blob_header(name, data)
    total = len(head) + len(data)
    # up to the next 64 only: a whole chunk of 0xFF after the blob would be taken for a command
    msg = head + data + b"\xff" * ((64 - (len(head) + len(data)) % 64) % 64)
    # every read of the module is answered by "OK <received>/<total>" (the DSM0 reads its 64-byte packets as they
    # come). The first packet alone: the module refuses an app that does not fit with it ("ERR BLOB ... does not fit",
    # machine::app_blob_fits()) and would take the packets after it as commands.
    got = 0
    for i, n in [(0, 64)] + [(i, 512) for i in range(64, len(msg), 512)]:
        m.send(bytes(msg[i:i + n]))
        while got < min(i + n, total):
            r = m.read_nul(30000)
            if not r.startswith("OK "):
                raise UploadError(r)
            got = int(r[3:].split("/")[0])
    # then the flash's: "CRC32 ...", "BLOB <name> @ <addr> updated!" (appended: "END 0x... emptied", "New BLOB <name> @
    # <addr> stored!")
    r = m.read_nul(60000).strip()
    if not r.endswith(expect) or "not updated" in r or "MISMATCH" in r:
        raise UploadError(r)
    return r


def update(m, apps, opts):
    """Updates one module: the names it could not write, with why, or None when the module was not reached."""
    try:
        m.open()
    except usb.core.USBError as e:  # "Resource busy": another program has it (the web app, sc_usb.py)
        print("  %snot opened: %s%s" % (WARNING, e, ENDC))
        return None
    problems, written, tried = [], 0, False
    try:
        blobs = m.query_json("blobs", 20000)
        on_module = {}
        for b in blobs:  # the first of a name, the one the boot registers
            on_module.setdefault(b["name"], b)
        datas = {name: open(path, "rb").read() for name, path in apps.items()}
        changed = [n for n in apps if n in on_module and int(on_module[n]["crc32"], 16) != zlib.crc32(datas[n])]
        new = [n for n in apps if n not in on_module]
        if new and opts.no_new:
            print("  %d apps not on the module (--no-new):\n\t%s" % (len(new), "\n\t".join(new)))
            new = []
        if not changed and not new:
            print("  %sall %d apps up to date%s" % (OK, sum(1 for n in apps if n in on_module), ENDC))
            return problems

        free = free_sectors(blobs)
        if opts.dry_run and new and free is not None:
            print("  about %d sectors of 4 KB free for new apps" % max(free, 0))
        for name in changed + new:
            data, is_new = datas[name], name not in on_module
            label = "  %-26s %7d bytes  " % (name, len(data))
            need = sectors(len(blob_header(name, data)) + len(data))
            if not is_new:
                # in place only on as many sectors as its place: the boot steps from one blob to the next by their
                # sizes (machine::app_blob_fits())
                place = sectors(len(blob_header(name, data)) + int(on_module[name]["len"]))
                if need != place:
                    why = "%d sectors, its place has %d: %s" % (need, place, ELSEWHERE)
                    print(label + WARNING + "NOT updated: " + why + ENDC)
                    problems.append((name, why))
                    continue
            if opts.dry_run:
                if is_new and free is not None and need > free:
                    why = "no room: %d sectors, about %d free" % (need, max(free, 0))
                    print(label + WARNING + why + ENDC)
                    problems.append((name, why))
                    continue
                print(label + ("would be appended (%d sectors)" % need if is_new else "would be updated"))
                free = free - need if is_new and free is not None else free
                continue
            tried = True
            try:
                r = upload(m, name, data, "stored!" if is_new else "updated!")
                print(label + OK + r.splitlines()[-1] + ENDC)
                written += 1
            except UploadError as e:
                why = str(e) + ("; " + ELSEWHERE if "does not fit" in str(e) else "")
                print(label + WARNING + "NOT %s: %s" % ("installed" if is_new else "updated", why) + ENDC)
                problems.append((name, why))
                m.drain()
    except UploadError as e:
        print("  %s%s%s" % (WARNING, e, ENDC))
        problems.append(("blobs", str(e)))
    except usb.core.USBError as e:
        # a transfer that stopped half way: the module may still wait for the rest of a blob, it would take
        # "ui:reset" for its data
        print("  %sUSB error: %s; reset the module (power or the button) before app/upload.py again%s"
              % (WARNING, e, ENDC))
        problems.append(("USB", str(e)))
        tried = False
    finally:
        if tried and not opts.no_reset:
            try:
                m.send("ui:reset")
                print("  reset (%d written)" % written)
            except usb.core.USBTimeoutError as e:
                print("  %sno reset: %s; reset the module (power or the button)%s" % (WARNING, e, ENDC))
                problems.append(("reset", str(e)))
            except usb.core.USBError:
                # gone before the host had the transfer back (the DSM0 behind a hub's transaction translator): taken
                print("  reset (%d written)" % written)
        elif tried:
            print("  %sno reset (--no-reset): reset the module, its \"blobs\" are the old ones until it boots%s"
                  % (WARNING, ENDC))
        m.close()
    return problems


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-d", "--device")
    ap.add_argument("-n", "--dry-run", action="store_true")
    ap.add_argument("--no-new", action="store_true")
    ap.add_argument("--no-reset", action="store_true")
    opts = ap.parse_args()

    found = modules(opts.device)
    if not found:
        sys.exit("no S&C module found" + (" matching '%s'" % opts.device if opts.device else ""))
    apps = local_apps()

    failed = []
    for m in found:
        print("-------------------------------------------------")
        print(m.describe())
        problems = update(m, apps, opts)
        if problems is None:
            failed.append("%s: not reached" % m.describe())
        else:
            failed += ["%s %s: %s" % (m.name, m.id, name) for name, _ in problems]
    print("-------------------------------------------------")
    if failed:
        print("%sNOT up to date:%s\n  %s" % (WARNING, ENDC, "\n  ".join(failed)))
        sys.exit(1)
    print("%d module(s) %s" % (len(found), "checked" if opts.dry_run else "up to date"))


if __name__ == "__main__":
    main()
