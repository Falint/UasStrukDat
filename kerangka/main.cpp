#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <limits>

using namespace std;

struct Ticket {
    string namaKereta;
    string jamBerangkat;
    string asal;
    string tujuan;
};

// Node untuk menyimpan Key (Nama) dan Value (Daftar Tiket)
struct HashNode {
    string nama;
    vector<Ticket> tickets;
};

// =========================================================================
// CUSTOM HASH TABLE DENGAN SEPARATE CHAINING (LINKED LIST)
// =========================================================================
class CustomHashTable {
private:
    static const int TABLE_SIZE = 10; // Sengaja dibuat kecil (10) agar mudah terjadi kolisi saat testing
    
    // Array of Linked List (Separate Chaining)
    // Setiap indeks menyimpan sebuah list (linked list dari C++ STL) untuk menampung data yang mengalami kolisi
    list<HashNode> table[TABLE_SIZE];

    // Fungsi Hash Manual menggunakan rumus Modulus dan Polinomial
    int hashFunction(const string& key) {
        long long hashVal = 0;
        for (char c : key) {
            // Rumus: H(x) = (H(x) * 31 + ASCII Karakter) % TABLE_SIZE
            // Angka 31 sering digunakan sebagai pengali bilangan prima ganjil pada fungsi hash string
            hashVal = (hashVal * 31 + c) % TABLE_SIZE;
        }
        return hashVal;
    }

public:
    // Mencari pointer ke vector tiket berdasarkan nama. Jika tidak ada, return nullptr.
    vector<Ticket>* cari(const string& nama) {
        int index = hashFunction(nama);
        
        // Looping pada Linked List di dalam indeks tersebut (Separate Chaining)
        // Jika tidak terjadi kolisi, ini hanya looping 1 kali (sangat instan O(1))
        for (auto& node : table[index]) {
            if (node.nama == nama) {
                return &node.tickets; // Ketemu
            }
        }
        return nullptr; // Tidak ketemu
    }

    // Menambah data baru, atau mengembalikan pointer ke vector tiket yang sudah ada
    vector<Ticket>* tambah(const string& nama) {
        int index = hashFunction(nama);

        // 1. Cari apakah nama sudah ada di dalam list (untuk menghindari duplikasi)
        for (auto& node : table[index]) {
            if (node.nama == nama) {
                return &node.tickets;
            }
        }

        // 2. Jika belum ada, buat node baru dan masukkan ke dalam Linked List (Separate Chaining)
        HashNode newNode;
        newNode.nama = nama;
        table[index].push_back(newNode);
        
        // Kembalikan reference dari elemen terakhir yang baru saja ditambahkan
        return &table[index].back().tickets;
    }

    // Menghapus node secara utuh dari hash table
    bool hapus(const string& nama) {
        int index = hashFunction(nama);

        // Cari di dalam linked list
        auto it = table[index].begin();
        while (it != table[index].end()) {
            if (it->nama == nama) {
                table[index].erase(it); // Hapus elemen dari linked list
                return true;
            }
            it++;
        }
        return false;
    }

    // Hanya untuk keperluan menampilkan seluruh isi tabel dan struktur bucket-nya
    void cetakSemua() {
        bool kosong = true;
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (!table[i].empty()) {
                kosong = false;
                cout << "[Bucket Index " << i << "]:\n";
                for (const auto& node : table[i]) {
                    cout << "  -> Nama: " << node.nama << "\n";
                    int tNum = 1;
                    for (const auto& t : node.tickets) {
                        cout << "       " << tNum++ << ". " << t.namaKereta 
                             << " (" << t.jamBerangkat << ") | " 
                             << t.asal << " -> " << t.tujuan << "\n";
                    }
                }
            }
        }
        if (kosong) {
            cout << "Belum ada data penumpang.\n";
        }
    }
};

// Instansiasi Hash Table Custom
CustomHashTable dataPenumpang;

// Helper function untuk membersihkan buffer input (mencegah infinite loop cin)
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void tambahTiket() {
    cout << "\n=== Tambah Tiket ===\n";
    string nama;
    cout << "Masukkan Nama Penumpang: ";
    getline(cin, nama);

    Ticket t;
    cout << "Nama Kereta   : "; getline(cin, t.namaKereta);
    cout << "Jam Berangkat : "; getline(cin, t.jamBerangkat);
    cout << "Stasiun Asal  : "; getline(cin, t.asal);
    cout << "Tujuan        : "; getline(cin, t.tujuan);

    // Dapatkan pointer ke daftar tiket milik penumpang tersebut (O(1))
    vector<Ticket>* listTiket = dataPenumpang.tambah(nama);
    listTiket->push_back(t);
    
    cout << "Tiket berhasil ditambahkan ke akun " << nama << "!\n";
}

void cariPenumpang() {
    cout << "\n=== Cari Penumpang ===\n";
    string nama;
    cout << "Masukkan Nama Penumpang: ";
    getline(cin, nama);

    vector<Ticket>* listTiket = dataPenumpang.cari(nama);
    if (listTiket != nullptr) {
        cout << "\nTiket milik " << nama << ":\n";
        int i = 1;
        for (const auto& t : *listTiket) {
            cout << i++ << ". " << t.namaKereta << " (" << t.jamBerangkat << ") | " 
                 << t.asal << " -> " << t.tujuan << "\n";
        }
    } else {
        cout << "Data penumpang tidak ditemukan.\n";
    }
}

