#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <random>
#include <limits>

using namespace std;

// --- Analisis Struktur Data ---
// 1. unordered_map<string, Train> (Hash Table):
//    - Digunakan untuk menyimpan data kereta dengan ID Kereta sebagai key.
//    - Kompleksitas Waktu: O(1) rata-rata untuk operasi pencarian (Read by ID), penyisipan (Create),
//      pembaruan (Update), dan penghapusan (Delete). Kasus terburuk O(n) jika banyak collision.
//    - Keuntungan dibanding Linear Search (vector/list): Pencarian sangat cepat karena menggunakan
//      hash function untuk langsung menuju ke lokasi memori data, sedangkan linear search harus
//      memeriksa satu per satu elemen dari awal hingga akhir (O(n)).
//
// 2. unordered_map<string, Reservation> (Hash Table):
//    - Digunakan untuk menyimpan data reservasi dengan Kode Booking sebagai key.
//    - Kompleksitas Waktu: O(1) rata-rata untuk CRUD berdasarkan kode booking.
//
// 3. vector<Passenger> (Dynamic Array):
//    - Digunakan di dalam struct Reservation untuk menyimpan daftar penumpang.
//    - Alasan: Alokasi memori dinamis (bisa bertambah sesuai jumlah tiket) dan akses elemen cepat O(1).
//      Sangat cocok karena jumlah penumpang biasanya diketahui saat reservasi dibuat dan jarang di-insert/delete di tengah.
//
// 4. Linear Search:
//    - Digunakan saat mencari data yang BUKAN merupakan key dari hash table (misal: mencari kereta berdasarkan tujuan).
//    - Kompleksitas: O(n).
//
// 5. Sorting:
//    - Menggunakan std::sort (biasanya implementasi IntroSort: gabungan Quicksort, Heapsort, Insertion Sort).
//    - Kompleksitas Waktu: O(n log n).
//    - Untuk melakukan sorting dari unordered_map, kita perlu menyalin value-nya ke dalam vector terlebih dahulu.

struct Passenger {
    string nik;
    string nama;
    int umur;
};

struct Train {
    string idKereta;
    string namaKereta;
    string asal;
    string tujuan;
    int kapasitas;
    int kursiTersedia;
    double hargaTiket;
    int totalPemesanan = 0; // Untuk statistik kereta paling sering dipesan
};

struct Reservation {
    string kodeBooking;
    vector<Passenger> penumpang;
    string idKereta;
    int jumlahTiket;
    double totalHarga;
    string tanggalReservasi;
};

class TrainReservationSystem {
private:
    unordered_map<string, Train> trains;
    unordered_map<string, Reservation> reservations;

    // Helper: Membersihkan buffer input
    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Helper: Generate kode booking unik
    string generateBookingCode() {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> distribution(0, chars.size() - 1);
        string code = "BK";
        for (int i = 0; i < 6; ++i) {
            code += chars[distribution(generator)];
        }
        return code;
    }

    // Helper: Mendapatkan tanggal saat ini sebagai string
    string getCurrentDate() {
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        string date = ctime(&time);
        date.pop_back(); // Hapus newline di akhir
        return date;
    }

public:
    // ==========================================
    //            FITUR CRUD KERETA
    // ==========================================

    void createTrain() {
        Train t;
        cout << "\n=== Tambah Data Kereta ===\n";
        cout << "ID Kereta    : "; cin >> t.idKereta;
        
        if (trains.find(t.idKereta) != trains.end()) {
            cout << "Error: Kereta dengan ID " << t.idKereta << " sudah ada!\n";
            return;
        }

        clearInputBuffer();
        cout << "Nama Kereta  : "; getline(cin, t.namaKereta);
        cout << "Stasiun Asal : "; getline(cin, t.asal);
        cout << "Tujuan       : "; getline(cin, t.tujuan);
        cout << "Kapasitas    : "; cin >> t.kapasitas;
        t.kursiTersedia = t.kapasitas;
        cout << "Harga Tiket  : "; cin >> t.hargaTiket;

        trains[t.idKereta] = t;
        cout << "Data kereta berhasil ditambahkan!\n";
    }

