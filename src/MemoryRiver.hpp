#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <string>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

// The MemoryRiver class provides simple binary persistence for objects of type T.
// It reserves 'info_len' integers at the head of the file for metadata.
// Index returned by write() is the absolute byte offset in the file where the
// object starts (compatible across write/update/read).

template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;

    explicit MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // Read the n-th (1-based) header int into tmp
    void get_info(int &tmp, int n) {
        if (n > info_len || n <= 0) return;
        file.open(file_name, std::ios::in | std::ios::binary);
        if (!file.is_open()) return;
        file.seekg(static_cast<std::streamoff>((n - 1) * sizeof(int)), std::ios::beg);
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // Write tmp into the n-th (1-based) header int
    void write_info(int tmp, int n) {
        if (n > info_len || n <= 0) return;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) return;
        file.seekp(static_cast<std::streamoff>((n - 1) * sizeof(int)), std::ios::beg);
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // Append object t to the file; return the byte offset where it was written
    int write(T &t) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            // Try create if absent
            initialise(file_name);
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) return -1;
        }
        file.seekp(0, std::ios::end);
        std::streampos pos = file.tellp();
        int index = static_cast<int>(pos);
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
        return index;
    }

    // Overwrite the object at byte offset 'index' with t
    void update(T &t, const int index) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) return;
        file.seekp(static_cast<std::streamoff>(index), std::ios::beg);
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
    }

    // Read the object at byte offset 'index' into t
    void read(T &t, const int index) {
        file.open(file_name, std::ios::in | std::ios::binary);
        if (!file.is_open()) return;
        file.seekg(static_cast<std::streamoff>(index), std::ios::beg);
        file.read(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
    }

    // No-op for deletion in the no-reclamation model
    void Delete(int /*index*/) {
        // intentionally left blank
    }
};

#endif // BPT_MEMORYRIVER_HPP
