#pragma once
#include "big_uint.hpp"

namespace osi {
namespace math {

class big_int {
private:
    bool negative_{false};
    big_uint abs_val_;

public:
    big_int() = default;

    explicit big_int(int64_t val) {
        if (val < 0) {
            negative_ = true;
            abs_val_ = big_uint(static_cast<uint64_t>(-val));
        } else {
            negative_ = false;
            abs_val_ = big_uint(static_cast<uint64_t>(val));
        }
    }

    explicit big_int(big_uint val, bool negative = false)
        : negative_(negative && !val.is_zero()), abs_val_(std::move(val)) {}

    bool is_zero() const { return abs_val_.is_zero(); }
    bool is_negative() const { return negative_ && !abs_val_.is_zero(); }
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

    std::string str() const {
        if (is_zero()) return "0";
        return (is_negative() ? "-" : "") + abs_val_.str();
    }
};

} // namespace math
} // namespace osi