void tampilSemuaData() {
    cout << "\n=== Seluruh Data Penumpang ===\n";
    // Memanggil metode cetakSemua dari Hash Table kustom kita untuk memperlihatkan pembagian Bucket
    dataPenumpang.cetakSemua();
}

void editTiket() {
    cout << "\n=== Edit Tiket ===\n";
    string nama;
    cout << "Masukkan Nama Penumpang: ";
    getline(cin, nama);

    vector<Ticket>* listTiket = dataPenumpang.cari(nama);
    if (listTiket != nullptr) {
        if (listTiket->empty()) {
            cout << "Penumpang tidak memiliki tiket.\n";
            return;
        }

        cout << "\nTiket milik " << nama << ":\n";
        for (size_t i = 0; i < listTiket->size(); ++i) {
            cout << i + 1 << ". " << (*listTiket)[i].namaKereta << " (" << (*listTiket)[i].jamBerangkat << ") | " 
                 << (*listTiket)[i].asal << " -> " << (*listTiket)[i].tujuan << "\n";
        }

        int index;
        cout << "Pilih nomor tiket yang akan diedit: ";
        if (!(cin >> index)) {
            clearInput();
            cout << "Input tidak valid.\n";
            return;
        }
        clearInput();

        if (index >= 1 && index <= (int)listTiket->size()) {
            Ticket& t = (*listTiket)[index - 1];
            cout << "Nama Kereta Baru (sebelumnya " << t.namaKereta << "): "; getline(cin, t.namaKereta);
            cout << "Jam Baru (sebelumnya " << t.jamBerangkat << "): "; getline(cin, t.jamBerangkat);
            cout << "Asal Baru (sebelumnya " << t.asal << "): "; getline(cin, t.asal);
            cout << "Tujuan Baru (sebelumnya " << t.tujuan << "): "; getline(cin, t.tujuan);
            cout << "Tiket berhasil diedit!\n";
        } else {
            cout << "Nomor tiket tidak valid.\n";
        }
    } else {
        cout << "Data penumpang tidak ditemukan.\n";
    }
}

void hapusTiket() {
    cout << "\n=== Hapus Tiket ===\n";
    string nama;
    cout << "Masukkan Nama Penumpang: ";
    getline(cin, nama);

    vector<Ticket>* listTiket = dataPenumpang.cari(nama);
    if (listTiket != nullptr) {
        if (listTiket->empty()) {
            cout << "Penumpang tidak memiliki tiket.\n";
            return;
        }

        cout << "\nTiket milik " << nama << ":\n";
        for (size_t i = 0; i < listTiket->size(); ++i) {
            cout << i + 1 << ". " << (*listTiket)[i].namaKereta << " (" << (*listTiket)[i].jamBerangkat << ") | " 
                 << (*listTiket)[i].asal << " -> " << (*listTiket)[i].tujuan << "\n";
        }

        int index;
        cout << "Pilih nomor tiket yang akan dihapus: ";
        if (!(cin >> index)) {
            clearInput();
            cout << "Input tidak valid.\n";
            return;
        }
        clearInput();

        if (index >= 1 && index <= (int)listTiket->size()) {
            listTiket->erase(listTiket->begin() + (index - 1));
            cout << "Tiket berhasil dihapus!\n";

            // Jika seluruh tiket habis, hapus node penumpang tersebut dari Hash Table
            if (listTiket->empty()) {
                dataPenumpang.hapus(nama);
                cout << "Seluruh tiket habis, data penumpang " << nama << " otomatis dihapus dari sistem Hash Table.\n";
            }
        } else {
            cout << "Nomor tiket tidak valid.\n";
        }
    } else {
        cout << "Data penumpang tidak ditemukan.\n";
    }
}

void generateDummyData() {
    // Membuat beberapa data dummy untuk keperluan pengujian
    Ticket t1 = {"Argo Bromo", "08:00", "Bandung", "Jakarta"};
    Ticket t2 = {"Taksaka", "14:00", "Jakarta", "Yogyakarta"};
    
    // Tambah t1 dan t2 ke Kafka
    dataPenumpang.tambah("Kafka")->push_back(t1);
    dataPenumpang.tambah("Kafka")->push_back(t2);

    Ticket t3 = {"Sancaka", "10:00", "Surabaya", "Solo"};
    dataPenumpang.tambah("Budi")->push_back(t3);
}

void menu() {
    int pilihan;
    do {
        cout << "\n================================\n";
        cout << "   MANAJEMEN TIKET KERETA API   \n";
        cout << "   (CUSTOM HASH TABLE SYSTEM)   \n";
        cout << "================================\n";
        cout << "1. Tambah Tiket\n";
        cout << "2. Cari Penumpang\n";
        cout << "3. Tampil Semua Data (Per Bucket)\n";
        cout << "4. Edit Tiket\n";
        cout << "5. Hapus Tiket\n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";

        if (!(cin >> pilihan)) {
            clearInput();
            continue;
        }
        clearInput(); // Membersihkan sisa enter (\n)

        switch (pilihan) {
            case 1: tambahTiket(); break;
            case 2: cariPenumpang(); break;
            case 3: tampilSemuaData(); break;
            case 4: editTiket(); break;
            case 5: hapusTiket(); break;
            case 0: cout << "Keluar dari program...\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 0);
}

int main() {
    generateDummyData();
    menu();
    return 0;
}