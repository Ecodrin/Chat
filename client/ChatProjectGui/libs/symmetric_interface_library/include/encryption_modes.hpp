#pragma once


#include <iostream>
#include "symmetric_encryption_interfaces.hpp"

namespace symmetric_interface_library {
    enum class EncryptionModeEnum {
        ECB,
        CBC,
        PCBC,
        CFB,
        OFB,
        CTR,
        RandomDelta,
    }; 

    class EncryptionMode {
    protected:
        std::shared_ptr<InterfaceSymmetricAlgorithm> alg;
    public:
        virtual std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) = 0;
        virtual std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) = 0;
    };

    class ECB : public EncryptionMode {
    public:
        ECB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };

    class CBC : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        CBC(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };
    
    class PCBC : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        PCBC(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };

    class CFB : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        CFB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };

    class OFB : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        OFB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };

    class CTR : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        CTR(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };

    class RandomDelta : public EncryptionMode {
    protected:
        std::vector<std::byte> iv;
    public:
        RandomDelta(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv);
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };
}