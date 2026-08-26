#pragma once
#include <type_traits>
#include "big_udec.hpp"

namespace osi {
namespace math {

class big_dec {
private:
    bool negative_{false};
    big_udec abs_val_;

public:
    big_dec() = default;

    explicit big_dec(big_udec val, bool negative = false)
        : negative_(negative && !val.is_zero()), abs_val_(std::move(val)) {}

    template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, int>::type = 0>
    explicit big_dec(T val) {
        if (val < 0) {
            negative_ = true;
            abs_val_ = big_udec(static_cast<uint64_t>(-static_cast<int64_t>(val)));
        } else {
            negative_ = false;
            abs_val_ = big_udec(static_cast<uint64_t>(val));
        }
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, int>::type = 0>
    explicit big_dec(T val) : negative_(false), abs_val_(static_cast<uint64_t>(val)) {}

    bool is_zero() const { return abs_val_.is_zero(); }
    bool is_negative() const { return negative_ && !abs_val_.is_zero(); }
    bool is_positive() const { return !negative_ && !abs_val_.is_zero(); }
    const big_udec& abs() const { return abs_val_; }

    big_dec operator-() const {
        big_dec res = *this;
        if (!res.is_zero()) {
            res.negative_ = !res.negative_;
        }
        return res;
    }

    big_dec& add(const big_dec& that) {
        if (negative_ == that.negative_) {
            abs_val_.add(that.abs_val_);
        } else {
            if (abs_val_ >= that.abs_val_) {
                abs_val_.sub(that.abs_val_);
            } else {
                big_udec diff = that.abs_val_ - abs_val_;
                abs_val_ = std::move(diff);
                negative_ = that.negative_;
            }
        }
        if (abs_val_.is_zero()) negative_ = false;
        return *this;
    }

    big_dec operator+(const big_dec& that) const {
        big_dec res = *this;
        res.add(that);
        return res;
    }

    big_dec& sub(const big_dec& that) {
        return add(-that);
    }

    big_dec operator-(const big_dec& that) const {
        big_dec res = *this;
        res.sub(that);
        return res;
    }

    big_dec& multiply(const big_dec& that) {
        negative_ = (negative_ != that.negative_);
        abs_val_.multiply(that.abs_val_);
        if (abs_val_.is_zero()) negative_ = false;
        return *this;
    }

    big_dec operator*(const big_dec& that) const {
        big_dec res = *this;
        res.multiply(that);
        return res;
    }

    big_dec& divide(const big_dec& that) {
        assert(!that.is_zero(), "Division by zero");
        negative_ = (negative_ != that.negative_);
        abs_val_.divide(that.abs_val_);
        if (abs_val_.is_zero()) negative_ = false;
        return *this;
    }

    big_dec operator/(const big_dec& that) const {
        big_dec res = *this;
        res.divide(that);
        return res;
    }

    big_dec& power(uint32_t exp) {
        if (exp == 0) {
            negative_ = false;
            abs_val_ = big_udec::one();
            return *this;
        }
        if (is_zero()) return *this;
        abs_val_.power(exp);
        if ((exp & 1) == 0) {
            negative_ = false;
        }
        return *this;
    }

    int compare(const big_dec& that) const {
        if (is_negative() != that.is_negative()) {
            return is_negative() ? -1 : 1;
        }
        int cmp = abs_val_.compare(that.abs_val_);
        return is_negative() ? -cmp : cmp;
    }

    bool operator==(const big_dec& that) const { return compare(that) == 0; }
    bool operator!=(const big_dec& that) const { return compare(that) != 0; }
    bool operator<(const big_dec& that) const { return compare(that) < 0; }
    bool operator<=(const big_dec& that) const { return compare(that) <= 0; }
    bool operator>(const big_dec& that) const { return compare(that) > 0; }
    bool operator>=(const big_dec& that) const { return compare(that) >= 0; }

    const big_udec& unsigned_ref() const { return abs_val_; }

    std::string fractional_str() const {
        if (is_zero()) return "0 / 1";
        return (is_negative() ? "-" : "") + abs_val_.fractional_str();
    }

    static bool parse_fraction(const std::string& str, big_dec& out) {
        if (str.empty()) return false;
        size_t start = 0;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
            start++;
        }
        bool negative = false;
        if (start < str.size() && str[start] == '-') {
            negative = true;
            start++;
        } else if (start < str.size() && str[start] == '+') {
            start++;
        }
        big_udec udec;
        if (!big_udec::parse_fraction(str.substr(start), udec)) return false;
        out = big_dec(udec, negative);
        return true;
    }

    std::string str(size_t max_decimal_places = 10) const {
        if (is_zero()) return "0";
        return (is_negative() ? "-" : "") + abs_val_.str(max_decimal_places);
    }

    friend std::ostream& operator<<(std::ostream& os, const big_dec& v) {
        return os << v.str();
    }
};

} // namespace math
} // namespace osi
