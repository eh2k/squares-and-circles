#!/usr/bin/env python3
"""The ENGINE_ALIAS lines of an app (squares-and-circles-api.h) as the constants of its binary, for build.sh:

  arm-none-eabi-g++ -E -P ... App.cpp | python engine_aliases.py OUT.c

From the preprocessor output, in its order (an alias that an #if leaves out is none): ENGINE_NAME("A;B") adds its
engines, ENGINE_ALIAS("OLD"[, "LAYOUT"]) belongs to the engine before it (to the first one without one before it).
OUT.c gets two constants for .text_nogc, aligned and padded to 4 as the prologues of the public functions after them
need (mkmodule's asm_template.tmpl):
- __engine_alias: the first alias of each engine, ';' between the engines ("" for one without), the format the
  firmwares before read;
- __engine_aliases: all of them, ';' between the engines, '|' between the aliases of one, OLD=LAYOUT for one with
  the old parameters' layout (machine/src/udynlink_engine.cpp add_aliases()).
Without an alias OUT.c is removed: the binary has none.
"""
import os
import re
import sys

TOKEN = re.compile(r'__engine_names_fit\("([^"]*)"\)|__engine_alias_fits\("([^"]*)"(?:\s*,\s*"([^"]*)")?\)')


def aliases(text):
    """[[alias, ...] per engine], an alias "OLD" or "OLD=LAYOUT"."""
    engines = []
    pending = []  # aliases before the first ENGINE_NAME: the first engine's
    for m in TOKEN.finditer(text):
        names, old, layout = m.groups()
        if names is not None:
            for _ in names.split(";"):
                engines.append([])
            if pending:
                engines[0][:0] = pending
                pending = []
        else:
            alias = old + ("=" + layout if layout else "")
            (engines[-1] if engines else pending).append(alias)
    if pending:  # no ENGINE_NAME: one engine, CATEGORY/FileName
        engines.append(pending)
    return engines


def constant(name, value):
    size = (len(value) + 1 + 3) & ~3
    return '__attribute__((used, section(".text_nogc"), aligned(4))) const char %s[%d] = "%s";\n' % (name, size, value)


def main():
    out = sys.argv[1]
    engines = aliases(sys.stdin.read())
    if not any(engines):
        if os.path.exists(out):
            os.remove(out)
        return
    first = ";".join(a[0].split("=")[0] if a else "" for a in engines)
    full = ";".join("|".join(a) for a in engines)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with open(out, "w") as f:
        f.write("// app/engine_aliases.py from the ENGINE_ALIAS lines of the app\n")
        f.write(constant("__engine_alias", first))
        f.write(constant("__engine_aliases", full))


if __name__ == "__main__":
    main()