    void readAllTrains() {
        cout << "\n=== Daftar Seluruh Kereta ===\n";
        if (trains.empty()) {
            cout << "Belum ada data kereta.\n";
            return;
        }

        cout << left << setw(10) << "ID" << setw(20) << "Nama" << setw(15) << "Asal" 
             << setw(15) << "Tujuan" << setw(10) << "Kapasitas" << setw(10) << "Sisa" 
             << "Harga\n";
        cout << string(90, '-') << "\n";
        for (const auto& pair : trains) {
            const Train& t = pair.second;
            cout << left << setw(10) << t.idKereta << setw(20) << t.namaKereta << setw(15) << t.asal 
                 << setw(15) << t.tujuan << setw(10) << t.kapasitas << setw(10) << t.kursiTersedia 
                 << "Rp" << fixed << setprecision(2) << t.hargaTiket << "\n";
        }
    }

    void searchTrainById() {
        string id;
        cout << "Masukkan ID Kereta: "; cin >> id;

        // Hash Lookup: O(1)
        auto it = trains.find(id);
        if (it != trains.end()) {
            const Train& t = it->second;
            cout << "\nKereta Ditemukan:\n";
            cout << "Nama   : " << t.namaKereta << "\nRute   : " << t.asal << " - " << t.tujuan 
                 << "\nHarga  : Rp" << fixed << setprecision(2) << t.hargaTiket 
                 << "\nSisa Kursi: " << t.kursiTersedia << "/" << t.kapasitas << "\n";
        } else {
            cout << "Kereta dengan ID " << id << " tidak ditemukan.\n";
        }
    }

    void searchTrainByDestination() {
        string dest;
        cout << "Masukkan Tujuan: "; 
        clearInputBuffer();
        getline(cin, dest);

        // Linear Search: O(n)
        bool found = false;
        cout << "\nHasil Pencarian Tujuan '" << dest << "':\n";
        for (const auto& pair : trains) {
            if (pair.second.tujuan == dest) {
                const Train& t = pair.second;
                cout << "- [" << t.idKereta << "] " << t.namaKereta << " (" << t.asal << " - " << t.tujuan << "), Rp" << t.hargaTiket << "\n";
                found = true;
            }
        }
        if (!found) cout << "Tidak ada kereta dengan tujuan tersebut.\n";
    }

    void updateTrain() {
        string id;
        cout << "Masukkan ID Kereta yang akan diedit: "; cin >> id;

        auto it = trains.find(id);
        if (it != trains.end()) {
            Train& t = it->second;
            cout << "Mengedit Kereta: " << t.namaKereta << "\n";
            clearInputBuffer();
            cout << "Nama Kereta Baru (kosongkan jika tidak diubah): "; 
            string temp; getline(cin, temp);
            if (!temp.empty()) t.namaKereta = temp;
            
            cout << "Asal Baru (kosongkan jika tidak diubah): "; getline(cin, temp);
            if (!temp.empty()) t.asal = temp;

            cout << "Tujuan Baru (kosongkan jika tidak diubah): "; getline(cin, temp);
            if (!temp.empty()) t.tujuan = temp;

            cout << "Harga Baru (ketik -1 jika tidak diubah): ";
            double hrg; cin >> hrg;
            if (hrg != -1) t.hargaTiket = hrg;

            cout << "Data kereta berhasil diupdate.\n";
        } else {
            cout << "Kereta tidak ditemukan.\n";
        }
    }

    void deleteTrain() {
        string id;
        cout << "Masukkan ID Kereta yang akan dihapus: "; cin >> id;

        if (trains.erase(id)) { // Hash table erase: O(1)
            cout << "Kereta berhasil dihapus.\n";
        } else {
            cout << "Kereta tidak ditemukan.\n";
        }
    }

