import os

f1 = open("system.img", 'rb')
f2 = open("map.img", 'rb')
f3 = open("files.img", 'rb')
f4 = open("sectors.img", 'rb')

system = bytearray(f1.read())
map = bytearray(f2.read())
files = bytearray(f3.read())
sectors = bytearray(f4.read())

path = "coba.txt"

f5 = open(path, 'rb')

file_data = f5.read()
buffer = [[0] * 512]*16

i = 0
while(i < 16):
    buffer[i] = file_data[i*512:(i + 1)*512]
    i += 1

sectorNeeded = 0
for row in buffer:
    if(row):
        sectorNeeded += 1

sectorAvailable = 0
for row in map:
    if(row):
        sectorAvailable += 1

fileSectorAvailableIndex = None
i = 0
while(fileSectorAvailableIndex == None and i < 64):
    if(files[i*16 + 2] == ord('\0')):
        fileSectorAvailableIndex = i
    else:
        i += 1

sectorsSectorAvailableIndex = None
i = 0
while(sectorsSectorAvailableIndex == None and i < 32):
    if(sectors[i*16:(i+1)*16] == bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
        sectorsSectorAvailableIndex = i
    else:
        i += 1


if(fileSectorAvailableIndex != None):
    if(sectorAvailable >= sectorNeeded):
        print("loading to load file......")

        fileParentIndex = 0xFF
        filename = path.split('/')[-1]

        files[fileSectorAvailableIndex * 16] = fileParentIndex
        files[fileSectorAvailableIndex * 16 + 1] = sectorsSectorAvailableIndex
        i = 0
        for char in filename:
            files[fileSectorAvailableIndex * 16 + 2 + i] = ord(char)
            i += 1

        i = 0
        written_sector = 0
        tempSector = [0]*16
        while(written_sector < sectorNeeded and i < 512):
            if(map[i] != 0xFF):
                map[i] = 0xFF
                tempSector[written_sector] = i
                written_sector += 1
            i += 1

        for i in range(16):
            sectors[sectorsSectorAvailableIndex * 16 + i] = tempSector[i]

        for i in range(sectorNeeded):
            system[tempSector[i] << 9: (
                tempSector[i] + 1) << 9] = bytes(buffer[i])

        for i in range(sectorNeeded):
            print(system[tempSector[i] << 9: (tempSector[i] + 1) << 9])
            print()

        f1 = open("system.img", 'wb')
        f2 = open("map.img", 'wb')
        f3 = open("files.img", 'wb')
        f4 = open("sectors.img", 'wb')

        f1.write(bytes(system))
        f2.write(bytes(map))
        f3.write(bytes(files))
        f4.write(bytes(sectors))

        print("Load file berhasil!!!!!")

    else:
        print("Tidak cukup sektor kosong")
else:
    print("Tidak cukup entri di files")
