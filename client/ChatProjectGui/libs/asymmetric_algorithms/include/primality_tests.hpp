#pragma once 

#include <iostream>
#include <cmath>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>

#include "symbol_service.hpp"

namespace asymmetric_algorithms {
    enum class PrimalityTestEnum {
        FermatPrimalityTest,
        SolovayStrassenPrimalityTest,
        MillerRabinPrimalityTest,
    };
    class InterfacePrimalityTest {
    public:
        virtual double check_primality(const boost::multiprecision::cpp_int& x, double probability) = 0;
    };

    class PrimalityTest : public InterfacePrimalityTest {
    public:
        double check_primality(const boost::multiprecision::cpp_int& x, double probability) final;

    protected:
        std::vector<boost::multiprecision::cpp_int> primality_witnesses;
        enum class ProccessIterationStatus {
            NOTPRIME,
            MAYBEPRIME,
            CONTINIEFOR,
        };
        virtual ProccessIterationStatus proccess_iteration(const boost::multiprecision::cpp_int& n, const boost::multiprecision::cpp_int& a) = 0;
        virtual size_t calculate_k(double probability);
        virtual double calculate_res_probability(size_t k);
    private:
        boost::random::random_device rng;
    };

    class FermatPrimalityTest : public PrimalityTest {
    protected:
        ProccessIterationStatus proccess_iteration(const boost::multiprecision::cpp_int& n, const boost::multiprecision::cpp_int& a) override;
    };

    class SolovayStrassenPrimalityTest : public PrimalityTest {
    protected:
        ProccessIterationStatus proccess_iteration(const boost::multiprecision::cpp_int& n, const boost::multiprecision::cpp_int& a) override;
    };

    class MillerRabinPrimalityTest : public PrimalityTest {
    private:
        void clear_values();
        boost::multiprecision::cpp_int s, t;
    protected:
        ProccessIterationStatus proccess_iteration(const boost::multiprecision::cpp_int& n, const boost::multiprecision::cpp_int& a) override;
        size_t calculate_k(double probability) override;
        double calculate_res_probability(size_t k) override;
    };

}