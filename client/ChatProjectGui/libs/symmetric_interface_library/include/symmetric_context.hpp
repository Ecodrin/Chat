#pragma once

#include <iostream>
#include "encryption_modes.hpp"
#include "encryption_padding.hpp"
#include "symmetric_encryption_interfaces.hpp"

namespace symmetric_interface_library {
    class SymmetricContext {  
    public:
        SymmetricContext(
            std::shared_ptr<InterfaceSymmetricAlgorithm> symmetric_alg, 
            const std::vector<std::byte> &key, 
            EncryptionModeEnum encryption_mode_enum, 
            symmetric_interface_library::PaddingModeEnum padding_mode_enum, 
            const std::vector<std::byte> & iv = {}, 
            const std::vector<std::any> & additional_params = {});
        ~SymmetricContext() = default;
        void set_key(const std::vector<std::byte> &new_key);
        std::future<void> encryption(const std::vector<std::byte> & encrypted_block, std::vector<std::byte> & res);
        std::future<void> decryption(const std::vector<std::byte> & encrypted_block, std::vector<std::byte> & res);
        std::future<void> encryption(const std::string& input_file_path, const std::string& output_file_path);
        std::future<void> decryption(const std::string& input_file_path, const std::string& output_file_path);

    private:

        const size_t read_file_scale_block = 10000;
        std::vector<std::byte> processes(const std::vector<std::byte> & block, bool encrypt, bool make_padding = true);

        std::shared_ptr<InterfaceSymmetricAlgorithm> symmetric_alg;
        std::vector<std::byte> key;
        std::vector<std::byte> iv;
        std::vector<std::any> additional_params;
        std::shared_ptr<PaddingMode> padding_mode;
        std::shared_ptr<EncryptionMode> encryption_mode;


        void choose_padding(PaddingModeEnum padding_mode_enum);
        void choose_encryption_mode(EncryptionModeEnum encryption_mode_enum);
    };
}