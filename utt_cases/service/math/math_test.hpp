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

} // namespace math_test
