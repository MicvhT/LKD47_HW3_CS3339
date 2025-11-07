// Build:   g++ -std=c++17 -O2 -Wall -Wextra -o LKD47_HW3_CS3339 LKD47_HW3_CS3339.cpp
// Usage:   ./LKD47_HW3_CS3339 num_entries associativity input_memory_reference_file

// ./LKD47_HW2_CS3339 4 2 input_memory_reference_file
// cat cache_sim_output 
// (or nano or just open the file in GUI lol)

#include <algorithm>
#include <cctype>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

bool isPowerOfTwo(long long x) {
    return x > 0 && ((x & (x - 1)) == 0);
}

void printUsage(const char* prog) {
    cerr << "Usage:\n  " << prog
         << " num_entries associativity input_memory_reference_file\n";
}

int main(int argc, char* argv[]) {
    // ----- Parse & validate CLI args -----
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    long long num_entries = 0;
    long long associativity = 0;
    try {
        num_entries   = stoll(argv[1]);
        associativity = stoll(argv[2]);
    } catch (...) {
        cerr << "Error: num_entries and associativity must be integers.\n";
        return 1;
    }

    string input_path = argv[3];

    if (num_entries <= 0) {
        cerr << "Error: num_entries must be > 0.\n";
        return 1;
    }
    if (!isPowerOfTwo(num_entries)) {
        cerr << "Error: num_entries must be a power of two.\n";
        return 1;
    }
    if (associativity <= 0) {
        cerr << "Error: associativity must be > 0.\n";
        return 1;
    }
    if (num_entries % associativity != 0) {
        cerr << "Error: associativity must evenly divide num_entries.\n";
        return 1;
    }

    long long num_sets_ll = num_entries / associativity;
    if (num_sets_ll <= 0) {
        cerr << "Error: num_sets computed as zero or negative (invalid inputs).\n";
        return 1;
    }

    // ----- Open files -----
    ifstream in(input_path);
    if (!in) {
        cerr << "Error: failed to open input file: " << input_path << "\n";
        return 1;
    }

    ofstream out("cache_sim_output");
    if (!out) {
        cerr << "Error: failed to open output file: cache_sim_output\n";
        return 1;
    }

    // ----- Cache data structure -----
    // One deque (FIFO queue) per set, holding tags.
    size_t num_sets = static_cast<size_t>(num_sets_ll);
    size_t ways     = static_cast<size_t>(associativity);
    vector<deque<long long>> sets(num_sets);

    // ----- Read addresses and simulate -----
    long long addr;
    while (in >> addr) {
        // Map address -> set index & tag
        // Since each block is 1 word, index = addr % num_sets, tag = addr / num_sets.
        size_t set_idx = static_cast<size_t>(addr % num_sets_ll);
        long long tag  = addr / num_sets_ll;

        auto& q = sets[set_idx];

        // Check hit: tag present in the set
        bool hit = find(q.begin(), q.end(), tag) != q.end();

        // Emit result line
        out << addr << " : " << (hit ? "HIT" : "MISS") << '\n';

        // On miss: insert tag with FIFO policy (evict oldest if full)
        if (!hit) {
            if (q.size() >= ways) {
                q.pop_front();         // evict oldest
            }
            q.push_back(tag);          // append newest
        }
    }

    in.close();
    out.close();

    return 0;
}
