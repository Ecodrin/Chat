#include "../include/encryption_padding.hpp"


namespace symmetric_interface_library {
    void Zeros::padding(std::vector<std::byte> & block, const size_t count_required_bytes) {
        block.resize(count_required_bytes);
    }

    void Zeros::unpadding(std::vector<std::byte> & block) {
        if(block.size() == 0) {
            return;
        }
        int n = -1;
        for (int i = block.size() - 1; i >= 0; --i) {
            if (block[i] != std::byte(0)) {
                n = i;
                break;
            }
        }
        if(n == -1) {
            return;
        }
        block.resize(block.size() - n);
    }

    void ANSIX923::padding(std::vector<std::byte> & block, const size_t count_required_bytes) {
        size_t old_size = block.size();
        block.resize(count_required_bytes);
        if(count_required_bytes == 0 || count_required_bytes < old_size) {
            return;
        }
        block[block.size() - 1] = static_cast<std::byte>(count_required_bytes - old_size);
    }

    void ANSIX923::unpadding(std::vector<std::byte> & block) {
        if(block.size() == 0) {
            return;
        }
        size_t n = static_cast<size_t>(block.back());
        if (n == 0 || n > block.size()) {
            return;
        }
        for (size_t i = block.size() - n; i < block.size() - 1; ++i) {
            if (block[i] != std::byte(0)) {
                // throw std::runtime_error("Invalid ANSIX923 padding");
                return;
            }
        }
        block.resize(block.size() - n);
    }

    void PKCS7::padding(std::vector<std::byte> & block, const size_t count_required_bytes) {
        size_t old_size = block.size();
        block.resize(count_required_bytes);
        if(count_required_bytes == 0 || count_required_bytes < old_size) {
            return;
        }
        for(size_t i = old_size; i < count_required_bytes; ++i) {
            block[i] = static_cast<std::byte>(count_required_bytes - old_size);
        }
    }

    void PKCS7::unpadding(std::vector<std::byte> & block) {
        if(block.size() == 0) {
            return;
        }
        size_t n = static_cast<size_t>(block.back());
        if (n == 0 || n > block.size()) {
            return;
        }
        for (size_t i = block.size() - n; i < block.size(); ++i) {
            if (block[i] != static_cast<std::byte>(n)) {
                return;
            }
        }
        block.resize(block.size() - n);
    }

    void ISO10126::padding(std::vector<std::byte> & block, const size_t count_required_bytes) {
        size_t old_size = block.size();
        block.resize(count_required_bytes);
        if(count_required_bytes == 0 || count_required_bytes < old_size) {
            return;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 255);
        for(size_t i = old_size; i < count_required_bytes - 1; ++i) {
            block[i] = static_cast<std::byte>(dist(gen));
        }
        block[block.size() - 1] = static_cast<std::byte>(count_required_bytes - old_size);
    }

    void ISO10126::unpadding(std::vector<std::byte> & block) {
        if(block.size() == 0) {
            return;
        }
        size_t n = static_cast<size_t>(block.back());
        if (n == 0 || n > block.size()) {
            return;
        }
        block.resize(block.size() - n);
    }
}
