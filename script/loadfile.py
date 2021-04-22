# This script are meant to be run individually, separated from the Operating System

import os
import sys

def loadfile(filepath):
    with open("./output/system.img", 'rb') as f:
        system = f.read()
        system = bytearray(system)
        map = system[256*512:257*512]
        files = system[257*512:259*512]
        sector = system[259*512:260*512]

    x = filepath.split("/")
    filename = x[-1]

    with open(filepath, 'rb') as f:
        fileloaded = f.read()
        fileloaded = bytearray(fileloaded)

    sectorneeded = 0
    filechunk = [[0x0 for i in range(512)] for j in range(16)]

    for i in range(0,16):
        filechunk[i] = fileloaded[i*512:(i+1)*512]
        if(filechunk[i]):
            sectorneeded+=1

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

    if(emptysector-256>sectorneeded and filesectorfound):
        sectors = [0 for i in range(16)]
        i = 0
        counter = 0
        while(i<512 and counter<sectorneeded):
            if(map[i]==0x0):
                map[i] = 0xFF #MARK sector used in map
                sectors[counter] = i #Save list of sectors in sectors
                counter+=1
            i+=1
        system[256*512:257*512] = map
        sectorsindex=0
        for i in range(32):
            if(sector[i*16:(i+1)*16]==bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
                sectorsindex = i
                for j in range(16):
                    sector[i*16+j] = sectors[j]
                break
        
        system[259*512:260*512] = sector
        files[filesectorindex*16] = 0xFF
        files[filesectorindex*16+1] = sectorsindex
        for i in range(len(filename)):
            files[filesectorindex*16+2+i] = ord(filename[i])
        system[257*512:259*512] = files
        for i in range(sectorneeded):
            system[sectors[i]*512:(sectors[i]+1)*512] = bytes(filechunk[i]).ljust(512,b'\0')

        with open("./output/system.img", 'wb') as f:
            f.write(bytes(system))

    else:
        print("Not enough memory to load file")

for i in range(1,len(sys.argv)):
    loadfile(sys.argv[i])