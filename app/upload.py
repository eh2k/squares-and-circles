#!/usr/bin/env python3
"""Bring the apps on every attached S&C module (DSM, OCT41, OCT40) to the builds in app/ (app/build.sh --upload).

  python3 app/upload.py [-d DEV] [-n] [--no-new] [--no-shrink] [--no-rewrite] [--no-reset]

  -d DEV        only the modules whose USB product/serial string contains DEV (as test_usb/sc_usb.py -d)
  -n            only show what would be written
  --no-new      only update the apps the module has, install none
  --no-shrink   remove nothing from the module
  --no-rewrite  never write the chain of apps again (see below; then nothing is removed either)
  --no-reset    no "ui:reset" afterwards

The apps a module should have are those of its full image: app/<module>.json (app/OCT40.json, rules as for
make_firmware_full.py --apps, machine/app_rules.py), without one everything here (app/<GROUP>/<Name>.bin and the
drumkits app/<GROUP>/<kit>/<kit>.rc8, e.g. app/DRUMKITS/TR-909/TR-909.rc8 -> the engine DRUMKITS/TR-909). Every module lists its
blobs with their CRC32s ("blobs"):
- an app here with another CRC32 is updated,
- an app of the module's list that it does not have is installed (not with --no-new),
- a blob of the chain of apps that the rules leave out, an app (<GROUP>/<Name>.bin) that is no longer here, and a kit
  (<GROUP>/<kit>.rc8) that is here in another group now, is removed (not with --no-shrink); the ROMs and other blobs
  not from here (samples, kits saved by the DrumSynth web-app, FV-1 banks) stay.
Updates go in place and new apps into the gap after the apps (WRI, as test_usb/sc_usb.py upload), when they fit: an
app in place only on the same number of 4 KB sectors, the new ones only as far as the gap up to the blob index keeps a
sector for each patch slot not saved yet. When they do not all fit so, or something is removed, the chain of apps is
written again from the first one that moves on, without gaps (web-app/js/remoteBulk.js chainPlan/chainWrite, the
web-app's [Update all]): the blobs not from here (the ROMs) read back from the module (FSRD), each written as firmware
pages ("0x<offset>") with its first sector last, so that a stop half way leaves a chain the boot walks. The sectors
after the new end up to where the old chain ended ("sectors": the boot's walk, the copies it ignores too) are made
empty, and so is the sector after an app put into the gap: old blobs there would join the chain again once it
reaches them. WRI unloads the four engines; "ui:reset" afterwards loads the patch again and registers the new apps.
"""

import argparse
import glob
import os
import re
import sys
import zlib

APP_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(APP_DIR, "..", "test_usb"))
sys.path.insert(0, os.path.join(APP_DIR, "..", "machine"))

try:
    import usb.core
    import sc_usb
except ImportError as e:
    sys.exit("%s: pyusb is required (pip install pyusb)" % e)
import app_rules  # noqa: E402

WARNING, OK, ENDC = ("\033[93m", "\033[92m", "\033[0m") if sys.stdout.isatty() else ("", "", "")
SECTOR = 4096
BLANK = b"\xff" * SECTOR
PATCH_SLOTS = 8  # Patch-1 .. Patch-8, a sector each (machine_internals.h)


def local_apps():
    """name on the module -> file here: the apps and the drumkits in any group (make_firmware_full.py names them the
    same)."""
    files = glob.glob(os.path.join(APP_DIR, "*", "*.bin"))
    apps = {os.path.relpath(f, APP_DIR).replace(os.sep, "/"): f for f in sorted(files)}
    # app/<GROUP>/<kit>/<kit>.rc8 is the drumkit <GROUP>/<kit>.rc8
    for f in sorted(glob.glob(os.path.join(APP_DIR, "*", "*", "*.rc8"))):
        group, name, fn = os.path.relpath(f, APP_DIR).split(os.sep)
        apps[group + "/" + fn] = f
    # app/<GROUP>/<Name>/<Name>.bin is the app <GROUP>/<Name>.bin
    for f in sorted(glob.glob(os.path.join(APP_DIR, "*", "*", "*.bin"))):
        group, name, fn = os.path.relpath(f, APP_DIR).split(os.sep)
        if fn == name + ".bin":
            apps[group + "/" + fn] = f
    return apps


