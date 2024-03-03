/**
 * @file ArrayHelpers.h
 *
 * @author awong
 */

#include <stddef.h>
#include <array>
#include <algorithm>

template <typename T, size_t N, size_t M>
inline bool widen_array(std::array<T, N>& dst, std::array<T, M> const& src, const size_t len) {
    static_assert(N >= M, "widen_array detected narrowing");
    if (len > N) return false;
    if (len > M) return false;
    std::copy_n(begin(src), len, begin(dst));
    return true;
}

template <typename T, size_t N, size_t M>
inline bool narrow_array(std::array<T, N>& dst, std::array<T, M> const& src, const size_t len) {
    static_assert(M >= N, "narrow_array detected widening");
    if (len > N) return false;
    if (len > M) return false;
    std::copy_n(begin(src), len, begin(dst));
    return true;
}
