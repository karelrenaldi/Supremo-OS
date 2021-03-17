# Sistem Operasi Rpl_Nol_Tiga
## Filesystem
Filesystem terdiri dari 3 sektor khusus yaitu sektor map pada sektor 0x100, sektor files pada sektor 0x101-0x102, dan sektor sectors pada sektor 0x103.

### Sektor map
Terdiri atas 512 byte dimana masing-masing byte menandakan apakah sektor terisi atau belum

### Sektor files
Terdiri atas 2 sektor berukuran 512 bytes yang menampung informasi dari file dan direktori yang terdapat pada filesystem.
Tiap entry berukuran 16 bytes dengan 1 byte penanda parent directory, 1 byte penanda index sectors, dan 14 bytes untuk nama file
## ReadSector
ReadSector adalah prosedur yang digunakan untuk membaca suatu sector

Cara kerja:

Memanggil interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
## WriteSector
WriteSector adalah prosedur yang digunakan untuk menulis ke suatu sector

Cara kerja:

Memanggil interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
## ReadFile
ReadFile adalah prosedur yang digunakan untuk membaca isi dari suatu file

Cara kerja:
1. Membaca sektor Map dan Dir
2. Cek apakah ada Dir yang kosong. Jika tidak, hentikan penulisan file
3. Jika ya, cek apakah jumlah sektor di map cukup untuk buffer file. Jika tidak, hentikan penulisan file
4. Jika ya, Bersihkan sektor yang akan digunakan untuk menyimpan nama
5. Isi sektor pada dir dengan nama file
6. Cari sektor di map yang kosong
7. Isi sektor tersebut dengan Byte di buffer file dan tandai di map
8. Ulangi 6 dan 7 hingga semua buffer file ditulis
## WriteFile
WriteFile adalah prosedur yang digunakan untuk menuliskan sesuatu ke suatu suatu file

Cara kerja:
1. Baca sektor dir
2. Cari apakah nama file terdapat di dir. Jika tidak berikan pesan kegagalan.
3. Jika ada baca seluruh sektor ke buffer 

## LoadFile
LoadFile adalah prosedur yang digunakan untuk membuka suatu file
## Shell
### ls
ls merupakan command pada shell untuk mencetak daftar folder dan file yang ada pada current directory

Cara kerja:
1. Iterasi semua file dan folder yang ada di sistem operasi
2. Jika parent index dari suatu file/folder sama dengan index current directory, maka cetak nama file/folder tersebut
### cd
cd merupakan command pada shell yang digunakan untuk mengubah directory.

Cara kerja:
1. Mengecek apakah input berawalan dengan "cd "
2. Jika ya, simpan path, yaitu string yang berada setelah kata "cd"
3. Panggil prosedur cd dengan argumen path yang ingin dituju dan index dari directory sekarang
4. Untuk tiap nama folder yang dipisahkan oleh karakter '/', cari index dari folder tersebut dan ubah current index
5. Jika index tidak ditemukan, berikan pesan bahwa path tidak ada
### cat
cat merupakan command pada shell yang digunakan untuk melakukan print dari isi file ke layar

Cara kerja:
1. Baca isi file dengan menggunakan readFile
2. Print seluruh isi file dengan menggunakan printString
### ln
ln merupakan command pada shell untuk melakukan symbolic link dalam mode hard link

Cara kerja:
1. Cek apakah file sudah ada / file tidak ditemukan / entry file penuh
2. Jika tidak, lakukan linking
