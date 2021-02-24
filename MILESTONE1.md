# Sistem Operasi Rpl_Nol_Tiga

## Deskripsi Singkat Cara Kerja Interrupt
Interrupt pada sistem operasi kami bekerja dengan memanggil makeInterrupt21() satu kali.
Lalu dilanjutkan dengan pemanggilan fungsi drawing() untuk menampilkan gambar saat awal booting sistem operasi.  
Fungsi drawing() dipanggil setelah memanggil interrupt untuk menjalankan graphic mode. Setelah fungsi dijalankan, pengguna harus memasukan input enter sehingga kernel pindah ke mode text. drawing() menggunakan interrupt 10h dengan argumen AX berupa ((0x0c << 8) + 0x0c). Argumen CX diisi dengan posisi x dari pixel gambar dan DX diisi dengan posisi y dari pixel gambar.  
handelInterrupt() akan memanggil printString() dengan masukkan parameter kedua handleInterrupt jika parameter pertama handleInterrupt bernilai 0x0.  
printString() menjalankan interrupt 10h untuk menampilkan tulisan ke layar.  
handleInterrupt() akan memanggil readString() dengan masukkan parameter kedua handleInterrupt jika parameter pertama handleInterrupt bernilai 0x1.  
readString() menggunakan interrupt 16h untuk mengambil input kemudian menjalankan kembali interrupt 10h untuk menampilkan input.  
clear() dipakai untuk mengubah isi seluruh buffer dengan 0x0.  
## Deskripsi Singkat kernel.asm
Pada kernel.asm, didefinisikan 4 fungsi yang nantinya bisa digunakan. putInMemory digunakan untuk meletakkan suatu char dalam segmen
memori tertentu dan akan menampilkan char tersebut ke layar saat booting OS. interrupt berfungsi untuk melakukan interrupt terhadap
OS dan akan melakukan suatu aksi berdasarkan argumen parameter pertama. 4 argumen sisanya disediakan jika memang dibutuhkan saat
pemanggilan interrupt tertentu. makeInterrupt21 berjalan jika interrupt 21 dipanggil. makeInterrupt akan memanggil fungsi keempat, yaitu
interrupt21ServiceRoutine yang akan memanggil handleInterrupt.
## Author
- 13519180 Karel Renaldi
- 13519192 Gayuh Tri Rahutami
- 13519207 Rafidika Samekto