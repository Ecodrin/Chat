#include <bytes_utility.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstddef>

using namespace bytes_utility;

bool vectors_equal(const std::vector<std::byte>& a, const std::vector<std::byte>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

std::vector<std::byte> from_hex(const std::vector<uint8_t>& bytes) {
    std::vector<std::byte> result;
    result.reserve(bytes.size());
    for (uint8_t b : bytes) {
        result.push_back(std::byte{b});
    }
    return result;
}

bool operator==(const std::vector<std::byte>& a, const std::vector<std::byte>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (static_cast<uint8_t>(a[i]) != static_cast<uint8_t>(b[i])) return false;
    }
    return true;
}

void test_empty_input() {
    std::vector<std::byte> input = {};
    std::vector<size_t> rule = {1, 2, 3};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    assert(result.empty());
}

void test_simple_permutation_forward_zero() {
    std::vector<std::byte> input = {std::byte{0b10110000}, std::byte{0b01011000}};
    std::vector<size_t> rule = {0, 1};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b10000000}};
    assert(vectors_equal(result, expected));
}

void test_reverse_byte_forward_zero() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {7, 6, 5, 4, 3, 2, 1, 0};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b00001101}};
    assert(vectors_equal(result, expected));
}

void test_forward_first_index() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {8, 7, 6, 5, 4, 3, 2, 1};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderFirstIndex);
    std::vector<std::byte> expected = {std::byte{0b00001101}};
    assert(vectors_equal(result, expected));
}

void test_reverse_order_zero_index() {
    std::vector<std::byte> input = {std::byte{0b10110000}, std::byte{0b01011000}};
    std::vector<size_t> rule = {15, 14};
    auto result = permutations(input, rule, PermutationsOrderRule::ReverseOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b10000000}};
    assert(vectors_equal(result, expected));
}

void test_reverse_order_first_index() {
    std::vector<std::byte> input = {std::byte{0b10110000}, std::byte{0b01011000}};
    std::vector<size_t> rule = {16, 15};
    auto result = permutations(input, rule, PermutationsOrderRule::ReverseOrderFirstIndex);
    std::vector<std::byte> expected = {std::byte{0b10000000}};
    assert(vectors_equal(result, expected));
}

void test_invalid_index_too_large_forward_zero() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {8};
    try {
        auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
        assert(false);
    } catch (const std::runtime_error&) {
    }
}

void test_invalid_index_too_large_forward_first() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {9};
    try {
        auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderFirstIndex);
        assert(false);
    } catch (const std::runtime_error&) {
    }
}

void test_invalid_index_zero_with_first() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {0};
    try {
        auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderFirstIndex);
        assert(false);
    } catch (const std::runtime_error&) {
    }
}

void test_invalid_index_zero_with_first_reverse() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {0};
    try {
        auto result = permutations(input, rule, PermutationsOrderRule::ReverseOrderFirstIndex);
        assert(false);
    } catch (const std::runtime_error&) {
    }
}

void test_invalid_index_reverse_zero() {
    std::vector<std::byte> input = {std::byte{0b10110000}};
    std::vector<size_t> rule = {9};
    try {
        auto result = permutations(input, rule, PermutationsOrderRule::ReverseOrderZeroIndex);
        assert(false);
    } catch (const std::runtime_error&) {
    }
}

void test_multiple_output_bytes() {
    std::vector<std::byte> input = {std::byte{0b11001100}, std::byte{0b00110011}};
    std::vector<size_t> rule = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b11001100}, std::byte{0b00110011}}; // 0xCC, 0x33
    assert(vectors_equal(result, expected));
}

void test_non_multiple_of_8() {
    std::vector<std::byte> input = {std::byte{0b11001100}, std::byte{0b00110011}};
    std::vector<size_t> rule = {7, 6, 5};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b00100000}};
    assert(vectors_equal(result, expected));
}

void test_padding_shift() {
    std::vector<std::byte> input = {std::byte{0b11111111}};
    std::vector<size_t> rule = {7, 6};
    auto result = permutations(input, rule, PermutationsOrderRule::ForwardOrderZeroIndex);
    std::vector<std::byte> expected = {std::byte{0b11000000}};
    assert(vectors_equal(result, expected));
}

