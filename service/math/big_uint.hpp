#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <iostream>
#include "../../app_info/assert.hpp"

namespace osi {
namespace math {

class big_uint {
private:
    std::vector<uint32_t> limbs_;

    void remove_trailing_zeros() {
        while (limbs_.size() > 1 && limbs_.back() == 0) {
            limbs_.pop_back();
        }
    }

public:
    big_uint() : limbs_{0} {}

    explicit big_uint(uint64_t val) {
        if (val == 0) {
            limbs_ = {0};
        } else {
            limbs_.push_back(static_cast<uint32_t>(val & 0xFFFFFFFF));
            uint32_t high = static_cast<uint32_t>(val >> 32);
            if (high > 0) {
                limbs_.push_back(high);
            }
        }
    }

    explicit big_uint(const std::string& str) {
        set_zero();
        if (str.empty()) return;
        size_t start = 0;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
            start++;
        }
        for (size_t i = start; i < str.size(); ++i) {
            char c = str[i];
            if (c < '0' || c > '9') break;
            multiply(10U);
            add(big_uint(static_cast<uint64_t>(c - '0')));
        }
    }

    explicit big_uint(const std::vector<uint8_t>& bytes) {
        set_zero();
        for (int i = static_cast<int>(bytes.size()) - 1; i >= 0; --i) {
            multiply(256U);
            add(big_uint(static_cast<uint64_t>(bytes[i])));
        }
    }

    void set_zero() {
        limbs_ = {0};
    }

    void set_one() {
        limbs_ = {1};
    }

    bool is_zero() const {
        return limbs_.size() == 1 && limbs_[0] == 0;
    }

    bool is_one() const {
        return limbs_.size() == 1 && limbs_[0] == 1;
    }

    bool is_zero_or_one() const {
        return is_zero() || is_one();
    }

    bool is_even() const {
        return limbs_.empty() || (limbs_[0] & 1) == 0;
    }

    bool is_odd() const {
        return !is_even();
    }

    size_t limb_count() const {
        return limbs_.size();
    }

    size_t uint32_size() const {
        return limbs_.size();
    }

    size_t bit_count() const {
        if (is_zero()) return 0;
        size_t full = (limbs_.size() - 1) * 32;
        uint32_t top = limbs_.back();
        size_t top_bits = top == 0 ? 0 : static_cast<size_t>(32 - __builtin_clz(top));
        return full + top_bits;
    }

    bool fit_uint32() const {
        return limbs_.size() == 1;
    }

    uint32_t as_uint32() const {
        return limbs_.empty() ? 0 : limbs_[0];
    }

    bool fit_uint64() const {
        return limbs_.size() <= 2;
    }

    uint64_t as_uint64() const {
        if (limbs_.empty()) return 0;
        if (limbs_.size() == 1) return limbs_[0];
        return (static_cast<uint64_t>(limbs_[1]) << 32) | limbs_[0];
    }

    uint64_t highest_uint64() const {
        if (limbs_.empty()) return 0;
        if (limbs_.size() == 1) return limbs_[0];
        return (static_cast<uint64_t>(limbs_.back()) << 32) | limbs_[limbs_.size() - 2];
    }

