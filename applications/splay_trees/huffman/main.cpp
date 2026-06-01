#include "tree.h"

void writeBitStreamToFile(const std::vector<bool>& bits, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    unsigned char byte = 0;
    int bit_count = 0;

    for (bool bit : bits) {
        byte = (byte << 1) | bit;
        bit_count++;
        if (bit_count == 8) {
            out.put(byte);
            byte = 0;
            bit_count = 0;
        }
    }
    if (bit_count > 0 ) {
        byte <<= (8 - bit_count);
        out.put(byte);
    }
}

int main() {
    // Generate dummy source file to compress
    std::ofstream init_file("input.txt");
    init_file << "the_teeth_the_teeth_see_the_teeth";
    init_file.close();

    std::ifstream in("input.txt");
    if (!in.is_open()) {
        std::cerr << "Failed to load input file.\n";
        return 1;
    }

    AdaptiveHuffmanTree compression_engine;
    std::vector<bool> encoded_bitstream;
    char ch;

    std::cout << "Streaming bytes out of input.txt and encoding with Splay compression tree...\n";
    while (in.get(ch)) {
        size_t prev_size = encoded_bitstream.size();
        compression_engine.processCharacter(ch, encoded_bitstream);
        
        std::cout << "Char '" << ch << "' -> Binary String Segment: ";
        for (size_t i = prev_size; i < encoded_bitstream.size(); ++i) {
            std::cout << (encoded_bitstream[i] ? "1" : "0");
        }
        std::cout << "\n";
    }

    writeBitStreamToFile(encoded_bitstream, "compressed.bin");
    std::cout << "\nCompression Complete! Generated compressed.bin from raw bitstreams.\n";
    return 0;
}