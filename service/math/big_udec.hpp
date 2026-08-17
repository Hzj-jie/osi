#pragma once
#include "big_uint.hpp"

namespace osi {
namespace math {

class big_udec {
private:
    big_uint n_; // Numerator
    big_uint d_; // Denominator
    int32_t fraction_dirty_rate_{0};

    static bool fast_reduce_fraction(big_uint& n, big_uint& d) {
        if (n.is_zero()) {
            d.set_one();
            return true;
        }
        if (n.is_one() || d.is_one()) return true;
        if (n == d) {
            n.set_one();
            d.set_one();
            return true;
        }
        size_t m = std::min(n.trailing_binary_zeros(), d.trailing_binary_zeros());
        if (m > 0) {
            n.shift_right(m);
            d.shift_right(m);
        }
        return false;
    }

    bool increase_fraction_dirty_rate() {
        fraction_dirty_rate_++;
        if (fraction_dirty_rate_ >= 1000000) {
            reduce_fraction();
            return true;
        }
        return false;
    }

public:
    bool fast_reduce_fraction() {
        return fast_reduce_fraction(n_, d_);
    }

    void reduce_fraction() {
        if (fast_reduce_fraction()) {
            fraction_dirty_rate_ = 0;
            return;
        }
        big_uint g = big_uint::gcd(n_, d_);
        if (!g.is_one() && !g.is_zero()) {
            big_uint rem;
            n_ = n_.divide(g, rem);
            d_ = d_.divide(g, rem);
        }
        fraction_dirty_rate_ = 0;
    }

    big_udec() : n_(0U), d_(1U) {}

    explicit big_udec(uint64_t val) : n_(val), d_(1U) {}

    big_udec(big_uint n, big_uint d) : n_(std::move(n)), d_(std::move(d)) {
        assert(!d_.is_zero(), "Denominator cannot be zero");
    }

    static big_udec fraction(uint64_t n, uint64_t d) {
        return big_udec(big_uint(n), big_uint(d));
    }

    static big_udec zero() { return big_udec(0U); }
    static big_udec one() { return big_udec(1U); }

    const big_uint& numerator() {
        reduce_fraction();
        return n_;
    }
    const big_uint& denominator() {
        reduce_fraction();
        return d_;
    }
    const big_uint& numerator() const { return n_; }
    const big_uint& denominator() const { return d_; }

    bool is_zero() const { return n_.is_zero(); }
    bool is_one() const { return n_ == d_; }

    big_udec reciprocal() const {
        assert(!n_.is_zero(), "Division by zero in reciprocal");
        return big_udec(d_, n_);
    }

    big_udec& add(const big_udec& that) {
        if (that.is_zero()) return *this;
        if (is_zero()) {
            n_ = that.n_;
            d_ = that.d_;
            return *this;
        }
        if (d_ == that.d_) {
            n_.add(that.n_);
        } else {
            n_ = that.d_ * n_ + that.n_ * d_;
            d_ = d_ * that.d_;
        }
        increase_fraction_dirty_rate();
        return *this;
    }

    big_udec operator+(const big_udec& that) const {
        big_udec res = *this;
        res.add(that);
        return res;
    }

    big_udec& sub(const big_udec& that) {
        assert(*this >= that, "big_udec underflow");
        if (that.is_zero()) return *this;
        if (d_ == that.d_) {
            n_.sub(that.n_);
        } else {
            n_ = that.d_ * n_ - that.n_ * d_;
            d_ = d_ * that.d_;
        }
        increase_fraction_dirty_rate();
        return *this;
    }

    big_udec operator-(const big_udec& that) const {
        big_udec res = *this;
        res.sub(that);
        return res;
    }

    big_udec& multiply(const big_udec& that) {
        if (that.is_zero()) {
            n_.set_zero();
            d_.set_one();
            return *this;
        }
        if (is_zero()) return *this;

        big_uint n1 = n_;
        big_uint n2 = that.n_;
        big_uint d1 = d_;
        big_uint d2 = that.d_;
        fast_reduce_fraction(n1, d2);
        fast_reduce_fraction(n2, d1);

        n_ = n1 * n2;
        d_ = d1 * d2;
        increase_fraction_dirty_rate();
        return *this;
    }

