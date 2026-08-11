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
        if (that.is_zero()) return *this;
        if (is_zero()) {
            limbs_ = that.limbs_;
            return *this;
        }
        size_t common = std::min(limbs_.size(), that.limbs_.size());
        if (limbs_.size() < that.limbs_.size()) {
            limbs_.resize(that.limbs_.size(), 0);
        }
        uint64_t carry = 0;
        size_t i = 0;
        for (; i < common; ++i) {
            uint64_t sum = static_cast<uint64_t>(limbs_[i]) + that.limbs_[i] + carry;
            limbs_[i] = static_cast<uint32_t>(sum & 0xFFFFFFFF);
            carry = sum >> 32;
        }
        for (; carry > 0 && i < limbs_.size(); ++i) {
            uint64_t sum = static_cast<uint64_t>(limbs_[i]) + carry;
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
        uint64_t borrow = 0;
        size_t common = that.limbs_.size();
        size_t i = 0;
        for (; i < common; ++i) {
            uint64_t diff = static_cast<uint64_t>(limbs_[i]) - that.limbs_[i] - borrow;
            limbs_[i] = static_cast<uint32_t>(diff & 0xFFFFFFFF);
            borrow = (diff >> 63);
        }
        for (; borrow > 0 && i < limbs_.size(); ++i) {
            uint64_t diff = static_cast<uint64_t>(limbs_[i]) - borrow;
            limbs_[i] = static_cast<uint32_t>(diff & 0xFFFFFFFF);
            borrow = (diff >> 63);
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
        if (factor == 1) return *this;
        if ((factor & (factor - 1)) == 0) {
            size_t bits = 0;
            uint32_t temp = factor;
            while (temp > 1) { temp >>= 1; bits++; }
            shift_left_bits(bits);
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
        remove_trailing_zeros();
        return *this;
    }

    big_uint& multiply(const big_uint& that) {
        if (is_zero() || that.is_zero()) {
            set_zero();
            return *this;
        }
        if (that.limbs_.size() == 1) {
            return multiply(that.limbs_[0]);
        }
        if (limbs_.size() == 1) {
            uint32_t factor = limbs_[0];
            limbs_ = that.limbs_;
            return multiply(factor);
        }
        std::vector<uint32_t> res(limbs_.size() + that.limbs_.size(), 0);
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint64_t a_limb = limbs_[i];
            if (a_limb == 0) continue;
            uint64_t carry = 0;
            for (size_t j = 0; j < that.limbs_.size(); ++j) {
                uint64_t cur = res[i + j] + a_limb * that.limbs_[j] + carry;
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
        if (divisor == 1) {
            remainder = 0;
            return *this;
        }
        if ((divisor & (divisor - 1)) == 0) {
            size_t bits = 0;
            uint32_t temp = divisor;
            while (temp > 1) { temp >>= 1; bits++; }
            remainder = limbs_.empty() ? 0 : (limbs_[0] & (divisor - 1));
            shift_right(bits);
            return *this;
        }
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

    size_t trailing_binary_zeros() const {
        if (is_zero()) return 0;
        size_t count = 0;
        for (uint32_t limb : limbs_) {
            if (limb == 0) {
                count += 32;
            } else {
                count += static_cast<size_t>(__builtin_ctz(limb));
                break;
            }
        }
        return count;
    }

    void shift_right(size_t bits) {
        if (bits == 0 || is_zero()) return;
        size_t limb_shift = bits / 32;
        size_t bit_shift = bits % 32;
        if (limb_shift >= limbs_.size()) {
            set_zero();
            return;
        }
        limbs_.erase(limbs_.begin(), limbs_.begin() + limb_shift);
        if (bit_shift > 0) {
            uint64_t carry = 0;
            for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
                uint64_t cur = (carry << 32) | limbs_[i];
                limbs_[i] = static_cast<uint32_t>(cur >> bit_shift);
                carry = cur & ((1ULL << bit_shift) - 1);
            }
        }
        remove_trailing_zeros();
    }

    void shift_left_1_bit() {
        uint32_t carry = 0;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint64_t v = (static_cast<uint64_t>(limbs_[i]) << 1) | carry;
            limbs_[i] = static_cast<uint32_t>(v & 0xFFFFFFFF);
            carry = static_cast<uint32_t>(v >> 32);
        }
        if (carry > 0) {
            limbs_.push_back(carry);
        }
    }

    void set_bit(size_t bit_idx) {
        size_t limb_idx = bit_idx / 32;
        if (limb_idx >= limbs_.size()) {
            limbs_.resize(limb_idx + 1, 0);
        }
        limbs_[limb_idx] |= (1U << (bit_idx % 32));
    }

    big_uint divide(const big_uint& divisor, big_uint& remainder) const {
        assert(!divisor.is_zero(), "Division by zero");
        if (*this < divisor) {
            remainder = *this;
            return big_uint(0U);
        }
        if (divisor.limbs_.size() == 1) {
            uint32_t rem = 0;
            big_uint quotient = *this;
            quotient.divide_uint32(divisor.limbs_[0], rem);
            remainder = big_uint(rem);
            return quotient;
        }

        remainder.set_zero();
        big_uint quotient;
        quotient.limbs_.resize(limbs_.size(), 0);

        int total_bits = static_cast<int>(limbs_.size()) * 32;
        for (int i = total_bits - 1; i >= 0; --i) {
            remainder.shift_left_1_bit();
            uint32_t limb = limbs_[i / 32];
            if ((limb >> (i % 32)) & 1) {
                if (remainder.limbs_.empty()) remainder.limbs_.resize(1, 0);
                remainder.limbs_[0] |= 1U;
            }
            if (remainder >= divisor) {
                remainder.sub(divisor);
                quotient.limbs_[i / 32] |= (1U << (i % 32));
            }
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

    bool fit_uint64() const { return limbs_.size() <= 2; }
    uint64_t as_uint64() const {
        if (limbs_.empty()) return 0;
        if (limbs_.size() == 1) return limbs_[0];
        return (static_cast<uint64_t>(limbs_[1]) << 32) | limbs_[0];
    }

    void shift_left_bits(size_t bits) {
        if (bits == 0 || is_zero()) return;
        size_t limb_shift = bits / 32;
        size_t bit_shift = bits % 32;

        if (bit_shift > 0) {
            uint64_t carry = 0;
            for (size_t i = 0; i < limbs_.size(); ++i) {
                uint64_t v = (static_cast<uint64_t>(limbs_[i]) << bit_shift) | carry;
                limbs_[i] = static_cast<uint32_t>(v & 0xFFFFFFFF);
                carry = v >> 32;
            }
            if (carry > 0) {
                limbs_.push_back(static_cast<uint32_t>(carry));
            }
        }
        if (limb_shift > 0) {
            limbs_.insert(limbs_.begin(), limb_shift, 0);
        }
    }

    static big_uint gcd(big_uint a, big_uint b) {
        if (a.is_zero() || b.is_zero()) return big_uint(0U);
        if (a.is_one() || b.is_one()) return big_uint(1U);
        if (a.fit_uint64() && b.fit_uint64()) {
            return big_uint(std::gcd(a.as_uint64(), b.as_uint64()));
        }
        if (a == b) return a;

        size_t shift = std::min(a.trailing_binary_zeros(), b.trailing_binary_zeros());
        a.shift_right(a.trailing_binary_zeros());
        b.shift_right(b.trailing_binary_zeros());

        while (!a.is_zero() && !b.is_zero()) {
            if (a.fit_uint64() && b.fit_uint64()) {
                uint64_t g = std::gcd(a.as_uint64(), b.as_uint64());
                a = big_uint(g);
                break;
            }

            int cmp = a.compare(b);
            if (cmp == 0) break;
            if (cmp < 0) std::swap(a, b);

            if (a.limb_count() >= b.limb_count() + 2 || b.limb_count() <= 2) {
                big_uint rem;
                a.divide(b, rem);
                if (rem.is_zero()) {
                    a = b;
                    break;
                }
                a = std::move(b);
                b = std::move(rem);
            } else {
                a.sub(b);
            }
            a.shift_right(a.trailing_binary_zeros());
            b.shift_right(b.trailing_binary_zeros());
        }

        if (shift > 0) {
            a.shift_left_bits(shift);
        }
        return a;
    }
};

} // namespace math
} // namespace osi
