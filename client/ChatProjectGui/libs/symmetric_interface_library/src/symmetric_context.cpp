#include "../include/symmetric_context.hpp"

namespace symmetric_interface_library {
    SymmetricContext::SymmetricContext(
            std::shared_ptr<InterfaceSymmetricAlgorithm> symmetric_alg,
            const std::vector<std::byte> &key, 
            symmetric_interface_library::EncryptionModeEnum encryption_mode_enum, 
            symmetric_interface_library::PaddingModeEnum padding_mode_enum, 
            const std::vector<std::byte> & iv, 
            const std::vector<std::any> & additional_params) : 
            symmetric_alg{symmetric_alg},
            key{key},
            iv{iv}, 
            additional_params{additional_params} {
        this->symmetric_alg->set_key(this->key);

    
        choose_padding(padding_mode_enum);
        choose_encryption_mode(encryption_mode_enum);
    }

    void SymmetricContext::set_key(const std::vector<std::byte> &new_key) {
        key = new_key;
        this->symmetric_alg->set_key(this->key);
    }

    std::future<void> SymmetricContext::encryption(const std::vector<std::byte>& encrypted_block, std::vector<std::byte> & res) {
        return std::async(std::launch::async, [this, encrypted_block, &res]() {
            res = processes(encrypted_block, true);
        });
    }


    std::future<void> SymmetricContext::decryption(const std::vector<std::byte>& encrypted_block, std::vector<std::byte> & res) {
        return std::async(std::launch::async, [this, encrypted_block, &res]() {
            res = processes(encrypted_block, false);
        });
    }


    std::future<void> SymmetricContext::encryption(const std::string& input_file_path, const std::string& output_file_path) {
    return std::async(std::launch::async, [this, input_file_path, output_file_path]() {
        std::ifstream input(input_file_path, std::ios::binary);
        std::ofstream output(output_file_path, std::ios::binary);
        
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open input file");
        }
        if (!output.is_open()) {
            throw std::runtime_error("Cannot open output file");
        }
        size_t block_size = symmetric_alg->get_block_size() * read_file_scale_block;
        std::vector<std::byte> buffer(block_size);

        size_t size_file = 0;
        while (input.read(reinterpret_cast<char*>(buffer.data()), block_size)) {
            auto processed_block = processes(buffer, true, false);
            output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
        }

        size_t bytes_read = input.gcount();
        if (bytes_read > 0) {
            std::vector<std::byte> last_block(buffer.begin(), buffer.begin() + bytes_read);
            auto processed_block = processes(last_block, true);
            output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
        }
        input.close();
        output.close();
        });
    }

    std::future<void> SymmetricContext::decryption(const std::string& input_file_path, const std::string& output_file_path) {
    return std::async(std::launch::async, [this, input_file_path, output_file_path]() {
        std::ifstream input(input_file_path, std::ios::binary);
        std::ofstream output(output_file_path, std::ios::binary);
        
        if (!input.is_open()) throw std::runtime_error("Cannot open input file");
        if (!output.is_open()) throw std::runtime_error("Cannot open output file");
        size_t block_size = symmetric_alg->get_block_size() * read_file_scale_block;
        std::vector<std::byte> buffer(block_size);  
        while (input.read(reinterpret_cast<char*>(buffer.data()), block_size)) {
            auto processed_block = processes(buffer, false, false);
            output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
        }

        size_t bytes_read = input.gcount();
        if (bytes_read > 0) {
            std::vector<std::byte> last_block(buffer.begin(), buffer.begin() + bytes_read);
            auto processed_block = processes(last_block, false);
            output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
        }
        input.close();
        output.close();
        });
    }

    std::vector<std::byte> SymmetricContext::processes(const std::vector<std::byte> & block, bool encrypt, bool make_padding) {
        std::vector<std::byte> augmented_block{block};
        size_t n = block.size() / symmetric_alg->get_block_size();
        size_t m = block.size() % symmetric_alg->get_block_size();
        if (encrypt && make_padding) {
            padding_mode->padding(augmented_block, (n + 1) * symmetric_alg->get_block_size());
        }

        std::vector<std::byte> res;
        if (encrypt) {
            res = encryption_mode->encryption(augmented_block);
        } else {
            res = encryption_mode->decryption(augmented_block);
        }

        if (!encrypt && make_padding) {
            padding_mode->unpadding(res);
        }
        return res;
    }

    void SymmetricContext::choose_padding(PaddingModeEnum padding_mode_enum) {
        switch (padding_mode_enum) {
        case PaddingModeEnum::Zeros:
            padding_mode = std::make_shared<Zeros>();
            break;
        case PaddingModeEnum::ANSIX923:
            padding_mode = std::make_shared<ANSIX923>();
            break;
        case PaddingModeEnum::ISO10126:
            padding_mode = std::make_shared<ISO10126>();
            break;
        case PaddingModeEnum::PKCS7:
            padding_mode = std::make_shared<PKCS7>();
            break;
        default:
            break;
        }
    }

    
    void SymmetricContext::choose_encryption_mode(EncryptionModeEnum encryption_mode_enum) {
        switch (encryption_mode_enum) {
        case EncryptionModeEnum::ECB:
            encryption_mode = std::make_shared<ECB>(symmetric_alg);
            break;
        case EncryptionModeEnum::CBC:
            encryption_mode = std::make_shared<CBC>(symmetric_alg, iv);
            break;
        case EncryptionModeEnum::PCBC:
            encryption_mode = std::make_shared<PCBC>(symmetric_alg, iv);
            break;
        case EncryptionModeEnum::CFB:
            encryption_mode = std::make_shared<CFB>(symmetric_alg, iv);
            break;    
        case EncryptionModeEnum::OFB:
            encryption_mode = std::make_shared<OFB>(symmetric_alg, iv);
            break;      
        case EncryptionModeEnum::CTR:
            encryption_mode = std::make_shared<CTR>(symmetric_alg, iv);
            break;   
        case EncryptionModeEnum::RandomDelta:
            encryption_mode = std::make_shared<RandomDelta>(symmetric_alg, iv);;
            break;
        default:
            throw std::runtime_error("incorrect mode encryption");
        }
    }

    size_t SymmetricContext::get_read_file_scale_block() {
        return read_file_scale_block;
    }
}
