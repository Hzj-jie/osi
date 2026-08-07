#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include "../../app_info/assert.hpp"

namespace osi {
namespace math {

class big_uint {
private:
    std::vector<uint32_t> limbs_;

    void remove_trailing_zeros() {
        while (!limbs_.empty() && limbs_.back() == 0) {
            limbs_.pop_back();
        }
    }

public:
    big_uint() = default;

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>>
    explicit big_uint(T val) {
        uint64_t v = static_cast<uint64_t>(val);
        if (v != 0) {
            limbs_.push_back(static_cast<uint32_t>(v & 0xFFFFFFFF));
            uint32_t high = static_cast<uint32_t>(v >> 32);
            if (high != 0) {
                limbs_.push_back(high);
            }
        }
    }

    explicit big_uint(const std::vector<uint8_t>& bytes) {
        replace_by_bytes(bytes);
    }

    explicit big_uint(const std::string& decimal_str) {
        replace_by_string(decimal_str);
    }

    big_uint(const big_uint&) = default;
    big_uint& operator=(const big_uint&) = default;
    big_uint(big_uint&&) noexcept = default;
    big_uint& operator=(big_uint&&) noexcept = default;

    bool is_zero() const { return limbs_.empty(); }
    bool is_one() const { return limbs_.size() == 1 && limbs_[0] == 1; }
    size_t limb_count() const { return limbs_.size(); }
    const std::vector<uint32_t>& limbs() const { return limbs_; }

    void set_zero() { limbs_.clear(); }
    void set_one() {
        limbs_.clear();
        limbs_.push_back(1);
    }

    void replace_by_bytes(const std::vector<uint8_t>& bytes) {
        limbs_.clear();
        if (bytes.empty()) return;
        size_t n = (bytes.size() + 3) / 4;
        limbs_.resize(n, 0);
        for (size_t i = 0; i < bytes.size(); ++i) {
            limbs_[i / 4] |= (static_cast<uint32_t>(bytes[i]) << ((i % 4) * 8));
        }
        remove_trailing_zeros();
    }

    std::vector<uint8_t> as_bytes() const {
        if (limbs_.empty()) return {};
        std::vector<uint8_t> bytes;
        bytes.reserve(limbs_.size() * 4);
        for (uint32_t limb : limbs_) {
            bytes.push_back(static_cast<uint8_t>(limb & 0xFF));
            bytes.push_back(static_cast<uint8_t>((limb >> 8) & 0xFF));
            bytes.push_back(static_cast<uint8_t>((limb >> 16) & 0xFF));
            bytes.push_back(static_cast<uint8_t>((limb >> 24) & 0xFF));
        }
        while (!bytes.empty() && bytes.back() == 0) {
            bytes.pop_back();
        }
        return bytes;
    }

    void replace_by_string(const std::string& str) {
        set_zero();
        for (char c : str) {
            if (c >= '0' && c <= '9') {
                multiply(10);
                add(big_uint(static_cast<uint32_t>(c - '0')));
            }
        }
    }

    std::string str() const {
        if (is_zero()) return "0";
        big_uint temp = *this;
        std::vector<char> digits;
        while (!temp.is_zero()) {
            uint32_t rem = 0;
            temp.divide_uint32(10, rem);
            digits.push_back(static_cast<char>('0' + rem));
        }
        std::string res;
        res.reserve(digits.size());
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            res.push_back(digits[i]);
        }
        return res;
    }

    int compare(const big_uint& that) const {
        if (limbs_.size() != that.limbs_.size()) {
            return limbs_.size() < that.limbs_.size() ? -1 : 1;
        }
        for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
            if (limbs_[i] != that.limbs_[i]) {
                return limbs_[i] < that.limbs_[i] ? -1 : 1;
            }
        }
        return 0;
    }

    bool operator==(const big_uint& that) const { return compare(that) == 0; }
    bool operator!=(const big_uint& that) const { return compare(that) != 0; }
    bool operator<(const big_uint& that) const { return compare(that) < 0; }
    bool operator<=(const big_uint& that) const { return compare(that) <= 0; }
    bool operator>(const big_uint& that) const { return compare(that) > 0; }
    bool operator>=(const big_uint& that) const { return compare(that) >= 0; }