    big_udec& power_2() {
        return multiply(*this);
    }

    big_udec operator*(const big_udec& that) const {
        big_udec res = *this;
        res.multiply(that);
        return res;
    }

    big_udec& divide(const big_udec& that) {
        assert(!that.is_zero(), "Division by zero");
        if (is_zero()) return *this;

        big_uint n1 = n_;
        big_uint n2 = that.n_;
        big_uint d1 = d_;
        big_uint d2 = that.d_;
        fast_reduce_fraction(n1, n2);
        fast_reduce_fraction(d1, d2);

        n_ = n1 * d2;
        d_ = d1 * n2;
        increase_fraction_dirty_rate();
        return *this;
    }

    big_udec operator/(const big_udec& that) const {
        big_udec res = *this;
        res.divide(that);
        return res;
    }

    big_udec& power(uint32_t exp) {
        if (exp == 0) {
            n_.set_one();
            d_.set_one();
            return *this;
        }
        if (is_zero() || is_one() || exp == 1) {
            return *this;
        }
        n_.power(exp);
        d_.power(exp);
        return *this;
    }

    int compare(const big_udec& that) const {
        big_uint lhs = n_ * that.d_;
        big_uint rhs = that.n_ * d_;
        return lhs.compare(rhs);
    }

    bool operator==(const big_udec& that) const { return compare(that) == 0; }
    bool operator!=(const big_udec& that) const { return compare(that) != 0; }
    bool operator<(const big_udec& that) const { return compare(that) < 0; }
    bool operator<=(const big_udec& that) const { return compare(that) <= 0; }
    bool operator>(const big_udec& that) const { return compare(that) > 0; }
    bool operator>=(const big_udec& that) const { return compare(that) >= 0; }

    std::string fractional_str() const {
        return n_.str() + " / " + d_.str();
    }

    void stream_digits(std::ostream& os, size_t max_decimal_places) const {
        if (is_zero()) {
            os << "0";
            return;
        }
        big_uint rem;
        big_uint integer_part = n_.divide(d_, rem);
        os << integer_part.str();
        if (rem.is_zero() || max_decimal_places == 0) return;

        os << '.';
        big_uint cur_rem = rem;
        for (size_t i = 0; i < max_decimal_places && !cur_rem.is_zero(); ++i) {
            cur_rem.multiply(10U);
            big_uint digit = cur_rem.divide(d_, rem);
            os << digit.str();
            cur_rem = rem;
        }
    }

    static bool parse_fraction(const std::string& str, big_udec& out) {
        size_t idx = str.find('/');
        if (idx == std::string::npos) {
            big_uint n(str);
            out = big_udec(n, big_uint(1U));
            return true;
        }
        std::string n_str = str.substr(0, idx);
        std::string d_str = str.substr(idx + 1);

        auto trim = [](std::string& s) {
            size_t p1 = s.find_first_not_of(" \t\r\n");
            if (p1 == std::string::npos) { s.clear(); return; }
            size_t p2 = s.find_last_not_of(" \t\r\n");
            s = s.substr(p1, p2 - p1 + 1);
        };
        trim(n_str);
        trim(d_str);

        big_uint n(n_str);
        big_uint d(d_str);
        if (d.is_zero()) return false;
        out = big_udec(n, d);
        return true;
    }

    std::string str(size_t max_decimal_places = 10) const {
        big_uint rem;
        big_uint integer_part = n_.divide(d_, rem);
        std::string res = integer_part.str();
        if (rem.is_zero() || max_decimal_places == 0) return res;

        res.push_back('.');
        big_uint cur_rem = rem;
        for (size_t i = 0; i < max_decimal_places && !cur_rem.is_zero(); ++i) {
            cur_rem.multiply(10U);
            big_uint digit = cur_rem.divide(d_, rem);
            res += digit.str();
            cur_rem = rem;
        }
        return res;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_udec& v) {
        return os << v.str();
    }
};

} // namespace math
} // namespace osi