def modules(selector):
    """(id, module, description) of the attached S&C modules, the id as sc_usb.Device() finds the module by."""
    found = []
    for dev, _ in sc_usb.find_devices():
        product, serial = sc_usb._str(dev, dev.iProduct), sc_usb._str(dev, dev.iSerialNumber)
        s = (selector or "").lower()
        if s and sc_usb.VENDORS[dev.idVendor] != s and s not in product.lower() and s not in serial.lower():
            continue
        mid = product.split()[-1] if product.split() else ""
        # the OCT41's firmware puts 0x14 in the first byte of its id, the OCT4's is the Teensy's MAC (04:E9:E5)
        module = "DSM" if sc_usb.VENDORS[dev.idVendor] == "dsm" else "OCT41" if mid.startswith("14") else "OCT40"
        found.append((mid, module, "%s %s (bus %d dev %d)" % (module, mid, dev.bus, dev.address)))
    return found


# --- the flash as "blobs" has it: an app of the chain starts after its header, a blob of the index on a sector ---

def ptr(b):
    return int(b["ptr"], 16)


def header_len(name):
    return len(sc_usb.blob_header(name, b""))


def step(total):
    """The bytes the boot steps over for a blob of the chain (load_engines: a + 4096 - a % 4096), one sector more
    than needed when it ends on one."""
    return total + SECTOR - total % SECTOR


def chain_of(blobs):
    return sorted((b for b in blobs if not b["name"].startswith(".") and ptr(b) % SECTOR), key=ptr)


def program_base(blobs):
    return ptr(next(b for b in blobs if b["name"] == ".loader"))


