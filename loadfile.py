# This script are meant to be run individually, separated from the Operating System

import os

with open("system.img", 'rb') as f:
    system = f.read()
    system = bytearray(system)

with open("map.img", 'rb') as f:
    map = f.read()
    map = bytearray(map)

with open("files.img", 'rb') as f:
    files = f.read()
    files = bytearray(files)

with open("sectors.img", 'rb') as f:
    sector = f.read()
    sector = bytearray(sector)

# filepath = input("Input filepath: ")
filepath = "coba.txt"
x = filepath.split("/")
filename = x[-1]
print(filename)

with open(filepath, 'rb') as f:
    fileloaded = f.read()

sectorneeded = 0
filechunk = [[0x0 for i in range(512)] for j in range(16)]

for i in range(0,16):
    filechunk[i] = fileloaded[i*512:(i+1)*512]
    print(filechunk[i])
    if(filechunk[i]):
        sectorneeded+=1

print("Sector needed :" ,sectorneeded)
emptysector = 0
filesectorfound = False
filesectorindex = 0

for i in range(len(map)):
    if(map[i]==0x0):
        emptysector+=1

for i in range(64):
    if(files[i*16:(i+1)*16]==bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
        filesectorfound = True
        filesectorindex = i
        break

print("Empty sectors available:",emptysector)
if(emptysector-256>sectorneeded and filesectorfound):
    print("Executing load file...")
    sectors = [0 for i in range(16)]
    i = 0
    counter = 0
    while(i<512 and counter<sectorneeded):
        if(map[i]==0x0):
            map[i] = 0xFF #MARK sector used in map
            sectors[counter] = i #Save list of sectors in sectors
            counter+=1
        i+=1
    with open("map.img", 'wb') as f:
        f.write(bytes(map))
    sectorsindex=0
    for i in range(32):
        if(sector[i*16:(i+1)*16]==bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
            sectorsindex = i
            for j in range(16):
                sector[i*16+j] = sectors[j]
            break
    
    with open("sectors.img", 'wb') as f:
        f.write(bytes(sector))

    files[filesectorindex*16] = 0xFF
    files[filesectorindex*16+1] = sectorsindex
    for i in range(len(filename)):
        files[filesectorindex*16+2+i] = ord(filename[i])
    with open("files.img", 'wb') as f:
        f.write(bytes(files))

else:
    print("Not enough memory to load file")
