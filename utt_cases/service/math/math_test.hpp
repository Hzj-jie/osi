#pragma once
#include "../../../service/math/big_uint.hpp"
#include "../../../service/math/big_udec.hpp"
#include "../../../service/math/big_int.hpp"
#include "../../../service/math/big_dec.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace math_test {

class big_uint_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        big_uint a(123456789ULL);
        big_uint b(987654321ULL);

        utt_assert.equal(a.str(), std::string("123456789"));
        utt_assert.equal(b.str(), std::string("987654321"));

        big_uint c = a + b;
        utt_assert.equal(c.str(), std::string("1111111110"));

        big_uint d = b - a;
        utt_assert.equal(d.str(), std::string("864197532"));

        big_uint prod = a * b;
        utt_assert.equal(prod.str(), std::string("121932631112635269"));

        big_uint rem;
        big_uint q = prod.divide(a, rem);
        utt_assert.equal(q.str(), std::string("987654321"));
        utt_assert.is_true(rem.is_zero());

        return true;
    }

    DEFINE_CASE(big_uint_test);
};

REGISTER_CASE(big_uint_test);

class big_udec_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        big_udec half(big_uint(1U), big_uint(2U));
        big_udec quarter(big_uint(1U), big_uint(4U));

        big_udec sum = half + quarter;
        utt_assert.equal(sum.numerator().str(), std::string("3"));
        utt_assert.equal(sum.denominator().str(), std::string("4"));
        utt_assert.equal(sum.str(2), std::string("0.75"));

        big_udec prod = half * quarter;
        utt_assert.equal(prod.numerator().str(), std::string("1"));
        utt_assert.equal(prod.denominator().str(), std::string("8"));
        utt_assert.equal(prod.str(3), std::string("0.125"));

        return true;
    }

    DEFINE_CASE(big_udec_test);
};

REGISTER_CASE(big_udec_test);

class big_int_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        big_int a(100LL);
        big_int b(-45LL);

        big_int sum = a + b;
        utt_assert.equal(sum.str(), std::string("55"));

        big_int diff = b - a;
        utt_assert.equal(diff.str(), std::string("-145"));

        big_int prod = a * b;
        utt_assert.equal(prod.str(), std::string("-4500"));

        return true;
    }

    DEFINE_CASE(big_int_test);
};

REGISTER_CASE(big_int_test);

class big_dec_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        big_dec a(big_udec(big_uint(3U), big_uint(4U)), false); // +0.75
        big_dec b(big_udec(big_uint(1U), big_uint(2U)), true);  // -0.50

        big_dec sum = a + b;
        utt_assert.equal(sum.str(2), std::string("0.25"));

        big_dec prod = a * b;
        utt_assert.equal(prod.str(3), std::string("-0.375"));

        return true;
    }

    DEFINE_CASE(big_dec_test);
};

REGISTER_CASE(big_dec_test);

class big_udec_e_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        // calculate_e_power_1000
        {
            big_udec s(1U);
            s.add(big_udec(big_uint(1U), big_uint(1000U)));
            s.power(1000);
            utt_assert.equal(s.str(64), std::string("2.7169239322358924573830881219475771889643150188365728037223547748"));
        }

        // calculate_e_factorial_100
        {
            big_udec s(1U);
            big_udec c(1U);
            for (uint32_t i = 1; i <= 100; ++i) {
                c.divide(big_udec(i));
                s.add(c);
            }
            utt_assert.equal(s.str(64), std::string("2.7182818284590452353602874713526624977572470936999595749669676277"));
        }

        return true;
    }

    DEFINE_CASE(big_udec_e_test);
};

REGISTER_CASE(big_udec_e_test);

class big_udec_pi_test : public icase {
public:
    bool run() override {
        using namespace osi::math;

        // calculate_pi_sequence_of_numbers_257
        {
            big_udec s(0U);
            for (uint32_t i = 1; i <= 257; i += 2) {
                big_udec c(big_uint(4U), big_uint(i));
                if (((i >> 1) & 1) == 0) {
                    s.add(c);
                } else {
                    s.sub(c);
                }
            }
            utt_assert.equal(s.str(64), std::string("3.1493444751246198849245850855700337265726567226444297158636807717"));
        }

        // calculate_pi_nilakantha_20002
        {
            big_udec s(big_uint(3U), big_uint(1U));
            for (uint32_t i = 2; i <= 20002; i += 2) {
                big_uint d(i);
                d.multiply(i + 1);
                d.multiply(i + 2);
                big_udec c(big_uint(4U), d);
                if (((i >> 1) & 1) == 1) {
                    s.add(c);
                } else {
                    s.sub(c);
                }
            }
            utt_assert.equal(s.str(64), std::string("3.1415926535900430885195015124988771459935110088383081739079144538"));
        }

        // calculate_pi_arctangent_500
        {
            big_udec s(big_uint(2U), big_uint(1U));
            big_udec c(big_uint(2U), big_uint(3U));
            for (uint32_t i = 2; i <= 500; ++i) {
                s.add(c);
                c.multiply(big_udec(big_uint(i), big_uint(2 * i + 1)));
            }
            std::string pi_prefix = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481";
            utt_assert.equal(s.str(150), pi_prefix.substr(0, 152));
        }

        return true;
    }

    DEFINE_CASE(big_udec_pi_test);
};

REGISTER_CASE(big_udec_pi_test);

} // namespace math_test