    // ==========================================
    //           FITUR CRUD RESERVASI
    // ==========================================

    void createReservation() {
        if (trains.empty()) {
            cout << "Belum ada kereta yang tersedia.\n";
            return;
        }

        string idKereta;
        cout << "\n=== Buat Reservasi ===\n";
        cout << "Masukkan ID Kereta: "; cin >> idKereta;

        auto it = trains.find(idKereta);
        if (it == trains.end()) {
            cout << "Error: Kereta tidak ditemukan.\n";
            return;
        }

        Train& t = it->second;
        int jumlah;
        cout << "Jumlah tiket yang ingin dipesan: "; cin >> jumlah;

        if (jumlah <= 0) {
            cout << "Error: Jumlah tiket tidak valid.\n";
            return;
        }

        if (t.kursiTersedia < jumlah) {
            cout << "Error: Kursi tidak mencukupi. Sisa kursi: " << t.kursiTersedia << "\n";
            return;
        }

        Reservation res;
        res.kodeBooking = generateBookingCode();
        res.idKereta = idKereta;
        res.jumlahTiket = jumlah;
        res.totalHarga = jumlah * t.hargaTiket;
        res.tanggalReservasi = getCurrentDate();

        cout << "\nMasukkan Data Penumpang:\n";
        for (int i = 0; i < jumlah; ++i) {
            Passenger p;
            cout << "Penumpang " << i + 1 << "\n";
            cout << "NIK  : "; cin >> p.nik;
            clearInputBuffer();
            cout << "Nama : "; getline(cin, p.nama);
            cout << "Umur : "; cin >> p.umur;
            res.penumpang.push_back(p);
        }

        // Kurangi jumlah kursi dan tambah statistik
        t.kursiTersedia -= jumlah;
        t.totalPemesanan += jumlah;

        reservations[res.kodeBooking] = res;

        cout << "\nReservasi Berhasil!\n";
        cout << "Kode Booking Anda: " << res.kodeBooking << "\n";
        cout << "Total Harga: Rp" << fixed << setprecision(2) << res.totalHarga << "\n";
    }

    void readAllReservations() {
        cout << "\n=== Seluruh Reservasi ===\n";
        if (reservations.empty()) {
            cout << "Belum ada data reservasi.\n";
            return;
        }

        cout << left << setw(15) << "Kode Booking" << setw(15) << "ID Kereta" 
             << setw(10) << "Tiket" << "Total Harga\n";
        cout << string(60, '-') << "\n";
        for (const auto& pair : reservations) {
            const Reservation& r = pair.second;
            cout << left << setw(15) << r.kodeBooking << setw(15) << r.idKereta 
                 << setw(10) << r.jumlahTiket << "Rp" << fixed << setprecision(2) << r.totalHarga << "\n";
        }
    }

    void searchReservation() {
        string kode;
        cout << "Masukkan Kode Booking: "; cin >> kode;

        auto it = reservations.find(kode);
        if (it != reservations.end()) {
            const Reservation& r = it->second;
            cout << "\nDetail Reservasi:\n";
            cout << "Kode Booking : " << r.kodeBooking << "\n";
            cout << "ID Kereta    : " << r.idKereta << " (" << trains[r.idKereta].namaKereta << ")\n";
            cout << "Tanggal      : " << r.tanggalReservasi << "\n";
            cout << "Jumlah Tiket : " << r.jumlahTiket << "\n";
            cout << "Total Harga  : Rp" << fixed << setprecision(2) << r.totalHarga << "\n";
            cout << "Daftar Penumpang:\n";
            for (size_t i = 0; i < r.penumpang.size(); ++i) {
                cout << i + 1 << ". " << r.penumpang[i].nama << " (NIK: " << r.penumpang[i].nik << ", Umur: " << r.penumpang[i].umur << ")\n";
            }
        } else {
            cout << "Reservasi dengan kode " << kode << " tidak ditemukan.\n";
        }
    }

