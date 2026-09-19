#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <iostream>
#include "../app_info/assert.hpp"

namespace osi {
namespace math {

using uint128_t = unsigned __int128;

class big_uint {
private:
    std::vector<uint64_t> limbs_;

    void remove_trailing_zeros() {
        while (limbs_.size() > 1 && limbs_.back() == 0) {
            limbs_.pop_back();
        }
    }

public:
    big_uint() : limbs_{0} {}

    template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, int>::type = 0>
    explicit big_uint(T val) {
        if (val == 0) {
            limbs_ = {0};
        } else {
            limbs_ = {static_cast<uint64_t>(val)};
        }
    }

    explicit big_uint(const std::string& str) {
        set_zero();
        if (str.empty()) return;
        size_t start = 0;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
            start++;
        }
        size_t end = start;
        while (end < str.size() && str[end] >= '0' && str[end] <= '9') {
            end++;
        }
        size_t len = end - start;
        if (len == 0) return;

        size_t rem_digits = len % 18;
        size_t pos = start;
        if (rem_digits > 0) {
            uint64_t v = 0;
            for (size_t i = 0; i < rem_digits; ++i) {
                v = v * 10 + static_cast<uint64_t>(str[pos++] - '0');
            }
            limbs_ = {v};
        }
        constexpr uint64_t base18 = 1000000000000000000ULL;
        while (pos < end) {
            uint64_t v = 0;
            for (int i = 0; i < 18; ++i) {
                v = v * 10 + static_cast<uint64_t>(str[pos++] - '0');
            }
            multiply(base18);
            add(big_uint(v));
        }
    }

    explicit big_uint(const std::vector<uint8_t>& bytes) {
        set_zero();
        if (bytes.empty()) return;
        limbs_.clear();
        for (size_t i = 0; i < bytes.size(); i += 8) {
            uint64_t limb = 0;
            for (size_t j = 0; j < 8 && (i + j) < bytes.size(); ++j) {
                limb |= (static_cast<uint64_t>(bytes[i + j]) << (j * 8));
            }
            limbs_.push_back(limb);
        }
        remove_trailing_zeros();
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
        return limbs_.size() * 2;
    }

    size_t bit_count() const {
        if (is_zero()) return 0;
        size_t full = (limbs_.size() - 1) * 64;
        uint64_t top = limbs_.back();
        size_t top_bits = top == 0 ? 0 : static_cast<size_t>(64 - __builtin_clzll(top));
        return full + top_bits;
    }

    bool fit_uint32() const {
        return limbs_.size() == 1 && limbs_[0] <= 0xFFFFFFFFULL;
    }

    uint32_t as_uint32() const {
        return limbs_.empty() ? 0 : static_cast<uint32_t>(limbs_[0]);
    }

    bool fit_uint64() const {
        return limbs_.size() == 1;
    }

    uint64_t as_uint64() const {
        return limbs_.empty() ? 0 : limbs_[0];
    }

    uint64_t highest_uint64() const {
        return limbs_.empty() ? 0 : limbs_.back();
    }

    uint32_t highest_uint32() const {
        if (limbs_.empty()) return 0;
        uint64_t top = limbs_.back();
        return (top >> 32) != 0 ? static_cast<uint32_t>(top >> 32) : static_cast<uint32_t>(top);
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
            uint64_t a = limbs_[k + offset];
            uint64_t b = that.limbs_[k];
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
        uint128_t carry = 0;
        size_t i = 0;
        for (; i < common; ++i) {
            uint128_t sum = static_cast<uint128_t>(limbs_[i]) + that.limbs_[i] + carry;
            limbs_[i] = static_cast<uint64_t>(sum);
            carry = sum >> 64;
        }
        for (; i < that.limbs_.size(); ++i) {
            uint128_t sum = static_cast<uint128_t>(that.limbs_[i]) + carry;
            limbs_[i] = static_cast<uint64_t>(sum);
            carry = sum >> 64;
        }
        for (; carry > 0 && i < limbs_.size(); ++i) {
            uint128_t sum = static_cast<uint128_t>(limbs_[i]) + carry;
            limbs_[i] = static_cast<uint64_t>(sum);
            carry = sum >> 64;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint64_t>(carry));
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
        uint128_t borrow = 0;
        size_t i = 0;
        for (; i < that.limbs_.size(); ++i) {
            size_t idx = i + offset;
            uint128_t diff = static_cast<uint128_t>(limbs_[idx]) - that.limbs_[i] - borrow;
            limbs_[idx] = static_cast<uint64_t>(diff);
            borrow = (diff >> 127);
        }
        for (size_t idx = i + offset; borrow > 0 && idx < limbs_.size(); ++idx) {
            uint128_t diff = static_cast<uint128_t>(limbs_[idx]) - borrow;
            limbs_[idx] = static_cast<uint64_t>(diff);
            borrow = (diff >> 127);
        }
        remove_trailing_zeros();
        return *this;
    }

