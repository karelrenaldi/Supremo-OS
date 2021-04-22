map_file = open("./output/map.img", 'rb')
map_data = map_file.read()

print(len(map_data))
map_data_arr = [0 for i in range(512)]
for i in range(17):
    map_data_arr[i] = 0xFF

map_file = open("./output/map.img", 'wb')
map_file.write(bytes(bytearray(map_data_arr)))