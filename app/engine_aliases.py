#!/usr/bin/env python3
"""The ENGINE_ALIAS and ENGINE_INSERT_FX lines of an app (squares-and-circles-api.h) as the constants of its binary,
for build.sh:

  arm-none-eabi-g++ -E -P ... App.cpp | python engine_aliases.py OUT.c

From the preprocessor output, in its order (a line that an #if leaves out is none): ENGINE_NAME("A;B") adds its
engines, ENGINE_ALIAS("OLD"[, "LAYOUT"]) and ENGINE_INSERT_FX() belong to the engine before it (to the first one
without one before it). OUT.c gets the constants for .text_nogc, aligned and padded to 4 as the prologues of the
public functions after them need (mkmodule's asm_template.tmpl):
- __engine_alias: the first alias of each engine, ';' between the engines ("" for one without), the format the
  firmwares before read;
- __engine_aliases: all of them, ';' between the engines, '|' between the aliases of one, OLD=LAYOUT for one with
  the old parameters' layout (machine/src/udynlink_engine.cpp add_aliases());
- __insert_fx: a letter per engine, '1' marked ENGINE_INSERT_FX (offered as INSERT-FX on the I/O page), '0' not
  (machine/src/udynlink_engine.cpp add()); only with a mark.
Without an alias and a mark OUT.c is removed: the binary has none.
"""
import os
import re
import sys

TOKEN = re.compile(r'__engine_names_fit\("([^"]*)"\)|__engine_alias_fits\("([^"]*)"(?:\s*,\s*"([^"]*)")?\)'
                   r'|(__engine_insert_fx\("INSERT-FX"\))')


def engines_of(text):
    """[[alias, ...] per engine], an alias "OLD" or "OLD=LAYOUT", and [insert per engine] (True: ENGINE_INSERT_FX)."""
    engines, inserts = [], []
    pending, pending_insert = [], False  # before the first ENGINE_NAME: the first engine's
    for m in TOKEN.finditer(text):
        names, old, layout, insert = m.groups()
        if names is not None:
            for _ in names.split(";"):
                engines.append([])
                inserts.append(False)
            if pending or pending_insert:
                engines[0][:0] = pending
                inserts[0] |= pending_insert
                pending, pending_insert = [], False
        elif insert is not None:
            if inserts:
                inserts[-1] = True
            else:
                pending_insert = True
        else:
            alias = old + ("=" + layout if layout else "")
            (engines[-1] if engines else pending).append(alias)
    if (pending or pending_insert) and not engines:  # no ENGINE_NAME: one engine, CATEGORY/FileName
        engines.append(pending)
        inserts.append(pending_insert)
    return engines, inserts


def constant(name, value):
    size = (len(value) + 1 + 3) & ~3
    return '__attribute__((used, section(".text_nogc"), aligned(4))) const char %s[%d] = "%s";\n' % (name, size, value)


def main():
    out = sys.argv[1]
    engines, inserts = engines_of(sys.stdin.read())
    if not any(engines) and not any(inserts):
        if os.path.exists(out):
            os.remove(out)
        return
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with open(out, "w") as f:
        f.write("// app/engine_aliases.py from the ENGINE_ALIAS and ENGINE_INSERT_FX lines of the app\n")
        if any(engines):
            first = ";".join(a[0].split("=")[0] if a else "" for a in engines)
            full = ";".join("|".join(a) for a in engines)
            f.write(constant("__engine_alias", first))
            f.write(constant("__engine_aliases", full))
        if any(inserts):
            f.write(constant("__insert_fx", "".join("1" if i else "0" for i in inserts)))


if __name__ == "__main__":
    main()