void test_no_carry() {
    auto a = from_hex({0x12, 0x34});
    auto b = from_hex({0x01, 0x02});
    auto expected = from_hex({0x13, 0x36});
    assert(adding_bytes_vectors(a, b) == expected);
}

void test_carry_in_low_byte() {
    auto a = from_hex({0x00, 0xFF});
    auto b = from_hex({0x00, 0x01});
    auto expected = from_hex({0x01, 0x00});
    assert(adding_bytes_vectors(a, b) == expected);
}

void test_final_carry_overflow() {
    auto a = from_hex({0xFF, 0xFF});
    auto b = from_hex({0x00, 0x01});
    auto expected = from_hex({0x01, 0x00, 0x00});
    assert(adding_bytes_vectors(a, b) == expected);
}

void test_different_lengths() {
    auto a = from_hex({0x12, 0x34, 0x56}); // 0x123456
    auto b = from_hex({0x78, 0x9A});        //     0x789A
    auto expected = from_hex({0x12, 0xAC, 0xF0}); // 0x123456 + 0x789A = 0x12ACF0
    assert(adding_bytes_vectors(a, b) == expected);
}

void test_add_zero() {
    auto a = from_hex({0xA1, 0xB2});
    auto b = from_hex({0x00, 0x00});
    assert(adding_bytes_vectors(a, b) == a);
}

void test_chain_carry() {
    auto a = from_hex({0xFF, 0xFF, 0xFF});
    auto b = from_hex({0x00, 0x00, 0x01});
    auto expected = from_hex({0x01, 0x00, 0x00, 0x00});
    assert(adding_bytes_vectors(a, b) == expected);
}

void test_counting_units() {
    auto a = std::byte{0b01110101};
    assert(counting_number_units(a) == 5);

    auto b = std::byte{0b01010101};
    assert(counting_number_units(b) == 4);

    auto c = std::byte{0b00000001};
    assert(counting_number_units(c) == 1);

    auto d = std::byte{0b10000000};
    assert(counting_number_units(d) == 1);

    auto e = std::byte{0b00000000};
    assert(counting_number_units(e) == 0);
}


void test_rotate_left() {
    std::vector<std::byte> a = {
        std::byte{0b00001111},
        std::byte{0b01000000}
    };
    std::vector<std::byte> expected = {
        std::byte{0b01111010},
        std::byte{0b00000000}
    };
    auto res = cycling_rotate_left(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));

    a = {
        std::byte{0b01001111},
        std::byte{0b01000000}
    };
    expected = {
        std::byte{0b01111010},
        std::byte{0b00000010}
    };
    res = cycling_rotate_left(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));

    a = {
        std::byte{0b01001111},
        std::byte{0b01001110}
    };
    expected = {
        std::byte{0b01111010},
        std::byte{0b01110010}
    };
    res = cycling_rotate_left(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));


    a = {
        std::byte{0b01001111},
        std::byte{0b01001110}
    };
    expected = {
        std::byte{0b10000000}
    };
    res = cycling_rotate_left(a, 1, 4);
    assert(vectors_equal(res, expected));


    a = {
        std::byte{0b01001111},
        std::byte{0b01001110}
    };
    expected = {
        std::byte{0b00111101},
        std::byte{0b01000000}
    };
    res = cycling_rotate_left(a, 2, 10);
    assert(vectors_equal(res, expected));
}


void test_rotate_right() {
    std::vector<std::byte> a = {
        std::byte{0b00001111},
        std::byte{0b01000000}
    };
    std::vector<std::byte> expected = {
        std::byte{0b00000001},
        std::byte{0b11101000}
    };
    auto res = cycling_rotate_right(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));

    a = {
        std::byte{0b01001111},
        std::byte{0b01000000}
    };
    expected = {
        std::byte{0b00001001},
        std::byte{0b11101000}
    };
    res = cycling_rotate_right(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));

    a = {
        std::byte{0b00010000},
        std::byte{0b01001111}
    };
    expected = {
        std::byte{0b11100010},
        std::byte{0b00001001}
    };
    res = cycling_rotate_right(a, 3, a.size() * 8);
    assert(vectors_equal(res, expected));
}


