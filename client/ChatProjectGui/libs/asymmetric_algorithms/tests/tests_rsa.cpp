#include "rsa.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <filesystem>

#include "bytes_utility.hpp"

using namespace asymmetric_algorithms;

void check_file_2() {
    std::string s = ".png";
    std::string input_file_name = "/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/test";
    std::filesystem::path output_file_name = input_file_name + "_bin" + s;
    std::filesystem::path result_file_name = input_file_name + "_result" + s;
    input_file_name += s;
    std::filesystem::path input_file_name_path = input_file_name;


    // RSA alg(PrimalityTestEnum::MillerRabinPrimalityTest, 0.99, 2048);
    // alg.save_keys("/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/keys/key1");

    RSA alg("/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/keys/key1.pub", 
        "/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/keys/key1");
    
    std::cout << "start size: " << std::filesystem::file_size(input_file_name_path) << std::endl;
    alg.encryption(input_file_name, output_file_name).get();
    
    std::cout << "encrypt size: " << std::filesystem::file_size(output_file_name) << std::endl;
    alg.decryption(output_file_name, result_file_name).get();
    std::cout << "decrypt size: " << std::filesystem::file_size(result_file_name) << std::endl;

}

void test_block() {
    RSA alg("/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/keys/key1.pub", 
        "/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/asymmetric_algorithms/tests/keys/key1");
    std::vector<std::byte> a = bytes_utility::random_bytes_vector(2000);
    std::vector<std::byte> enc;
    std::cout << "SRC size: " << a.size() << std::endl; 
    alg.encryption(a, enc).get();
    std::cout << "ENC size: " << enc.size()  << std::endl;
    std::vector<std::byte> dec;
    alg.decryption(enc, dec).get();
    std::cout << "DEC size: " << dec.size() << std::endl;
    assert(a == dec);


}

void check_time_tests(std::function<void(void)> f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Время выполнения: " << duration_ns.count() << " нс\n";

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Время выполнения: " << duration_ms.count() << " мс\n";
    using seconds_f = std::chrono::duration<double>;
    auto duration_s = std::chrono::duration_cast<seconds_f>(end - start);
    std::cout << "Время выполнения: " << duration_s.count() << " с\n";

}

void test_convert() {
    auto t = bytes_utility::random_bytes_vector(32);
    t.insert(t.begin(), std::byte{0x01});
    t.insert(t.begin(), std::byte{0x00});
    auto v = RSA::convert_to_cpp_int(t);
    auto r = RSA::convert_to_bytes_vector(v);
    bytes_utility::print_bytes_vector(std::cout, t, " ");
    std::cout << std::endl;
    bytes_utility::print_bytes_vector(std::cout, r, " ");
    std::cout << std::endl;
    assert(t == r);
}

int main() {   
    // check_time_tests(check_file_2);
    test_block();
    // test_convert();
    //     std::byte{0},
    //     std::byte{3},
    //     std::byte{8},
    //     std::byte{34},
    // };
    // RSA alg(PrimalityTestEnum::MillerRabinPrimalityTest, 0.9, 512);
    // std::vector<std::byte> enc, decr;
    // bytes_utility::print_bytes_vector(std::cout, a, " ");
    // std::cout << std::endl;
    // alg.encryption(a, enc).get();
    // std::cout << "=====\n";
    // bytes_utility::print_bytes_vector(std::cout, enc, " ");
    // std::cout << std::endl;
    // alg.decryption(enc, decr).get();
    // std::cout << "=====\n";
    // bytes_utility::print_bytes_vector(std::cout, decr, " ");
    // std::cout << std::endl;
} 