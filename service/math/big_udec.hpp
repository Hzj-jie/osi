#pragma once
#include "big_uint.hpp"

namespace osi {
namespace math {

class big_udec {
private:
    big_uint n_; // Numerator
    big_uint d_; // Denominator

public:
    void reduce_fraction() {
        if (n_.is_zero()) {
            d_.set_one();
            return;
        }
        if (n_.is_one() || d_.is_one()) return;
        if (n_ == d_) {
            n_.set_one();
            d_.set_one();
            return;
        }
        size_t tz = std::min(n_.trailing_binary_zeros(), d_.trailing_binary_zeros());
        if (tz > 0) {
            n_.shift_right(tz);
            d_.shift_right(tz);
        }

        if (n_.is_one() || d_.is_one()) return;

        big_uint g = big_uint::gcd(n_, d_);
        if (!g.is_one() && !g.is_zero()) {
            big_uint rem;
            n_ = n_.divide(g, rem);
            d_ = d_.divide(g, rem);
        }
    }

    big_udec() : n_(0U), d_(1U) {}

    explicit big_udec(uint64_t val) : n_(val), d_(1U) {}

    big_udec(big_uint n, big_uint d) : n_(std::move(n)), d_(std::move(d)) {
        assert(!d_.is_zero(), "Denominator cannot be zero");
        reduce_fraction();
    }

    const big_uint& numerator() const { return n_; }
    const big_uint& denominator() const { return d_; }

    bool is_zero() const { return n_.is_zero(); }
    bool is_one() const { return n_ == d_; }

    big_udec& add(const big_udec& that) {
        if (d_ == that.d_) {
            n_.add(that.n_);
        } else {
            n_ = n_ * that.d_ + that.n_ * d_;
            d_ = d_ * that.d_;
        }
        reduce_fraction();
        return *this;
    }

    big_udec operator+(const big_udec& that) const {
        big_udec res = *this;
        res.add(that);
        return res;
    }

    big_udec& sub(const big_udec& that) {
        assert(*this >= that, "big_udec underflow");
        if (d_ == that.d_) {
            n_.sub(that.n_);
        } else {
            n_ = n_ * that.d_ - that.n_ * d_;
            d_ = d_ * that.d_;
        }
        reduce_fraction();
        return *this;
    }

    big_udec operator-(const big_udec& that) const {
        big_udec res = *this;
        res.sub(that);
        return res;
    }

    big_udec& multiply(const big_udec& that) {
        n_ = n_ * that.n_;
        d_ = d_ * that.d_;
        reduce_fraction();
        return *this;
    }

    big_udec operator*(const big_udec& that) const {
        big_udec res = *this;
        res.multiply(that);
        return res;
    }

    big_udec& divide(const big_udec& that) {
        assert(!that.is_zero(), "Division by zero");
        n_ = n_ * that.d_;
        d_ = d_ * that.n_;
        reduce_fraction();
        return *this;
    }

    big_udec operator/(const big_udec& that) const {
        big_udec res = *this;
        res.divide(that);
        return res;
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
            cur_rem.multiply(10);
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
            cur_rem.multiply(10);
            big_uint digit = cur_rem.divide(d_, rem);
            res += digit.str();
            cur_rem = rem;
        }
        return res;
    }
};

} // namespace math
} // namespace osi