def blob_room(blobs):
    """The gap between the chain of apps and the blobs of the blob index (the patches grow down from it): {end,
    index_begin, free (sectors), kept (the sectors the patch slots not saved yet will take)}, None without a chain."""
    index = next((b for b in blobs if b["name"] == ".index"), None)
    chain = chain_of(blobs)
    if index is None or not chain:
        return None
    end = max(ptr(b) - header_len(b["name"]) + step(header_len(b["name"]) + int(b["len"])) for b in chain)
    below = [b for b in blobs if not b["name"].startswith(".") and ptr(b) % SECTOR == 0 and ptr(b) < ptr(index)]
    index_begin = min([ptr(index)] + [ptr(b) for b in below if ptr(b) >= end])
    kept = PATCH_SLOTS - sum(1 for b in below if re.fullmatch(r"Patch-[1-8]", b["name"]))
    return {"end": end, "index_begin": index_begin, "free": (index_begin - end) // SECTOR, "kept": kept}


def index_blobs_at(blobs, lo, hi):
    """The blobs of the blob index (patches, ROM copies: on a sector each) with sectors in [lo, hi). Their records
    can point into the chain of apps (OCT40, 2026-10: ROM copies under a full image that grew), the flash there is
    then theirs and the chain's at once: nothing may be written there."""
    return [b for b in blobs if not b["name"].startswith(".") and ptr(b) % SECTOR == 0
            and ptr(b) < hi and ptr(b) + max(int(b["len"]), 1) > lo]


def removal(name, rules, apps):
    """Why the blob NAME of the chain goes from the module, None if it stays: the module's rules leave it out, it is
    an app that app/ no longer has, or a kit that app/ has in another group now (moved). Other blobs not from here
    (samples, kits saved by the DrumSynth web-app as DRUMKITS/<kit>.rc8, banks made elsewhere) stay."""
    rule = app_rules.decide(name, rules)
    if rule is None:
        return "no rule takes it"
    if not rule[1]:
        return rule[0]
    if "/" in name and name.endswith(".bin") and name not in apps:
        return "not in app/"
    if "/" in name and name.endswith(".rc8") and name not in apps:
        moved = [a for a in apps if a.endswith(".rc8") and os.path.basename(a) == os.path.basename(name)]
        if moved:
            return "now " + moved[0]
    return None


def chain_plan(blobs, updates, drop=()):
    """The chain of apps written again from its start, one blob after the other without a gap: updates (name -> data)
    in place of the blobs of their names, the ones in drop left out, the others (the ROMs of a firmware image too) as
    they are on the module, then the updates not on it in their order, as far as they fit (the patch slots kept). The
    copies the boot ignores, as the second of their names, are gone then. {"problem"} or {blobs, from, end, top,
    index_begin, left_out}: blobs[i] = {name, len, crc32, addr, data?, old?} (addr = the sector its header starts on,
    data only from updates), from = the first blob that moves or changes: the ones before it stay and are not written,
    from it on the ones without data come first (what a stop half way leaves, a new run cannot install again)."""
    index = next((b for b in blobs if b["name"] == ".index"), None)
    chain = chain_of(blobs)
    room = blob_room(blobs)
    if index is None or not chain or room is None:
        return {"problem": "no chain of apps on the module"}
    base = ptr(chain[0]) - header_len(chain[0]["name"])
    if base % SECTOR:
        return {"problem": "the chain of apps starts at 0x%x, not on a sector" % base}
    top = room["index_begin"] - room["kept"] * SECTOR

    planned = []
    for b in chain:
        if b["name"] in drop:
            continue
        data = updates.get(b["name"])
        planned.append({"name": b["name"], "len": len(data), "crc32": zlib.crc32(data), "data": data, "old": b}
                       if data is not None else
                       {"name": b["name"], "len": int(b["len"]), "crc32": int(b["crc32"], 16), "old": b})
    addr = base

    def place(p):
        nonlocal addr
        p["addr"] = addr
        addr += step(header_len(p["name"]) + p["len"])

    for p in planned:
        place(p)
    # the apps already there may stay in the patch slots they reach into now (a full image up to the index), as long
    # as they do not take more of them: only the new ones keep out of them
    limit = max(top, room["end"])
    if addr > limit:
        return {"problem": "the apps on the module need %d sectors of 4 KB then, there are %d"
                % ((addr - base) // SECTOR, (limit - base) // SECTOR)}
    left_out = []
    on_chain = {b["name"] for b in chain}
    for name, data in updates.items():
        if name in on_chain:
            continue
        if addr + step(header_len(name) + len(data)) > top:
            left_out.append(name)
            continue
        p = {"name": name, "len": len(data), "crc32": zlib.crc32(data), "data": data}
        place(p)
        planned.append(p)

    start = next((i for i, p in enumerate(planned)
                  if "old" not in p or "data" in p or ptr(p["old"]) != p["addr"] + header_len(p["name"])),
                 len(planned))
    rest = planned[start:]
    del planned[start:]
    if rest:
        addr = rest[0]["addr"]  # where the ones that stay end
    for p in [p for p in rest if "data" not in p] + [p for p in rest if "data" in p]:
        place(p)
        planned.append(p)
    lo = rest[0]["addr"] if rest else addr
    inside = index_blobs_at(blobs, lo, addr + SECTOR)
    if inside:
        return {"problem": "the blob index has %s in the flash the chain of apps would be written to (0x%x..0x%x): "
                           "it would be overwritten" % (", ".join(b["name"] for b in inside), lo, addr + SECTOR)}
    return {"blobs": planned, "from": start, "end": addr, "top": top, "index_begin": room["index_begin"],
            "left_out": left_out}


def walk_end(d, blobs):
    """Where the boot's walk over the chain ended ("sectors": the CRC32s of the sectors up to there, FIRMWARE_SIZE),
    the copies it ignores as the second of their names and blobs that joined an old chain included; and those
    CRC32s by address."""
    base = program_base(blobs)
    crcs = {base + int(k, 16): int(v, 16) for k, v in d.query_json("sectors").items()}
    return (max(crcs) + SECTOR if crcs else base), crcs


def stale_sectors(blobs, new_end, old_end, index_begin, crcs):
    """The sectors after the new end of the chain (and the empty one there) up to the old one that are not empty,
    none of a blob of the index."""
    return [a for a in range(new_end + SECTOR, min(old_end, index_begin), SECTOR)
            if crcs.get(a) != zlib.crc32(BLANK) and not index_blobs_at(blobs, a, a + SECTOR)]


def chain_write(d, plan, blobs, stale=()):
    """Writes a chain_plan(): what is not in the updates first read from the module (FSRD SYSTEM/<name>, checked
    against its CRC32), then the sectors from the first blob that moves on as firmware pages ("0x<offset>"). The first
    sector is made empty first and written last: until then the chain ends before it, with the blobs before it.
    Then the stale sectors after the new end empty. The number of blobs written; raises with what the module has
    after a reset."""
    base = program_base(blobs)
    todo = plan["blobs"][plan["from"]:]
    for i, p in enumerate(todo):
        if "data" in p:
            continue
        progress("  reading %d/%d from the module: %s" % (i + 1, len(todo), p["name"]))
        d.send("FSRDSYSTEM/" + p["name"])
        data = d.read_n(p["len"], 10000) if p["len"] else b""
        if zlib.crc32(data) != p["crc32"]:
            raise sc_usb.UploadError("%s: read back from the module with CRC32 %X, not %X"
                                     % (p["name"], zlib.crc32(data), p["crc32"]))
        p["data"] = data

    first = todo[0]["addr"] if todo else plan["end"]
    image = bytearray(BLANK * ((plan["end"] - first) // SECTOR))
    for p in todo:
        o = p["addr"] - first
        head = b"BLOB" + bytes(sc_usb.blob_header(p["name"], p["data"])[4:])  # WRI -> BLOB, as the boot reads it
        image[o:o + len(head)] = head
        image[o + len(head):o + len(head) + p["len"]] = p["data"]

    # one blob after the other: its sectors after the first, the sector after it empty (the end of the chain for now,
    # the end for good after the last unless the index blobs start there), then its first one, the one with the header
    writes = [(first, True, None)] if todo else []
    for k, p in enumerate(todo):
        nxt = todo[k + 1]["addr"] if k + 1 < len(todo) else plan["end"]
        writes += [(a, False, None) for a in range(p["addr"] + SECTOR, nxt, SECTOR)]
        if nxt < plan["index_begin"]:
            writes.append((nxt, True, None))
        writes.append((p["addr"], False, p["name"]))
    if not todo and plan["end"] < plan["index_begin"]:
        writes.append((plan["end"], True, None))  # the chain only shorter: it ends there from now on
    writes += [(a, True, None) for a in stale]

    done = None  # the last blob written whole
    for i, (a, blank, name) in enumerate(writes):
        data = BLANK if blank else bytes(image[a - first:a - first + SECTOR])
        progress("  writing %d apps again: sector %d/%d%s" % (len(todo), i + 1, len(writes),
                                                              ", %s done" % done if done else ""))
        try:
            sc_usb.upload(d, "0x%06x" % (a - base), data, "stored!")
        except (sc_usb.UploadError, usb.core.USBError) as e:
            if not todo or done == todo[-1]["name"]:  # only empty sectors after the chain were left
                raise type(e)("%s; the chain of apps is written, sectors after it are not empty yet" % e)
            raise type(e)("%s; after a reset the module has the apps %s of the new order, app/upload.py then "
                          "installs the others again" % (e, "up to " + done if done else "before " + todo[0]["name"]))
        if name:
            done = name
    progress("")
    return len(todo)


def flash_text(used, free, room):
    """The flash of the apps: used, free for new ones (the patch slots not saved yet kept), and all of it from the
    first app up to the blob index."""
    kb = lambda n: n // 1024
    return "%d KB used, %d KB free of %d KB (up to the blob index, %d KB kept for %d patches not saved yet)" % (
        kb(used), kb(free), kb(room["index_begin"] - room["base"]), kb(room["kept"] * SECTOR), room["kept"])


def progress(text):
    if sys.stdout.isatty():
        sys.stdout.write("\r\033[K" + text)
        sys.stdout.flush()


def update(mid, module, apps, opts):
    """Updates one module; the names it could not update, with why, or None when the module was not reached."""
    try:
        d = sc_usb.Device(mid)
    except SystemExit as e:
        print("  " + WARNING + str(e) + ENDC)
        return None
    except usb.core.USBError as e:  # "Resource busy": another program has it (the web-app, sc_usb.py)
        print("  %snot opened: %s%s" % (WARNING, e, ENDC))
        return None
    problems, written, reset = [], 0, False
    try:
        blobs = d.query_json("blobs")
        if not isinstance(blobs, list):
            print("  %s\"blobs\": %s%s" % (WARNING, blobs, ENDC))
            return None
        on_module = {}
        for b in blobs:  # the first of a name, as the firmware's flash_read() finds it (the chain before the index)
            on_module.setdefault(b["name"], b)
        rules_path = app_rules.module_json(module)
        rules = app_rules.load(rules_path) if rules_path else app_rules.TAKE_ALL
        wanted = [name for name in apps if app_rules.takes(name, rules)]
        print("  apps: %s, %d of the %d here" % (os.path.relpath(rules_path) if rules_path else "all of app/",
                                                 len(wanted), len(apps)))
        matched = set()
        for name in set(apps) | set(app_rules.ROMS) | set(on_module):
            app_rules.decide(name, rules, matched)
        for rule in rules:
            if rule[0] is not None and rule not in matched:
                print("  %sWARNING: %s (%s) matches no app/blob%s" % (WARNING, rule[0], os.path.relpath(rules_path),
                                                                      ENDC))

        # the flash of the apps: from the first app up to where the boot's walk ends (the copies it ignores too, as
        # machine::app_blob_fits() counts) and from there up to the blob index, less the patch slots not saved yet
        room = blob_room(blobs)
        if room:
            chain = chain_of(blobs)
            room["base"] = ptr(chain[0]) - header_len(chain[0]["name"])
            old_end, crcs = walk_end(d, blobs)
            room["walk_end"] = max(old_end, room["end"])
            top = room["index_begin"] - room["kept"] * SECTOR
            print("  flash for apps: " + flash_text(room["walk_end"] - room["base"], top - room["walk_end"], room) +
                  (", %d KB of it blobs the boot ignores" % ((room["walk_end"] - room["end"]) // 1024)
                   if room["walk_end"] > room["end"] else ""))
            inside = index_blobs_at(blobs, room["base"], room["walk_end"])
            if inside:
                print("  %sWARNING: the blob index has %s inside the chain of apps (their records point at the apps' "
                      "flash): nothing is written there%s" % (WARNING, ", ".join(b["name"] for b in inside), ENDC))

        datas = {name: open(path, "rb").read() for name, path in apps.items()}
        removals = {} if opts.no_shrink else \
            {b["name"]: why for b in chain_of(blobs) for why in [removal(b["name"], rules, apps)] if why}
        missing = [name for name in wanted if name not in on_module]
        if missing and opts.no_new:
            print("  %d of them not on the module (--no-new):\n\t%s" % (len(missing), "\n\t".join(missing)))
        candidates = [name for name in apps if name not in removals and
                      (name in on_module and int(on_module[name]["crc32"], 16) != zlib.crc32(datas[name])
                       or name in missing and not opts.no_new)]
        if not candidates and not removals:
            print("  %sall %d apps up to date%s" % (OK, sum(1 for name in apps if name in on_module), ENDC))
            return problems

        # in place, the new ones one after the other into the gap, as far as they fit
        usable = (room["index_begin"] - room["walk_end"]) // SECTOR - room["kept"] if room else float("inf")
        todo, why = [], {}
        for name in candidates:
            is_new = name not in on_module
            why[name] = sc_usb.upload_problem(name, datas[name], blobs, is_new)
            need = step(header_len(name) + len(datas[name])) // SECTOR if is_new else 0
            if why[name] is None and need > usable:
                why[name] = "needs %d sectors of 4 KB, %d left in the gap after the ones before it (%d kept for the " \
                            "patches)" % (need, usable, room["kept"])
            if why[name] is None:
                usable -= need
                todo.append(name)

        # else the chain of apps again: to remove blobs, or if it then takes more of the apps
        plan = None
        if (removals or len(todo) < len(candidates)) and not opts.no_rewrite:
            p = chain_plan(blobs, {name: datas[name] for name in candidates}, set(removals))
            if "problem" in p:
                print("  %sthe chain of apps not written again: %s%s" % (WARNING, p["problem"], ENDC))
            elif removals or len(candidates) - len(p["left_out"]) > len(todo):
                plan = p

        def label(name, size):
            return "  %-26s %7d bytes  " % (name, size)

        if plan:
            rewrite = plan["blobs"][plan["from"]:]
            stale = stale_sectors(blobs, plan["end"], room["walk_end"], plan["index_begin"], crcs)
            print("  %d blobs (the ROMs too) are written again%s, one after the other (%d sectors)%s"
                  % (len(rewrite), " from %s on" % rewrite[0]["name"] if rewrite else "",
                     (plan["end"] - rewrite[0]["addr"]) // SECTOR if rewrite else 0,
                     ", %d sectors after them emptied" % len(stale) if stale else ""))
            for name, reason in removals.items():
                print(label(name, int(on_module[name]["len"])) + "removed (%s)" % reason)
            for name in candidates:
                if name in plan["left_out"]:
                    print(label(name, len(datas[name])) + WARNING + "NOT installed: no room left for it" + ENDC)
                    problems.append((name, "no room left for it"))
                elif name not in on_module:
                    print(label(name, len(datas[name])) + "new")
                else:
                    sectors = [step(header_len(name) + n) // SECTOR for n in (int(on_module[name]["len"]),
                                                                              len(datas[name]))]
                    print(label(name, len(datas[name])) + "changed" +
                          (", %d -> %d sectors" % tuple(sectors) if why[name] else ""))
            print("  flash for apps then: " + flash_text(plan["end"] - room["base"], plan["top"] - plan["end"], room))
            if opts.dry_run:
                return problems
            reset = True
            written = chain_write(d, plan, blobs, stale)
            print("  %s%d blobs written again%s%s" % (OK, written, ", %d removed" % len(removals) if removals else "",
                                                     ENDC))
            return problems

        for name, reason in removals.items():
            print(label(name, int(on_module[name]["len"])) + WARNING + "NOT removed (%s): %s" %
                  (reason, "--no-rewrite" if opts.no_rewrite else "the chain of apps is not written again") + ENDC)
            problems.append((name, "not removed"))
        base, index_begin = program_base(blobs), room["index_begin"] if room else None
        added = 0  # the bytes of the chain the new apps take
        for name in candidates:
            is_new = name not in on_module
            if why[name]:
                print(label(name, len(datas[name])) + WARNING + "NOT %s: %s" %
                      ("installed" if is_new else "updated", why[name]) + ENDC)
                problems.append((name, why[name]))
                continue
            if opts.dry_run:
                print(label(name, len(datas[name])) + ("would be installed" if is_new else "would be updated"))
                added += step(header_len(name) + len(datas[name])) if is_new else 0
                continue
            reset = True
            try:
                r = sc_usb.upload(d, name, datas[name], "stored!" if is_new else "updated!")
                print(label(name, len(datas[name])) + OK + r.splitlines()[-1] + ENDC)
                written += 1
                added += step(header_len(name) + len(datas[name])) if is_new else 0
                # "New BLOB <name> @ <addr> stored!": the sector after it empty, the end of the chain
                at = re.search(r"@ 0x([0-9a-fA-F]+) stored!$", r)
                nxt = int(at.group(1), 16) + step(header_len(name) + len(datas[name])) if is_new and at else None
                if nxt is not None and index_begin is not None and nxt < index_begin \
                        and not index_blobs_at(blobs, nxt, nxt + SECTOR):
                    sc_usb.upload(d, "0x%06x" % (nxt - base), BLANK, "stored!")
            except sc_usb.UploadError as e:
                print(label(name, len(datas[name])) + WARNING + "NOT %s: %s" %
                      ("installed" if is_new else "updated", e) + ENDC)
                problems.append((name, str(e)))
                d.drain()
        if room and added:
            end = room["walk_end"] + added
            print("  flash for apps then: " + flash_text(end - room["base"],
                                                         room["index_begin"] - room["kept"] * SECTOR - end, room))
    except sc_usb.UploadError as e:  # the module refused a page of the chain: the transfer is over, a reset is safe
        progress("")
        print("  %s%s%s" % (WARNING, e, ENDC))
        problems.append(("chain of apps", str(e)))
    except usb.core.USBError as e:
        # a transfer that stopped half way: the module may still wait for the rest of a blob, it would take "ui:reset"
        # for its data
        progress("")
        print("  %sUSB error: %s; reset the module (power or the button) before app/upload.py again%s" % (WARNING, e, ENDC))
        problems.append(("USB", str(e)))
        reset = False
    finally:
        if reset and not opts.no_reset:
            d.send("ui:reset")
            print("  reset (%d written)" % written)
        elif reset:
            print("  %sno reset (--no-reset): reset the module before app/upload.py again, its \"blobs\" are the old "
                  "ones until it boots%s" % (WARNING, ENDC))
        try:
            d.close()
        except usb.core.USBError:
            pass  # gone after the reset
    return problems


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-d", "--device")
    ap.add_argument("-n", "--dry-run", action="store_true")
    ap.add_argument("--no-new", action="store_true")
    ap.add_argument("--no-shrink", action="store_true")
    ap.add_argument("--no-rewrite", action="store_true")
    ap.add_argument("--no-reset", action="store_true")
    opts = ap.parse_args()

    found = modules(opts.device)
    if not found:
        sys.exit("no S&C module found" + (" matching '%s'" % opts.device if opts.device else ""))
    apps = local_apps()

    failed = []
    for mid, module, text in found:
        print("-------------------------------------------------")
        print(text)
        problems = update(mid, module, apps, opts)
        if problems is None:
            failed.append("%s: not reached" % text)
        else:
            failed += ["%s: %s" % (text.split(" (")[0], name) for name, _ in problems]
    print("-------------------------------------------------")
    if failed:
        print("%sNOT up to date:%s\n  %s" % (WARNING, ENDC, "\n  ".join(failed)))
        sys.exit(1)
    print("%d module(s) %s" % (len(found), "checked" if opts.dry_run else "up to date"))


if __name__ == "__main__":
    main()
