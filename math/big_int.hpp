#pragma once
#include <type_traits>
#include "big_uint.hpp"

namespace osi {
namespace math {

class big_int {
private:
    bool negative_{false};
    big_uint abs_val_;

public:
    big_int() = default;

    template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, int>::type = 0>
    explicit big_int(T val) {
        if (val < 0) {
            negative_ = true;
            abs_val_ = big_uint(static_cast<uint64_t>(-static_cast<int64_t>(val)));
        } else {
            negative_ = false;
            abs_val_ = big_uint(static_cast<uint64_t>(val));
        }
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, int>::type = 0>
    explicit big_int(T val) : negative_(false), abs_val_(static_cast<uint64_t>(val)) {}

    explicit big_int(const std::string& str) {
        if (str.empty()) {
            negative_ = false;
            abs_val_.set_zero();
            return;
        }
        size_t start = 0;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
            start++;
        }
        if (start < str.size() && str[start] == '-') {
            negative_ = true;
            start++;
        } else if (start < str.size() && str[start] == '+') {
            negative_ = false;
            start++;
        } else {
            negative_ = false;
        }
        abs_val_ = big_uint(str.substr(start));
        if (abs_val_.is_zero()) negative_ = false;
    }

    explicit big_int(big_uint val, bool negative = false)
        : negative_(negative && !val.is_zero()), abs_val_(std::move(val)) {}

    bool is_zero() const { return abs_val_.is_zero(); }
    bool is_negative() const { return negative_ && !abs_val_.is_zero(); }
    bool is_positive() const { return !negative_ && !abs_val_.is_zero(); }
    const big_uint& abs() const { return abs_val_; }

    big_int operator-() const {
        big_int res = *this;
        if (!res.is_zero()) {
            res.negative_ = !res.negative_;
        }
        return res;
    }

    big_int& add(const big_int& that) {
        if (negative_ == that.negative_) {
            abs_val_.add(that.abs_val_);
        } else {
            if (abs_val_ >= that.abs_val_) {
                abs_val_.sub(that.abs_val_);
            } else {
                big_uint diff = that.abs_val_ - abs_val_;
                abs_val_ = std::move(diff);
                negative_ = that.negative_;
            }
        }
        if (abs_val_.is_zero()) negative_ = false;
        return *this;
    }

    big_int operator+(const big_int& that) const {
        big_int res = *this;
        res.add(that);
        return res;
    }

    big_int& sub(const big_int& that) {
        return add(-that);
    }

    big_int operator-(const big_int& that) const {
        big_int res = *this;
        res.sub(that);
        return res;
    }

    big_int& multiply(const big_int& that) {
        negative_ = (negative_ != that.negative_);
        abs_val_.multiply(that.abs_val_);
        if (abs_val_.is_zero()) negative_ = false;
        return *this;
    }

    big_int operator*(const big_int& that) const {
        big_int res = *this;
        res.multiply(that);
        return res;
    }

    big_int divide(const big_int& that, big_int& remainder) const {
        assert(!that.is_zero(), "Division by zero");
        big_uint r;
        big_uint q = abs_val_.divide(that.abs_val_, r);
        bool q_neg = (negative_ != that.negative_);
        bool r_neg = negative_;
        remainder = big_int(r, r_neg);
        return big_int(q, q_neg);
    }

    big_int& divide(const big_int& that) {
        big_int rem;
        *this = divide(that, rem);
        return *this;
    }

    big_int operator/(const big_int& that) const {
        big_int rem;
        return divide(that, rem);
    }

    big_int operator%(const big_int& that) const {
        big_int rem;
        divide(that, rem);
        return rem;
    }

    big_int& power(uint32_t exp) {
        if (exp == 0) {
            negative_ = false;
            abs_val_.set_one();
            return *this;
        }
        if (is_zero()) return *this;
        abs_val_.power(exp);
        if ((exp & 1) == 0) {
            negative_ = false;
        }
        return *this;
    }

    int compare(const big_int& that) const {
        if (is_negative() != that.is_negative()) {
            return is_negative() ? -1 : 1;
        }
        int cmp = abs_val_.compare(that.abs_val_);
        return is_negative() ? -cmp : cmp;
    }

    bool operator==(const big_int& that) const { return compare(that) == 0; }
    bool operator!=(const big_int& that) const { return compare(that) != 0; }
    bool operator<(const big_int& that) const { return compare(that) < 0; }
    bool operator<=(const big_int& that) const { return compare(that) <= 0; }
    bool operator>(const big_int& that) const { return compare(that) > 0; }
    bool operator>=(const big_int& that) const { return compare(that) >= 0; }

    int64_t as_int64() const {
        int64_t val = static_cast<int64_t>(abs_val_.as_uint64());
        return negative_ ? -val : val;
    }

    std::string str() const {
        if (is_zero()) return "0";
        return (is_negative() ? "-" : "") + abs_val_.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const big_int& v) {
        return os << v.str();
    }
};

} // namespace math
} // namespace osi