    void updateReservation() {
        string kode;
        cout << "Masukkan Kode Booking yang akan diupdate: "; cin >> kode;

        auto it = reservations.find(kode);
        if (it == reservations.end()) {
            cout << "Kode booking tidak ditemukan.\n";
            return;
        }

        Reservation& r = it->second;
        cout << "1. Ubah jumlah tiket\n";
        cout << "2. Ubah data penumpang\n";
        cout << "Pilihan: ";
        int pil; cin >> pil;

        if (pil == 1) {
            int tiketBaru;
            cout << "Jumlah tiket baru: "; cin >> tiketBaru;

            if (tiketBaru <= 0) {
                cout << "Jumlah tiket tidak valid.\n";
                return;
            }

            int selisih = tiketBaru - r.jumlahTiket;
            Train& t = trains[r.idKereta];

            if (t.kursiTersedia < selisih) {
                cout << "Kursi tidak mencukupi untuk tambahan tiket ini.\n";
                return;
            }

            // Sesuaikan kursi dan statistik
            t.kursiTersedia -= selisih;
            t.totalPemesanan += selisih;
            r.jumlahTiket = tiketBaru;
            r.totalHarga = tiketBaru * t.hargaTiket;

            // Sesuaikan vector penumpang
            if (selisih > 0) {
                cout << "Masukkan data " << selisih << " penumpang tambahan:\n";
                for (int i = 0; i < selisih; ++i) {
                    Passenger p;
                    cout << "NIK  : "; cin >> p.nik;
                    clearInputBuffer();
                    cout << "Nama : "; getline(cin, p.nama);
                    cout << "Umur : "; cin >> p.umur;
                    r.penumpang.push_back(p);
                }
            } else if (selisih < 0) {
                cout << "Menghapus " << -selisih << " data penumpang terakhir...\n";
                for (int i = 0; i < -selisih; ++i) {
                    r.penumpang.pop_back();
                }
            }
            cout << "Reservasi berhasil diupdate.\n";

        } else if (pil == 2) {
            int no;
            cout << "Masukkan nomor urut penumpang yang akan diedit (1 - " << r.penumpang.size() << "): ";
            cin >> no;
            if (no >= 1 && no <= (int)r.penumpang.size()) {
                Passenger& p = r.penumpang[no - 1];
                cout << "NIK Baru: "; cin >> p.nik;
                clearInputBuffer();
                cout << "Nama Baru: "; getline(cin, p.nama);
                cout << "Umur Baru: "; cin >> p.umur;
                cout << "Data penumpang diupdate.\n";
            } else {
                cout << "Nomor tidak valid.\n";
            }
        }
    }

    void deleteReservation() {
        string kode;
        cout << "Masukkan Kode Booking yang akan dibatalkan: "; cin >> kode;

        auto it = reservations.find(kode);
        if (it != reservations.end()) {
            Reservation r = it->second;
            // Kembalikan kursi
            if (trains.find(r.idKereta) != trains.end()) {
                trains[r.idKereta].kursiTersedia += r.jumlahTiket;
                trains[r.idKereta].totalPemesanan -= r.jumlahTiket;
            }
            reservations.erase(it);
            cout << "Reservasi berhasil dibatalkan. Kursi telah dikembalikan.\n";
        } else {
            cout << "Kode booking tidak ditemukan.\n";
        }
    }

    // ==========================================
    //            FITUR TAMBAHAN
    // ==========================================

    void viewStatistics() {
        cout << "\n=== Statistik Sistem ===\n";
        cout << "Total Reservasi Aktif : " << reservations.size() << "\n";
        
        double totalPendapatan = 0;
        for (const auto& r : reservations) {
            totalPendapatan += r.second.totalHarga;
        }
        cout << "Total Pendapatan      : Rp" << fixed << setprecision(2) << totalPendapatan << "\n";

        if (!trains.empty()) {
            string topTrain = "";
            int maxPesan = -1;
            for (const auto& t : trains) {
                if (t.second.totalPemesanan > maxPesan) {
                    maxPesan = t.second.totalPemesanan;
                    topTrain = t.second.namaKereta;
                }
            }
            if (maxPesan > 0) {
                cout << "Kereta Paling Populer : " << topTrain << " (" << maxPesan << " tiket terjual)\n";
            } else {
                cout << "Belum ada tiket kereta yang terjual.\n";
            }
        }
    }