    uint32_t highest_uint32() const {
        if (limbs_.empty()) return 0;
        return limbs_.back();
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

    int compare_offset(const big_uint& that, size_t offset) const {
        size_t that_size = that.limbs_.size() + offset;
        if (limbs_.size() < that_size) return -1;
        if (limbs_.size() > that_size) return 1;
        for (int k = static_cast<int>(that.limbs_.size()) - 1; k >= 0; --k) {
            uint32_t a = limbs_[k + offset];
            uint32_t b = that.limbs_[k];
            if (a != b) return (a > b) ? 1 : -1;
        }
        for (int k = static_cast<int>(offset) - 1; k >= 0; --k) {
            if (limbs_[k] != 0) return 1;
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
        for (; i < that.limbs_.size(); ++i) {
            uint64_t sum = static_cast<uint64_t>(that.limbs_[i]) + carry;
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

    big_uint& sub(const big_uint& that, size_t offset = 0) {
        if (offset == 0) {
            assert(*this >= that, "big_uint underflow");
        } else {
            assert(compare_offset(that, offset) >= 0, "big_uint underflow");
        }
        if (limbs_.size() < that.limbs_.size() + offset) {
            limbs_.resize(that.limbs_.size() + offset, 0);
        }
        uint64_t borrow = 0;
        size_t i = 0;
        for (; i < that.limbs_.size(); ++i) {
            size_t idx = i + offset;
            uint64_t diff = static_cast<uint64_t>(limbs_[idx]) - that.limbs_[i] - borrow;
            limbs_[idx] = static_cast<uint32_t>(diff & 0xFFFFFFFF);
            borrow = (diff >> 63);
        }
        for (size_t idx = i + offset; borrow > 0 && idx < limbs_.size(); ++idx) {
            uint64_t diff = static_cast<uint64_t>(limbs_[idx]) - borrow;
            limbs_[idx] = static_cast<uint32_t>(diff & 0xFFFFFFFF);
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

    static big_uint multiply_schoolbook(const big_uint& a, const big_uint& b) {
        if (a.is_zero() || b.is_zero()) return big_uint(0U);
        if (a.limbs_.size() == 1) {
            big_uint res = b;
            res.multiply(a.limbs_[0]);
            return res;
        }
        if (b.limbs_.size() == 1) {
            big_uint res = a;
            res.multiply(b.limbs_[0]);
            return res;
        }
        std::vector<uint32_t> res(a.limbs_.size() + b.limbs_.size(), 0);
        for (size_t i = 0; i < a.limbs_.size(); ++i) {
            uint64_t a_limb = a.limbs_[i];
            if (a_limb == 0) continue;
            uint64_t carry = 0;
            for (size_t j = 0; j < b.limbs_.size(); ++j) {
                uint64_t cur = res[i + j] + a_limb * b.limbs_[j] + carry;
                res[i + j] = static_cast<uint32_t>(cur & 0xFFFFFFFF);
                carry = cur >> 32;
            }
            res[i + b.limbs_.size()] += static_cast<uint32_t>(carry);
        }
        big_uint ans;
        ans.limbs_ = std::move(res);
        ans.remove_trailing_zeros();
        return ans;
    }

    static big_uint multiply_karatsuba(const big_uint& a, const big_uint& b) {
        size_t n = std::max(a.limbs_.size(), b.limbs_.size());
        if (n <= 32 || a.limbs_.size() <= 4 || b.limbs_.size() <= 4) {
            return multiply_schoolbook(a, b);
        }

        size_t m = (n + 1) / 2;

        big_uint a0, a1, b0, b1;
        if (a.limbs_.size() <= m) {
            a0 = a;
        } else {
            a0.limbs_.assign(a.limbs_.begin(), a.limbs_.begin() + m);
            a0.remove_trailing_zeros();
            a1.limbs_.assign(a.limbs_.begin() + m, a.limbs_.end());
            a1.remove_trailing_zeros();
        }

        if (b.limbs_.size() <= m) {
            b0 = b;
        } else {
            b0.limbs_.assign(b.limbs_.begin(), b.limbs_.begin() + m);
            b0.remove_trailing_zeros();
            b1.limbs_.assign(b.limbs_.begin() + m, b.limbs_.end());
            b1.remove_trailing_zeros();
        }

        big_uint z0 = multiply_karatsuba(a0, b0);
        big_uint z2 = multiply_karatsuba(a1, b1);

        big_uint sum_a = a0 + a1;
        big_uint sum_b = b0 + b1;
        big_uint z1 = multiply_karatsuba(sum_a, sum_b);

        big_uint mid = z1 - z2 - z0;

        z2.shift_left_bits(2 * m * 32);
        mid.shift_left_bits(m * 32);

        big_uint res = std::move(z0);
        res.add(mid);
        res.add(z2);
        return res;
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
        *this = multiply_karatsuba(*this, that);
        return *this;
    }

    big_uint& power_2() {
        return multiply(*this);
    }

    big_uint& power(uint32_t exp) {
        if (exp == 0) {
            set_one();
            return *this;
        }
        if (is_zero() || is_one() || exp == 1) {
            return *this;
        }
        big_uint base = *this;
        set_one();
        while (exp > 0) {
            if (exp & 1) {
                multiply(base);
            }
            if (exp > 1) {
                base.power_2();
            }
            exp >>= 1;
        }
        return *this;
    }

    big_uint operator*(uint32_t v) const {
        big_uint res = *this;
        res.multiply(v);
        return res;
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

    big_uint operator<<(size_t bits) const {
        big_uint res = *this;
        res.shift_left_bits(bits);
        return res;
    }

    big_uint operator>>(size_t bits) const {
        big_uint res = *this;
        res.shift_right(bits);
        return res;
    }

    big_uint operator&(const big_uint& that) const {
        big_uint res;
        size_t n = std::min(limbs_.size(), that.limbs_.size());
        res.limbs_.resize(n);
        for (size_t i = 0; i < n; ++i) {
            res.limbs_[i] = limbs_[i] & that.limbs_[i];
        }
        res.remove_trailing_zeros();
        return res;
    }

    big_uint operator|(const big_uint& that) const {
        big_uint res = *this;
        size_t n = std::max(limbs_.size(), that.limbs_.size());
        res.limbs_.resize(n, 0);
        for (size_t i = 0; i < that.limbs_.size(); ++i) {
            res.limbs_[i] |= that.limbs_[i];
        }
        res.remove_trailing_zeros();
        return res;
    }

    big_uint operator^(const big_uint& that) const {
        big_uint res = *this;
        size_t n = std::max(limbs_.size(), that.limbs_.size());
        res.limbs_.resize(n, 0);
        for (size_t i = 0; i < that.limbs_.size(); ++i) {
            res.limbs_[i] ^= that.limbs_[i];
        }
        res.remove_trailing_zeros();
        return res;
    }

    void add_offset(uint32_t val, size_t offset) {
        if (val == 0) return;
        if (limbs_.size() <= offset) {
            limbs_.resize(offset + 1, 0);
        }
        uint64_t carry = val;
        for (size_t idx = offset; carry > 0 && idx < limbs_.size(); ++idx) {
            uint64_t sum = static_cast<uint64_t>(limbs_[idx]) + carry;
            limbs_[idx] = static_cast<uint32_t>(sum & 0xFFFFFFFF);
            carry = sum >> 32;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint32_t>(carry));
        }
    }

    big_uint divide(const big_uint& that, big_uint& remainder) const {
        assert(!that.is_zero(), "Division by zero");
        if (is_zero() || that.is_one()) {
            remainder.set_zero();
            return *this;
        }
        if (is_one()) {
            remainder = big_uint(1U);
            return big_uint(0U);
        }
        if (that.fit_uint32()) {
            uint32_t rem = 0;
            big_uint quotient = *this;
            quotient.divide_uint32(that.limbs_[0], rem);
            remainder = big_uint(rem);
            return quotient;
        }
        remainder = *this;
        if (remainder < that) {
            return big_uint(0U);
        }

        big_uint quotient;
        size_t i = remainder.limbs_.size() - that.limbs_.size();

        while (true) {
            while (true) {
                if (remainder.limbs_.size() < that.limbs_.size() + i) break;

                uint64_t n = 0, d = 0;
                if (remainder.limbs_.size() == that.limbs_.size() + i) {
                    if (remainder.limbs_.size() > 1) {
                        n = remainder.highest_uint64();
                        d = that.highest_uint64();
                    } else {
                        n = remainder.highest_uint32();
                        d = that.highest_uint32();
                    }
                } else {
                    n = remainder.highest_uint64();
                    d = that.highest_uint32();
                }

                if (n < d) break;
                if (n == d) {
                    int cmp = remainder.compare_offset(that, i);
                    if (cmp >= 0) {
                        remainder.sub(that, i);
                        quotient.add_offset(1, i);
                        if (cmp == 0) {
                            quotient.remove_trailing_zeros();
                            return quotient;
                        }
                    }
                    break;
                }

                uint64_t q = n / (d + 1);
                if (q == 0) q = 1;
                if (q > 0xFFFFFFFFULL) q = 0xFFFFFFFFULL;
                uint32_t q32 = static_cast<uint32_t>(q);

                int cmp = remainder.compare_offset(that * q32, i);
                if (cmp < 0) {
                    q32 = 1;
                    cmp = remainder.compare_offset(that, i);
                    if (cmp < 0) break;
                }

                quotient.add_offset(q32, i);
                remainder.sub(that * q32, i);
            }
            if (i == 0) break;
            --i;
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
                a = a.divide(b, rem);
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

    static big_uint factorial(uint32_t n) {
        big_uint res(1U);
        for (uint32_t i = 2; i <= n; ++i) {
            res.multiply(i);
        }
        return res;
    }

    std::string str() const {
        if (is_zero()) return "0";
        big_uint copy = *this;
        std::string res;
        while (!copy.is_zero()) {
            uint32_t rem = 0;
            copy.divide_uint32(1000000000U, rem);
            for (int i = 0; i < 9; ++i) {
                res.push_back(static_cast<char>('0' + (rem % 10)));
                rem /= 10;
                if (rem == 0) {
                    if (!copy.is_zero()) {
                        res.append(9 - i - 1, '0');
                    }
                    break;
                }
            }
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_uint& v) {
        return os << v.str();
    }
};

} // namespace math
} // namespace osi
