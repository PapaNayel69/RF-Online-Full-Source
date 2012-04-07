Panduan Build & Jalan RF Online Source
======================================

Gambaran Singkat
----------------
- Kode berasal dari era VC++ 6.0 (format `.dsw/.dsp`), target Win32 32-bit.
- Komponen utama: `R3Engine` (lib grafis DirectX 8), klien `RF_Online`, server `AccountServer`, `LoginServer`, `ControlServer`.
- Ketergantungan: DirectX 8.1 SDK, ADO/MDAC, SQL Server (skema DB tidak disertakan), Miles Sound System runtime (`mss32.dll`).

Prasyarat Lingkungan
--------------------
- Windows XP/2000 (atau VM) 32-bit sangat disarankan.
- Visual C++ 6.0 + SP6 (atau VS 2003 dengan konversi manual).
- DirectX 8.1 SDK: set Tools → Options → Directories untuk include dan lib DX8.
- SQL Server 2000/2005 32-bit + MDAC/ADO (`msado15.dll` sudah di-import).
- Pastikan `mss32.dll` tersedia saat menjalankan klien.

Urutan Build (VC6)
------------------
1) Engine  
   - Buka `R3Engine/R3Engine/R3Engine.dsw`.  
   - Build konfigurasi yang dibutuhkan (contoh: `R3Engine - Win32 Client Debug/Release`).  
   - Hasil: `R3Engine*.lib` di folder proyek.

2) Utilitas engine (opsional)  
   - Workspace di `R3Engine/R3Engine/Util/*.dsw` bila perlu tool ekspor.  
   - Output ke `R3Engine/R3Bin`.

3) Klien  
   - Buka `RF Client Side/RF_Client/RF_Online.dsw`.  
   - Tambah include path ke `R3Engine/R3Engine` dan lib path ke hasil `R3Engine*.lib` + `RF Client Side/RF_Client/lib/CharacterMR.lib`.  
   - Link dengan lib DX8 (`d3d8.lib`, `d3dx8.lib`, dll.).  
   - Build `RF_Online - Win32 Release` (atau Debug/Developer). Output default: `RF Client Side/Bin/RF_Online.bin` (buat folder jika belum ada).

4) Server  
   - Buka masing-masing workspace:  
     - `RF Server Side/RF_Server/AccountServer/AccountServer.dsw`  
     - `RF Server Side/RF_Server/LoginServer/LoginServer.dsw`  
     - `RF Server Side/RF_Server/ControlServer/ControlServer.dsw`  
   - Pastikan include path mencakup `../common`, `../network`, `../protocol`, `../database`.  
   - Link lib standar Win32 + `Ws2_32.lib`, `winmm.lib`.  
   - Build konfigurasi `Win32 Release`. Output diarahkan ke `RF Server Side/RF_Server/RF_Build/release/<Server>` (buat folder bila perlu).

Menjalankan
-----------
- Siapkan database SQL Server dengan skema RF (skrip tidak ada di repo). Kode membentuk connection string dari User/Pass/DB/Host di `SetConnectionStr` (`_rfdatabase.cpp`); isi nilai sesuai server Anda (bisa hardcode sementara).
- Urutan start umum: ControlServer → AccountServer → LoginServer. Pastikan IP/port di `mainthread.cpp` tiap server sesuai jaringan Anda.
- Klien memerlukan aset/data game penuh (tidak disertakan). Letakkan di jalur yang diharapkan sebelum menjalankan `RF_Online.bin`.

Catatan
-------
- Proyek tua; jauh lebih mudah dibangun di lingkungan XP + VC6 + DX8 daripada toolchain modern.  
- Jika memakai VS baru, Anda harus upgrade proyek, mengganti import ADO, dan memastikan D3D8 32-bit tersedia.  
- Tidak ada skrip otomatis; semua build via IDE VC6 atau dengan `nmake` setelah ekspor makefile dari VC6.
