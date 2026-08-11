#pragma once
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

    bool is_zero() const { return abs_val_.is_zero(); }
    bool is_negative() const { return negative_ && !abs_val_.is_zero(); }
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

    std::string str(size_t max_decimal_places = 10) const {
        if (is_zero()) return "0";
        return (is_negative() ? "-" : "") + abs_val_.str(max_decimal_places);
    }
};

} // namespace math
} // namespace osi
