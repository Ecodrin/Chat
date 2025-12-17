#include "../include/enc_utility.hpp"


namespace symmetric_algorithms {
    std::shared_ptr<symmetric_interface_library::InterfaceSymmetricAlgorithm> get_alg(const SymmetricAlgorithmsEnum &alg_enum, const std::vector<std::byte> & key) {
        switch (alg_enum) {
        case SymmetricAlgorithmsEnum::DES:
            return std::make_shared<DES>(key);
        case SymmetricAlgorithmsEnum::TripleDES:
            return std::make_shared<TripleDES>(key);
        case SymmetricAlgorithmsEnum::DEAL:
            return std::make_shared<DEAL>(key);
        case SymmetricAlgorithmsEnum::Serpent:
            return std::make_shared<Serpent>(key);
        case SymmetricAlgorithmsEnum::RC5:
            return std::make_shared<RC5>(key);
        default:
            throw std::invalid_argument("incorrect alg");
        }
    }

    size_t get_alg_key_size(const SymmetricAlgorithmsEnum &alg_enum) {
        switch (alg_enum) {
        case SymmetricAlgorithmsEnum::DES:
            return DES::standart_key_size;
        case SymmetricAlgorithmsEnum::TripleDES:
            return TripleDES::standart_key_size;
        case SymmetricAlgorithmsEnum::DEAL:
            return DEAL::standart_key_size;
        case SymmetricAlgorithmsEnum::Serpent:
            return Serpent::standart_key_size;
        case SymmetricAlgorithmsEnum::RC5:
            return RC5::standart_key_size;
        default:
            throw std::invalid_argument("incorrect alg");
        }
    }

    size_t get_alg_block_size(const SymmetricAlgorithmsEnum &alg_enum) {
        switch (alg_enum) {
        case SymmetricAlgorithmsEnum::DES:
            return DES::block_size;
        case SymmetricAlgorithmsEnum::TripleDES:
            return TripleDES::block_size;
        case SymmetricAlgorithmsEnum::DEAL:
            return DEAL::block_size;
        case SymmetricAlgorithmsEnum::Serpent:
            return Serpent::block_size;
        case SymmetricAlgorithmsEnum::RC5:
            return RC5::standart_block_size;
        default:
            throw std::invalid_argument("incorrect alg");
        }
    }
}