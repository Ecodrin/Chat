#pragma once

#include <memory>

#include "des.hpp"
#include "triple_des.hpp"
#include "deal.hpp"
#include "serpent.hpp"
#include "rc5.hpp"

namespace symmetric_algorithms {
    enum class SymmetricAlgorithmsEnum {
        Serpent,
        RC5,
        DES,
        DEAL,
        TripleDES,
    };
    
    std::shared_ptr<symmetric_interface_library::InterfaceSymmetricAlgorithm> get_alg(const SymmetricAlgorithmsEnum &alg_enum, const std::vector<std::byte> & key);
    size_t get_alg_key_size(const SymmetricAlgorithmsEnum &alg_enum);
    size_t get_alg_block_size(const SymmetricAlgorithmsEnum &alg_enum);
}