    big_uint operator-(const big_uint& that) const {
        big_uint res = *this;
        res.sub(that);
        return res;
    }

    big_uint& multiply(uint64_t factor) {
        if (factor == 0 || is_zero()) {
            set_zero();
            return *this;
        }
        if (factor == 1) return *this;
        if ((factor & (factor - 1)) == 0) {
            size_t bits = static_cast<size_t>(__builtin_ctzll(factor));
            shift_left_bits(bits);
            return *this;
        }
        uint128_t carry = 0;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint128_t prod = static_cast<uint128_t>(limbs_[i]) * factor + carry;
            limbs_[i] = static_cast<uint64_t>(prod);
            carry = prod >> 64;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint64_t>(carry));
        }
        remove_trailing_zeros();
        return *this;
    }

    static big_uint multiply_schoolbook(const big_uint& a, const big_uint& b) {
        if (a.is_zero() || b.is_zero()) return big_uint(0ULL);
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
        std::vector<uint64_t> res(a.limbs_.size() + b.limbs_.size(), 0);
        for (size_t i = 0; i < a.limbs_.size(); ++i) {
            uint128_t a_limb = a.limbs_[i];
            if (a_limb == 0) continue;
            uint128_t carry = 0;
            for (size_t j = 0; j < b.limbs_.size(); ++j) {
                uint128_t cur = res[i + j] + a_limb * b.limbs_[j] + carry;
                res[i + j] = static_cast<uint64_t>(cur);
                carry = cur >> 64;
            }
            res[i + b.limbs_.size()] += static_cast<uint64_t>(carry);
        }
        big_uint ans;
        ans.limbs_ = std::move(res);
        ans.remove_trailing_zeros();
        return ans;
    }

    static big_uint multiply_karatsuba(const big_uint& a, const big_uint& b) {
        size_t n = std::max(a.limbs_.size(), b.limbs_.size());
        if (n <= 16 || a.limbs_.size() <= 2 || b.limbs_.size() <= 2) {
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

        z2.shift_left_bits(2 * m * 64);
        mid.shift_left_bits(m * 64);

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
            uint64_t factor = limbs_[0];
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

    big_uint operator*(uint64_t v) const {
        big_uint res = *this;
        res.multiply(v);
        return res;
    }

    big_uint operator*(const big_uint& that) const {
        big_uint res = *this;
        res.multiply(that);
        return res;
    }

    big_uint& divide_uint64(uint64_t divisor, uint64_t& remainder) {
        assert(divisor != 0, "Division by zero");
        if (divisor == 1) {
            remainder = 0;
            return *this;
        }
        if ((divisor & (divisor - 1)) == 0) {
            size_t bits = static_cast<size_t>(__builtin_ctzll(divisor));
            remainder = limbs_.empty() ? 0 : (limbs_[0] & (divisor - 1));
            shift_right(bits);
            return *this;
        }
        uint128_t rem = 0;
        for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
            uint128_t cur = (rem << 64) | limbs_[i];
            limbs_[i] = static_cast<uint64_t>(cur / divisor);
            rem = cur % divisor;
        }
        remainder = static_cast<uint64_t>(rem);
        remove_trailing_zeros();
        return *this;
    }

    big_uint& divide_uint32(uint32_t divisor, uint32_t& remainder) {
        uint64_t rem64 = 0;
        divide_uint64(divisor, rem64);
        remainder = static_cast<uint32_t>(rem64);
        return *this;
    }

    size_t trailing_binary_zeros() const {
        if (is_zero()) return 0;
        size_t count = 0;
        for (uint64_t limb : limbs_) {
            if (limb == 0) {
                count += 64;
            } else {
                count += static_cast<size_t>(__builtin_ctzll(limb));
                break;
            }
        }
        return count;
    }

    void shift_right(size_t bits) {
        if (bits == 0 || is_zero()) return;
        size_t limb_shift = bits / 64;
        size_t bit_shift = bits % 64;
        if (limb_shift >= limbs_.size()) {
            set_zero();
            return;
        }
        limbs_.erase(limbs_.begin(), limbs_.begin() + limb_shift);
        if (bit_shift > 0) {
            uint128_t carry = 0;
            for (int i = static_cast<int>(limbs_.size()) - 1; i >= 0; --i) {
                uint128_t cur = (carry << 64) | limbs_[i];
                limbs_[i] = static_cast<uint64_t>(cur >> bit_shift);
                carry = cur & ((static_cast<uint128_t>(1) << bit_shift) - 1);
            }
        }
        remove_trailing_zeros();
    }

    void shift_left_bits(size_t bits) {
        if (bits == 0 || is_zero()) return;
        size_t limb_shift = bits / 64;
        size_t bit_shift = bits % 64;

        if (bit_shift > 0) {
            uint128_t carry = 0;
            for (size_t i = 0; i < limbs_.size(); ++i) {
                uint128_t v = (static_cast<uint128_t>(limbs_[i]) << bit_shift) | carry;
                limbs_[i] = static_cast<uint64_t>(v);
                carry = v >> 64;
            }
            if (carry > 0) {
                limbs_.push_back(static_cast<uint64_t>(carry));
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

    big_uint operator~() const {
        big_uint res = *this;
        for (size_t i = 0; i < res.limbs_.size(); ++i) {
            res.limbs_[i] = ~res.limbs_[i];
        }
        res.remove_trailing_zeros();
        return res;
    }

    std::vector<uint8_t> as_bytes() const {
        if (is_zero()) return {};
        std::vector<uint8_t> bytes;
        size_t total_uint32 = (limbs_.size() - 1) * 2;
        if ((limbs_.back() >> 32) != 0) {
            total_uint32 += 2;
        } else {
            total_uint32 += 1;
        }
        bytes.resize(total_uint32 * 4);
        for (size_t i = 0; i < limbs_.size(); ++i) {
            uint64_t limb = limbs_[i];
            size_t base = i * 8;
            for (size_t j = 0; j < 8 && (base + j) < bytes.size(); ++j) {
                bytes[base + j] = static_cast<uint8_t>((limb >> (j * 8)) & 0xFF);
            }
        }
        return bytes;
    }

    void write_binary(std::ostream& os) const {
        uint64_t count = static_cast<uint64_t>(limbs_.size());
        os.write(reinterpret_cast<const char*>(&count), sizeof(count));
        if (count > 0) {
            os.write(reinterpret_cast<const char*>(limbs_.data()), static_cast<std::streamsize>(count * sizeof(uint64_t)));
        }
    }

    bool read_binary(std::istream& is) {
        uint64_t count = 0;
        if (!is.read(reinterpret_cast<char*>(&count), sizeof(count))) return false;
        limbs_.resize(static_cast<size_t>(count));
        if (count > 0) {
            if (!is.read(reinterpret_cast<char*>(limbs_.data()), static_cast<std::streamsize>(count * sizeof(uint64_t)))) {
                return false;
            }
        }
        remove_trailing_zeros();
        if (limbs_.empty()) set_zero();
        return true;
    }

    static bool binary_to_decimal(std::istream& is, std::ostream& os) {
        big_uint v;
        if (!v.read_binary(is)) return false;
        os << v.str();
        return true;
    }

    static std::string binary_to_decimal(std::istream& is) {
        big_uint v;
        if (!v.read_binary(is)) return "";
        return v.str();
    }

    void add_offset(uint64_t val, size_t offset) {
        if (val == 0) return;
        if (limbs_.size() <= offset) {
            limbs_.resize(offset + 1, 0);
        }
        uint128_t carry = val;
        for (size_t idx = offset; carry > 0 && idx < limbs_.size(); ++idx) {
            uint128_t sum = static_cast<uint128_t>(limbs_[idx]) + carry;
            limbs_[idx] = static_cast<uint64_t>(sum);
            carry = sum >> 64;
        }
        if (carry > 0) {
            limbs_.push_back(static_cast<uint64_t>(carry));
        }
    }

    big_uint divide(const big_uint& that, big_uint& remainder) const {
        assert(!that.is_zero(), "Division by zero");
        if (is_zero() || that.is_one()) {
            remainder.set_zero();
            return *this;
        }
        if (is_one()) {
            remainder = big_uint(1ULL);
            return big_uint(0ULL);
        }
        if (that.fit_uint64()) {
            uint64_t rem = 0;
            big_uint quotient = *this;
            quotient.divide_uint64(that.limbs_[0], rem);
            remainder = big_uint(rem);
            return quotient;
        }
        if (*this < that) {
            remainder = *this;
            return big_uint(0ULL);
        }
        if (*this == that) {
            remainder.set_zero();
            return big_uint(1ULL);
        }

        // Knuth Algorithm D in-place
        std::vector<uint64_t> u = limbs_;
        std::vector<uint64_t> v = that.limbs_;

        size_t n = v.size();
        size_t m = u.size() - n;

        int shift = __builtin_clzll(v.back());
        if (shift > 0) {
            uint64_t carry = 0;
            for (size_t i = 0; i < n; ++i) {
                uint64_t next_carry = (shift == 64) ? 0 : (v[i] >> (64 - shift));
                v[i] = (v[i] << shift) | carry;
                carry = next_carry;
            }
            carry = 0;
            for (size_t i = 0; i < u.size(); ++i) {
                uint64_t next_carry = (shift == 64) ? 0 : (u[i] >> (64 - shift));
                u[i] = (u[i] << shift) | carry;
                carry = next_carry;
            }
            if (carry > 0) u.push_back(carry);
        }
        while (u.size() < n + m + 1) u.push_back(0);

        std::vector<uint64_t> q(m + 1, 0);
        uint64_t v_n1 = v[n - 1];
        uint64_t v_n2 = (n >= 2) ? v[n - 2] : 0;

        for (int j = static_cast<int>(m); j >= 0; --j) {
            uint128_t u_top = (static_cast<uint128_t>(u[j + n]) << 64) | u[j + n - 1];
            uint128_t q_hat = u_top / v_n1;
            uint128_t r_hat = u_top % v_n1;

            while (q_hat > 0xFFFFFFFFFFFFFFFFULL ||
                   (n >= 2 && q_hat * v_n2 > ((r_hat << 64) | u[j + n - 2]))) {
                --q_hat;
                r_hat += v_n1;
                if (r_hat > 0xFFFFFFFFFFFFFFFFULL) break;
            }

            uint64_t qh = static_cast<uint64_t>(q_hat);

            uint128_t borrow = 0;
            for (size_t i = 0; i < n; ++i) {
                uint128_t prod = static_cast<uint128_t>(qh) * v[i] + borrow;
                uint64_t prod_lo = static_cast<uint64_t>(prod);
                borrow = prod >> 64;
                if (u[j + i] < prod_lo) {
                    ++borrow;
                }
                u[j + i] -= prod_lo;
            }
            if (u[j + n] < borrow) {
                u[j + n] -= static_cast<uint64_t>(borrow);
                --qh;
                uint128_t carry = 0;
                for (size_t i = 0; i < n; ++i) {
                    uint128_t sum = static_cast<uint128_t>(u[j + i]) + v[i] + carry;
                    u[j + i] = static_cast<uint64_t>(sum);
                    carry = sum >> 64;
                }
                u[j + n] += static_cast<uint64_t>(carry);
            } else {
                u[j + n] -= static_cast<uint64_t>(borrow);
            }

            q[j] = qh;
        }

        if (shift > 0) {
            uint64_t carry = 0;
            for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
                uint64_t next_carry = (shift == 64) ? 0 : (u[i] << (64 - shift));
                u[i] = (u[i] >> shift) | carry;
                carry = next_carry;
            }
        }
        u.resize(n);

        big_uint res_q;
        res_q.limbs_ = std::move(q);
        res_q.remove_trailing_zeros();

        remainder.limbs_ = std::move(u);
        remainder.remove_trailing_zeros();

        return res_q;
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
        if (a.is_zero() || b.is_zero()) return big_uint(0ULL);
        if (a.is_one() || b.is_one()) return big_uint(1ULL);
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
        big_uint res(1ULL);
        for (uint32_t i = 2; i <= n; ++i) {
            res.multiply(static_cast<uint64_t>(i));
        }
        return res;
    }

    static std::string base_str(big_uint copy) {
        if (copy.is_zero()) return "0";
        std::string res;
        constexpr uint64_t base18 = 1000000000000000000ULL;
        while (!copy.is_zero()) {
            uint64_t rem = 0;
            copy.divide_uint64(base18, rem);
            for (int i = 0; i < 18; ++i) {
                res.push_back(static_cast<char>('0' + (rem % 10)));
                rem /= 10;
                if (rem == 0) {
                    if (!copy.is_zero()) {
                        res.append(18 - i - 1, '0');
                    }
                    break;
                }
            }
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::string str() const;

    friend std::ostream& operator<<(std::ostream& os, const big_uint& v) {
        return os << v.str();
    }
};

struct BarrettPower {
    big_uint B;        // 10^K
    big_uint M;        // floor(2^(2k) / B)
    size_t shift;      // 2 * k
    size_t num_digits; // K
};

inline std::vector<BarrettPower>& get_barrett_powers_cache() {
    static std::vector<BarrettPower> cache;
    return cache;
}

inline void ensure_barrett_powers_up_to(const big_uint& target) {
    auto& cache = get_barrett_powers_cache();
    constexpr uint64_t base18 = 1000000000000000000ULL;
    if (cache.empty()) {
        BarrettPower bp;
        bp.B = big_uint(base18);
        bp.num_digits = 18;
        bp.shift = 2 * bp.B.bit_count();
        big_uint two_to_shift(1ULL);
        two_to_shift.shift_left_bits(bp.shift);
        big_uint rem;
        bp.M = two_to_shift.divide(bp.B, rem);
        cache.push_back(std::move(bp));
    }

    while (cache.back().B <= target) {
        big_uint next_b = cache.back().B;
        next_b.power_2();

        BarrettPower bp;
        bp.B = std::move(next_b);
        bp.num_digits = cache.back().num_digits * 2;
        bp.shift = 2 * bp.B.bit_count();
        big_uint two_to_shift(1ULL);
        two_to_shift.shift_left_bits(bp.shift);
        big_uint rem;
        bp.M = two_to_shift.divide(bp.B, rem);
        cache.push_back(std::move(bp));
    }
}

inline void barrett_div(const big_uint& X, const BarrettPower& bp, big_uint& Q, big_uint& R) {
    if (X < bp.B) {
        Q.set_zero();
        R = X;
        return;
    }
    big_uint prod = X * bp.M;
    prod.shift_right(bp.shift);
    Q = std::move(prod);

    big_uint QB = Q * bp.B;
    if (X >= QB) {
        R = X - QB;
    } else {
        Q.sub(big_uint(1ULL));
        R = X - (Q * bp.B);
    }
    while (R >= bp.B) {
        R.sub(bp.B);
        Q.add(big_uint(1ULL));
    }
}

inline void format_cached_barrett_rec(const big_uint& val, size_t power_idx,
                                      std::string& out, bool pad, size_t pad_len) {
    const auto& cache = get_barrett_powers_cache();
    if (power_idx == 0 || val < cache[0].B) {
        std::string s = big_uint::base_str(val);
        if (pad && s.size() < pad_len) {
            out.append(pad_len - s.size(), '0');
        }
        out.append(s);
        return;
    }

    while (power_idx > 0 && cache[power_idx].B > val) {
        --power_idx;
    }

    if (cache[power_idx].B > val) {
        std::string s = big_uint::base_str(val);
        if (pad && s.size() < pad_len) {
            out.append(pad_len - s.size(), '0');
        }
        out.append(s);
        return;
    }

    big_uint q, rem;
    barrett_div(val, cache[power_idx], q, rem);
    size_t block_digits = cache[power_idx].num_digits;

    format_cached_barrett_rec(q, power_idx > 0 ? power_idx - 1 : 0, out,
                              pad, (pad && pad_len > block_digits) ? (pad_len - block_digits) : 0);
    format_cached_barrett_rec(rem, power_idx > 0 ? power_idx - 1 : 0, out,
                              true, block_digits);
}

inline std::string big_uint::str() const {
    if (is_zero()) return "0";
    if (bit_count() < 8000) {
        return base_str(*this);
    }

    ensure_barrett_powers_up_to(*this);
    const auto& cache = get_barrett_powers_cache();

    std::string out;
    format_cached_barrett_rec(*this, cache.size() - 1, out, false, 0);
    return out;
}

} // namespace math
} // namespace osi