    big_uint& add(const big_uint& that) {
        size_t n = std::max(limbs_.size(), that.limbs_.size());
        limbs_.resize(n, 0);
        uint64_t carry = 0;
        for (size_t i = 0; i < n; ++i) {
            uint64_t sum = carry + limbs_[i];
            if (i < that.limbs_.size()) {
                sum += that.limbs_[i];
            }
            limbs_[i] = static_cast<uint32_t>(sum & 0xFFFFFFFF);
            carry = sum >> 32;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint32_t>(carry));
        }
        return *this;
    }

    big_uint operator+(const big_uint& that) const {
        big_uint res = *this;
        res.add(that);
        return res;
    }

    big_uint& sub(const big_uint& that) {
        assert(*this >= that, "big_uint underflow");
        int64_t borrow = 0;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            int64_t diff = static_cast<int64_t>(limbs_[i]) - borrow;
            if (i < that.limbs_.size()) {
                diff -= that.limbs_[i];
            }
            if (diff < 0) {
                diff += 0x100000000LL;
                borrow = 1;
            } else {
                borrow = 0;
            }
            limbs_[i] = static_cast<uint32_t>(diff);
        }
        remove_trailing_zeros();
        return *this;
    }

    big_uint operator-(const big_uint& that) const {
        big_uint res = *this;
        res.sub(that);
        return res;
    }

    big_uint& multiply(uint32_t factor) {
        if (factor == 0 || is_zero()) {
            set_zero();
            return *this;
        }
        uint64_t carry = 0;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint64_t prod = static_cast<uint64_t>(limbs_[i]) * factor + carry;
            limbs_[i] = static_cast<uint32_t>(prod & 0xFFFFFFFF);
            carry = prod >> 32;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint32_t>(carry));
        }
        return *this;
    }

    big_uint& multiply(const big_uint& that) {
        if (is_zero() || that.is_zero()) {
            set_zero();
            return *this;
        }
        std::vector<uint32_t> res(limbs_.size() + that.limbs_.size(), 0);
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint64_t carry = 0;
            for (size_t j = 0; j < that.limbs_.size(); ++j) {
                uint64_t cur = res[i + j] + static_cast<uint64_t>(limbs_[i]) * that.limbs_[j] + carry;
                res[i + j] = static_cast<uint32_t>(cur & 0xFFFFFFFF);
                carry = cur >> 32;
            }
            res[i + that.limbs_.size()] += static_cast<uint32_t>(carry);
        }
        limbs_ = std::move(res);
        remove_trailing_zeros();
        return *this;
    }

    big_uint operator*(const big_uint& that) const {
        big_uint res = *this;
        res.multiply(that);
        return res;
    }

    big_uint& divide_uint32(uint32_t divisor, uint32_t& remainder) {
        assert(divisor != 0, "Division by zero");
        uint64_t rem = 0;
        for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
            uint64_t cur = (rem << 32) | limbs_[i];
            limbs_[i] = static_cast<uint32_t>(cur / divisor);
            rem = cur % divisor;
        }
        remainder = static_cast<uint32_t>(rem);
        remove_trailing_zeros();
        return *this;
    }

    big_uint divide(const big_uint& divisor, big_uint& remainder) const {
        assert(!divisor.is_zero(), "Division by zero");
        if (*this < divisor) {
            remainder = *this;
            return big_uint(0U);
        }
        big_uint quotient;
        quotient.limbs_.resize(limbs_.size(), 0);
        remainder.set_zero();

        for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
            remainder.shift_limbs_left(1);
            if (!remainder.limbs_.empty() || limbs_[i] != 0) {
                if (remainder.limbs_.empty()) remainder.limbs_.resize(1, 0);
                remainder.limbs_[0] = limbs_[i];
            }
            uint64_t low = 0, high = 0xFFFFFFFFLL, best = 0;
            while (low <= high) {
                uint64_t mid = low + (high - low) / 2;
                big_uint prod = divisor;
                prod.multiply(static_cast<uint32_t>(mid));
                if (prod <= remainder) {
                    best = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }
            quotient.limbs_[i] = static_cast<uint32_t>(best);
            big_uint sub_val = divisor;
            sub_val.multiply(static_cast<uint32_t>(best));
            remainder.sub(sub_val);
        }
        quotient.remove_trailing_zeros();
        return quotient;
    }

    big_uint operator/(const big_uint& that) const {
        big_uint rem;
        return divide(that, rem);
    }

    big_uint operator%(const big_uint& that) const {
        big_uint rem;
        divide(that, rem);
        return rem;
    }

    void shift_limbs_left(size_t shift) {
        if (shift == 0 || is_zero()) return;
        limbs_.insert(limbs_.begin(), shift, 0);
    }

    static big_uint gcd(big_uint a, big_uint b) {
        while (!b.is_zero()) {
            big_uint r = a % b;
            a = std::move(b);
            b = std::move(r);
        }
        return a;
    }
};

} // namespace math
} // namespace osi
