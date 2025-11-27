#pragma once

#include <iostream>
#include <vector>
#include <random>

#include "bytes_utility.hpp"

namespace symmetric_interface_library{
    enum class PaddingModeEnum {
        Zeros,
        ANSIX923,
        PKCS7,
        ISO10126,
    };

    class PaddingMode {
    public:
        virtual void padding(std::vector<std::byte> & block, const size_t count_required_bytes) = 0;
        virtual void unpadding(std::vector<std::byte> & block) = 0;
    };

    class Zeros : public PaddingMode {
    public:
        Zeros() = default;
        void padding(std::vector<std::byte> & block, const size_t count_required_bytes) override;
        void unpadding(std::vector<std::byte> & block) override;
    };

    class ANSIX923 : public PaddingMode {
    public:
        ANSIX923() = default;
        void padding(std::vector<std::byte> & block, const size_t count_required_bytes) override;
        void unpadding(std::vector<std::byte> & block) override;
    };

    class PKCS7 : public PaddingMode {
    public:
        PKCS7() = default;
        void padding(std::vector<std::byte> & block, const size_t count_required_bytes) override;
        void unpadding(std::vector<std::byte> & block) override;
    };

    class ISO10126 : public PaddingMode {
    public:
        ISO10126() = default;
        void padding(std::vector<std::byte> & block, const size_t count_required_bytes) override;
        void unpadding(std::vector<std::byte> & block) override;
    };
}