void test_concat_arrays() {
    // Основной тест из условия
    {
        std::vector<std::byte> a = {
            std::byte{0b10010000},
            std::byte{0b01001111}
        };
        std::vector<std::byte> b = {
            std::byte{0b10010001},
            std::byte{0b11001111}
        };
        std::vector<std::byte> expected = {
            std::byte{0b10011001},
            std::byte{0b00011100}
        };
        auto res = connect_arrays(a, 4, b, 10);
        assert(vectors_equal(res, expected));
    }
    {
        std::vector<std::byte> a = {std::byte{0b11001100}};
        std::vector<std::byte> b = {std::byte{0b10101010}};
        std::vector<std::byte> expected = {std::byte{0b11001010}};
        auto res = connect_arrays(a, 4, b, 4);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b10000000}};
        std::vector<std::byte> b = {std::byte{0b01000000}};
        std::vector<std::byte> expected = {std::byte{0b10000000}};
        auto res = connect_arrays(a, 1, b, 1);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11110000}};
        std::vector<std::byte> b = {std::byte{0b11001100}, std::byte{0b00111100}};
        std::vector<std::byte> expected = {
            std::byte{0b11110011},
            std::byte{0b00110000}
        };
        auto res = connect_arrays(a, 6, b, 10);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {};
        std::vector<std::byte> b = {std::byte{0b10101010}};
        std::vector<std::byte> expected = {std::byte{0b10100000}};
        auto res = connect_arrays(a, 0, b, 3);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11111111}};
        std::vector<std::byte> b = {};
        std::vector<std::byte> expected = {std::byte{0b11110000}};
        auto res = connect_arrays(a, 4, b, 0);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b10101010}, std::byte{0b11001100}};
        std::vector<std::byte> b = {std::byte{0b11110000}, std::byte{0b00111100}};
        std::vector<std::byte> expected = {
            std::byte{0b10101010},
            std::byte{0b11111100}
        };
        auto res = connect_arrays(a, 10, b, 6);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11111111}};
        std::vector<std::byte> b = {std::byte{0b00000000}};
        std::vector<std::byte> expected = {
            std::byte{0b11111111},
            std::byte{0b00000000}
        };
        auto res = connect_arrays(a, 8, b, 8);
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11001100}};
        std::vector<std::byte> b = {std::byte{0b10101010}};
        std::vector<std::byte> expected = {std::byte{0b11001101}};
        auto res = connect_arrays(a, 5, b, 3);
        assert(vectors_equal(res, expected));
    }

}

void test_rotate() {
    {
        std::vector<std::byte> a = {std::byte{0b11001100}};
        auto res = rotate_left(a, 4, a.size() * 8);
        std::vector<std::byte> expected = {std::byte{0b11000000}};
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11001100}, std::byte{0b11010110}};
        auto res = rotate_left(a, 4, a.size() * 8);
        std::vector<std::byte> expected = {std::byte{0b11001101}, std::byte{0b01100000}};
        assert(vectors_equal(res, expected));
    }


    {
        std::vector<std::byte> a = {std::byte{0b11001100}};
        auto res = rotate_right(a, 4, a.size() * 8);
        std::vector<std::byte> expected = {std::byte{0b00001100}};      
        assert(vectors_equal(res, expected));
    }

    {
        std::vector<std::byte> a = {std::byte{0b11001100}, std::byte{0b11010110}};
        auto res = rotate_right(a, 4, a.size() * 8);
        std::vector<std::byte> expected = {std::byte{0b00001100}, std::byte{0b11001101}};
        assert(vectors_equal(res, expected));
    }
}



int main() {
    test_empty_input();
    test_simple_permutation_forward_zero();
    test_reverse_byte_forward_zero();
    test_forward_first_index();
    test_reverse_order_zero_index();
    test_reverse_order_first_index();
    test_invalid_index_too_large_forward_zero();
    test_invalid_index_too_large_forward_first();
    test_invalid_index_zero_with_first();
    test_invalid_index_zero_with_first_reverse();
    test_invalid_index_reverse_zero();
    test_multiple_output_bytes();
    test_non_multiple_of_8();
    test_padding_shift();

    test_no_carry();
    test_carry_in_low_byte();
    test_final_carry_overflow();
    test_different_lengths();
    test_add_zero();
    test_chain_carry();
    

    test_counting_units();

    test_rotate_left();
    test_rotate_right();

    test_concat_arrays();

    test_rotate();

    std::cout << "\nAll tests passed successfully!\n";


    
    return 0;
}