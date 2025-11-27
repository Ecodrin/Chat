#include "primality_tests.hpp"


namespace asymmetric_algorithms {
    double PrimalityTest::check_primality(const boost::multiprecision::cpp_int& x, double probability) {
        if(probability < 0.5 || probability >= 1.0) {
            throw std::invalid_argument("probability must be in [0.5, 1)");
        }

        if(x < 0) {
            throw std::invalid_argument("incorrect x");
        }

        size_t k = calculate_k(probability);
        if(k >= x - 1) {
            throw std::invalid_argument("incorrect arg: number iterations > x");
        }

        for(size_t i = 0; i < k - 1; ++i) {
            PrimalityTest::ProccessIterationStatus status = proccess_iteration(x);
            switch (status) {
            case PrimalityTest::ProccessIterationStatus::NOTPRIME:
                return 0;
            case PrimalityTest::ProccessIterationStatus::CONTINIEFOR:
                continue;
            default:
                break;
            }
        }
        if(proccess_iteration(x, true) == PrimalityTest::ProccessIterationStatus::NOTPRIME) {
            return 0;
        }
        return calculate_res_probability(k);
    }

    double PrimalityTest::calculate_res_probability(size_t k) {
        return 1 - 1.0f/ std::pow(2.0, k);
    }

    size_t PrimalityTest::calculate_k(double probability) {
        return static_cast<size_t>(std::ceil(std::log2(1 / (1 - probability))));
    }

    PrimalityTest::ProccessIterationStatus FermatPrimalityTest::proccess_iteration(const boost::multiprecision::cpp_int& n, bool last_iteration) {
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(2, n - 1);
        boost::multiprecision::cpp_int a = dist(rng);
        while(std::find(primality_witnesses.begin(), primality_witnesses.end(), a) != primality_witnesses.end()) {
            a = dist(rng);
        }
        primality_witnesses.push_back(a);
        if(last_iteration) {
            primality_witnesses.clear();
        }

        if(SymbolService::gcd(a, n) != 1) {
            return PrimalityTest::ProccessIterationStatus::NOTPRIME;
        }
        return (PrimalityTest::ProccessIterationStatus)(SymbolService::mod_pow(a, n - 1, n) == 1);
    }

    PrimalityTest::ProccessIterationStatus SolovayStrassenPrimalityTest::proccess_iteration(const boost::multiprecision::cpp_int& n, bool last_iteration) {
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int>  dist(2, n - 2);
        boost::multiprecision::cpp_int a = dist(rng);
        while(std::find(primality_witnesses.begin(), primality_witnesses.end(), a) != primality_witnesses.end()) {
            a = dist(rng);
        }
        primality_witnesses.push_back(a);
        if(last_iteration) {
            primality_witnesses.clear();
        }

        if(gcd(n, a) != 1) {
            return PrimalityTest::ProccessIterationStatus::NOTPRIME;
        }
        return (PrimalityTest::ProccessIterationStatus)(SymbolService::mod_pow(a, (n - 1) / 2, n) == (n + SymbolService::calculate_jakobi_symbol(a, n)) % n);
    }

    size_t MillerRabinPrimalityTest::calculate_k(double probability) {
        return static_cast<size_t>(std::ceil(std::log2(1 / (1 - probability)) / 2));
    }

    double MillerRabinPrimalityTest::calculate_res_probability(size_t k) {
        return 1 - 1/(std::pow(4, k));
    }

    PrimalityTest::ProccessIterationStatus MillerRabinPrimalityTest::proccess_iteration(const boost::multiprecision::cpp_int & n, bool last_iteration) {
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int>  dist(2, n - 2);
        boost::multiprecision::cpp_int a = dist(rng);
        while(std::find(primality_witnesses.begin(), primality_witnesses.end(), a) != primality_witnesses.end()) {
            a = dist(rng);
        }
        primality_witnesses.push_back(a);
        if(last_iteration) {
            primality_witnesses.clear();
        }
        if(t == 0) {
            t = n - 1;
            s = 0;
            while (t % 2 == 0) {
                s++;
                t /= 2;
            }
        }

        auto x = SymbolService::mod_pow(a, t, n);
        if(x == 1 || x == n - 1) {
            if(last_iteration) {
                clear_values();
            }
            return PrimalityTest::ProccessIterationStatus::CONTINIEFOR;
        }

        for(boost::multiprecision::cpp_int j = 1; j < s; ++j) {
            x = (x * x) % n;
            if (x == 1) {
                clear_values();
                return PrimalityTest::ProccessIterationStatus::NOTPRIME;
            } else if(x == n - 1) {
                if(last_iteration) {
                    clear_values();
                }
                return PrimalityTest::ProccessIterationStatus::CONTINIEFOR;
            }
        }
        clear_values();
        return PrimalityTest::ProccessIterationStatus::NOTPRIME;
    }

    void MillerRabinPrimalityTest::clear_values() {
        t = 0;
        s = 0;
    }
}