#!/usr/bin/env python3

import sys, zlib, hashlib

def md5sum(data):
    h  = hashlib.md5()
    h.update(data)
    return h.hexdigest()

f = open(sys.argv[1], "rb")
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
print("---------------------------------------------")
print("FILE_NAME", f.name)
print("RAM_SIZE", (l * 4) + c + d)
print("BIN_SIZE", h + b + c)
print("FILE_LEN", len(data))

crc32sum = zlib.crc32(data[0:-1])
print("CRC32 %x" % crc32sum)
print("MD5 %s" % md5sum(data))
