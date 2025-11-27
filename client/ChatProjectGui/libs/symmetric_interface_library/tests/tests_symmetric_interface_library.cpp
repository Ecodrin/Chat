#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <future>
#include <thread>
#include <cstring>
#include <filesystem>
#include <cassert>
#include <random>
#include <cstdio> 
#include <algorithm>

#include "bytes_utility.hpp"
#include "symmetric_encryption_interfaces.hpp"
#include "symmetric_context.hpp"

namespace symmetric_interface_library {

class MockBlockCipher : public InterfaceSymmetricAlgorithm {
public:
    explicit MockBlockCipher(size_t block_size = 16) : block_sz(block_size) {}

    size_t get_block_size() override {
        return static_cast<size_t>(block_sz);
    }

    void set_key(const std::vector<std::byte>& k) override {
        key = k;
        assert(!key.empty());
    }

    std::vector<std::byte> encryption(const std::vector<std::byte>& block) override {
        assert(block.size() == block_sz);
        return apply_xor(block, key);
    }

    std::vector<std::byte> decryption(const std::vector<std::byte>& block) override {
        return encryption(block);
    }
    ~MockBlockCipher() = default;
 
private:
    size_t block_sz;
    std::vector<std::byte> key;

    std::vector<std::byte> apply_xor(const std::vector<std::byte>& data, const std::vector<std::byte>& k) {
        std::vector<std::byte> out(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            out[i] = std::byte{
                static_cast<unsigned char>(
                    std::to_integer<unsigned char>(data[i]) ^
                    std::to_integer<unsigned char>(k[i % k.size()])
                )
            };
        }
        return out;
    }
};

}


using namespace symmetric_interface_library;


std::vector<std::byte> random_bytes(size_t n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<unsigned char> dis(0, 255);

    std::vector<std::byte> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = std::byte(dis(gen));
    }
    return data;
}

bool is_zeros_padding(PaddingModeEnum p) {
    return p == PaddingModeEnum::Zeros;
}

bool plaintext_matches_decrypted(
    const std::vector<std::byte>& plaintext,
    const std::vector<std::byte>& decrypted,
    PaddingModeEnum padding
) {
    if (!is_zeros_padding(padding)) {
        return plaintext == decrypted;
    }


    if (decrypted.size() < plaintext.size()) return false;
    if (!std::equal(plaintext.begin(), plaintext.end(), decrypted.begin())) {
        return false;
    }
    for (size_t i = plaintext.size(); i < decrypted.size(); ++i) {
        if (decrypted[i] != std::byte{0}) {
            return false;
        }
    }
    return true;
}

bool test_in_memory(
    const std::vector<std::byte>& plaintext,
    EncryptionModeEnum mode,
    PaddingModeEnum padding
) {
    const size_t block_size = 16;
    std::vector<std::byte> key = random_bytes(16);
    std::vector<std::byte> iv = (mode != EncryptionModeEnum::ECB)
        ? random_bytes(block_size)
        : std::vector<std::byte>{};

    auto cipher = SymmetricContext(
        std::make_shared<MockBlockCipher>(block_size),
        key,
        mode,
        padding,
        iv
    );

    std::vector<std::byte> encrypted, decrypted;
    auto f1 = cipher.encryption(plaintext, encrypted);
    f1.get();
    auto f2 = cipher.decryption(encrypted, decrypted);
    f2.get();

    return plaintext_matches_decrypted(plaintext, decrypted, padding);
}

bool test_file(
    const std::vector<std::byte>& plaintext,
    EncryptionModeEnum mode,
    PaddingModeEnum padding
) {
    const std::string in_file = "test_in.bin";
    const std::string enc_file = "test_enc.bin";
    const std::string dec_file = "test_dec.bin";


    {
        std::ofstream f(in_file, std::ios::binary);
        if (!f) {
            std::cerr << "Cannot create input file\n";
            return false;
        }
        if (!plaintext.empty()) {
            f.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        }
        f.close();
        if (!f) {
            std::cerr << "Write failed\n";
            return false;
        }
    }

    const size_t block_size = 16;
    std::vector<std::byte> key = random_bytes(16);
    std::vector<std::byte> iv = (mode != EncryptionModeEnum::ECB)
        ? random_bytes(block_size)
        : std::vector<std::byte>{};

    std::vector<std::any> additional_params;
    if (mode == EncryptionModeEnum::RandomDelta) {
        additional_params.push_back(0); // или другое значение
    }

    try {
        auto encryptor = SymmetricContext(
            std::make_shared<MockBlockCipher>(block_size),
            key,
            mode,
            padding,
            iv,
            additional_params
        );
        auto f1 = encryptor.encryption(in_file, enc_file);
        f1.get();

        auto decryptor = SymmetricContext(
            std::make_shared<MockBlockCipher>(block_size),
            key,
            mode,
            padding,
            iv,
            additional_params
        );
        auto f2 = decryptor.decryption(enc_file, dec_file);
        f2.get();

    } catch (const std::exception& e) {
        std::cerr << "File test failed: " << e.what() 
                  << " (mode=" << static_cast<int>(mode) 
                  << ", padding=" << static_cast<int>(padding) << ")\n";
        std::remove(in_file.c_str());
        std::remove(enc_file.c_str());
        std::remove(dec_file.c_str());
        return false;
    }


    std::vector<std::byte> recovered;
    {
        std::ifstream f(dec_file, std::ios::binary);
        if (!f) {
            std::cerr << "Cannot open decrypted file\n";
            std::remove(in_file.c_str());
            std::remove(enc_file.c_str());
            std::remove(dec_file.c_str());
            return false;
        }
        f.seekg(0, std::ios::end);
        size_t len = f.tellg();
        f.seekg(0);
        if (len > 0) {
            recovered.resize(len);
            f.read(reinterpret_cast<char*>(recovered.data()), len);
        }
    }

    std::remove(in_file.c_str());
    std::remove(enc_file.c_str());
    std::remove(dec_file.c_str());

    return plaintext_matches_decrypted(plaintext, recovered, padding);
}

int main() {
    std::vector<EncryptionModeEnum> modes = {
        EncryptionModeEnum::ECB,
        EncryptionModeEnum::CBC,
        EncryptionModeEnum::PCBC,
        EncryptionModeEnum::CFB,
        EncryptionModeEnum::OFB,
        EncryptionModeEnum::CTR,
        EncryptionModeEnum::RandomDelta,
    };

    std::vector<PaddingModeEnum> paddings = {
        PaddingModeEnum::Zeros,
        PaddingModeEnum::ANSIX923,
        PaddingModeEnum::PKCS7,
        PaddingModeEnum::ISO10126,
    };

    std::vector<size_t> lengths = {0, 1, 15, 16, 17, 31, 32, 33, 100, 257};

    int total = 0, passed = 0;

    for (auto mode : modes) {
        for (auto pad : paddings) {
            for (size_t len : lengths) {
                auto plaintext = random_bytes(len);

                total++;
                if (test_in_memory(plaintext, mode, pad)){ 
                    passed++;
                } else {
                    std::cout << "memory " << "mode: " << (int) mode << " padding: " << (int)pad << std::endl;
                }

                total++;
                if (test_file(plaintext, mode, pad)) {
                    passed++;
                } else {
                    std::cout << "file   " << "mode: " << (int) mode << " padding: " << (int)pad << std::endl;
                }
            }
        }
    }

    std::cout << "✅ Passed: " << passed << " / " << total << "\n";
    if (passed == total) {
        std::cout << "🎉 All tests passed!\n";
        return 0;
    } else {
        std::cerr << "❌ Some tests failed!\n";
        return 1;
    }

}