    void sortTrains(int criteria) {
        if (trains.empty()) {
            cout << "Belum ada data kereta.\n";
            return;
        }

        // Salin ke vector untuk sorting O(n)
        vector<Train> trainList;
        for (const auto& pair : trains) {
            trainList.push_back(pair.second);
        }

        // Sorting O(n log n)
        if (criteria == 1) { // Harga termurah
            sort(trainList.begin(), trainList.end(), [](const Train& a, const Train& b) {
                return a.hargaTiket < b.hargaTiket;
            });
            cout << "\n--- Kereta Diurutkan Berdasarkan Harga Termurah ---\n";
        } else { // Kursi terbanyak
            sort(trainList.begin(), trainList.end(), [](const Train& a, const Train& b) {
                return a.kursiTersedia > b.kursiTersedia;
            });
            cout << "\n--- Kereta Diurutkan Berdasarkan Kursi Terbanyak ---\n";
        }

        cout << left << setw(10) << "ID" << setw(20) << "Nama" << setw(10) << "Sisa Kursi" << "Harga\n";
        for (const auto& t : trainList) {
            cout << left << setw(10) << t.idKereta << setw(20) << t.namaKereta 
                 << setw(10) << t.kursiTersedia << "Rp" << fixed << setprecision(2) << t.hargaTiket << "\n";
        }
    }
};

void showMenu() {
    cout << "\n============================================\n";
    cout << "        SISTEM RESERVASI KERETA API         \n";
    cout << "============================================\n";
    cout << "--- Menu Kereta ---\n";
    cout << " 1. Tambah Data Kereta\n";
    cout << " 2. Lihat Seluruh Kereta\n";
    cout << " 3. Cari Kereta (berdasarkan ID)\n";
    cout << " 4. Cari Kereta (berdasarkan Tujuan)\n";
    cout << " 5. Edit Data Kereta\n";
    cout << " 6. Hapus Data Kereta\n";
    cout << "--- Menu Reservasi ---\n";
    cout << " 7. Buat Reservasi\n";
    cout << " 8. Lihat Seluruh Reservasi\n";
    cout << " 9. Cari Detail Reservasi\n";
    cout << "10. Update Reservasi\n";
    cout << "11. Batalkan Reservasi\n";
    cout << "--- Fitur Tambahan ---\n";
    cout << "12. Tampilkan Statistik\n";
    cout << "13. Urutkan Kereta (Harga Termurah)\n";
    cout << "14. Urutkan Kereta (Sisa Kursi Terbanyak)\n";
    cout << " 0. Keluar\n";
    cout << "============================================\n";
    cout << "Pilih menu: ";
}

int main() {
    TrainReservationSystem system;
    int choice;

    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cout << "Input tidak valid. Harap masukkan angka.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.createTrain(); break;
            case 2: system.readAllTrains(); break;
            case 3: system.searchTrainById(); break;
            case 4: system.searchTrainByDestination(); break;
            case 5: system.updateTrain(); break;
            case 6: system.deleteTrain(); break;
            case 7: system.createReservation(); break;
            case 8: system.readAllReservations(); break;
            case 9: system.searchReservation(); break;
            case 10: system.updateReservation(); break;
            case 11: system.deleteReservation(); break;
            case 12: system.viewStatistics(); break;
            case 13: system.sortTrains(1); break;
            case 14: system.sortTrains(2); break;
            case 0: cout << "Terima kasih telah menggunakan layanan kami!\n"; return 0;
            default: cout << "Pilihan tidak tersedia.\n";
        }
    }

    return 0;
}
