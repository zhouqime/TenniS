#ifndef TENSORSTACK_KERNELS_COMMON_SIMD_DEF_SIMD_BASE_DEF_H
#define TENSORSTACK_KERNELS_COMMON_SIMD_DEF_SIMD_BASE_DEF_H

#include <array>

using _simd_f32 = float;
using _simd_f32x4 = std::array<_simd_f32, 4>;
using _simd_f32x4x2 = std::array<_simd_f32, 8>;

inline _simd_f32x4 _simd_f32x4_load(const _simd_f32 *p) {
    return { p[0], p[1], p[2], p[3] };
}

inline _simd_f32x4 _simd_f32x4_set(_simd_f32 a, _simd_f32 b, _simd_f32 c, _simd_f32 d) {
    return { a, b, c, d };
}

inline void _simd_f32x4_store(_simd_f32 *p, _simd_f32x4 m) {
    p[0] = m[0];
    p[1] = m[1];
    p[2] = m[2];
    p[3] = m[3];
}

inline _simd_f32x4 _simd_f32x4_add(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return { lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] };
}

inline _simd_f32x4 _simd_f32x4_sub(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return { lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] };
}

inline _simd_f32x4 _simd_f32x4_mul(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return { lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] };
}

inline _simd_f32x4 _simd_f32x4_div(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return { lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] };
}

inline _simd_f32x4 _simd_f32x4_max(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return{ std::max(lhs[0],rhs[0]), std::max(lhs[1],rhs[1]), std::max(lhs[2],rhs[2]), std::max(lhs[3],rhs[3]) };
}

inline _simd_f32x4 _simd_f32x4_min(_simd_f32x4 lhs, _simd_f32x4 rhs) {
    return{ std::min(lhs[0],rhs[0]), std::min(lhs[1],rhs[1]), std::min(lhs[2],rhs[2]), std::min(lhs[3],rhs[3]) };
}

inline void _simd_f32x4_transpose4x4(_simd_f32x4& q0, _simd_f32x4& q1, _simd_f32x4& q2, _simd_f32x4& q3) {
    //TODO:optimize?
    /*
        q0[0] = q0[0]; q1[0] = q0[1]; q2[0] = q0[2]; q3[0] = q0[3];
        q0[1] = q1[0]; q1[1] = q1[1]; q2[1] = q1[2]; q3[1] = q1[3];
        q0[2] = q2[0]; q1[2] = q2[1]; q2[2] = q2[2]; q3[2] = q2[3];
        q0[3] = q3[0]; q1[3] = q3[1]; q2[3] = q3[2]; q3[3] = q3[3];   
    */
    _simd_f32 t0[4], t1[4], t2[4], t3[4];
    t0[0] = q0[0]; t1[0] = q0[1]; t2[0] = q0[2]; t3[0] = q0[3];
    t0[1] = q1[0]; t1[1] = q1[1]; t2[1] = q1[2]; t3[1] = q1[3];
    t0[2] = q2[0]; t1[2] = q2[1]; t2[2] = q2[2]; t3[2] = q2[3];
    t0[3] = q3[0]; t1[3] = q3[1]; t2[3] = q3[2]; t3[3] = q3[3];
    for (int i = 0; i < 4; i++)
    {
        q0[i] = t0[i]; q1[i] = t1[i]; q2[i] = t2[i]; q3[i] = t3[i];
    }

}

inline _simd_f32x4 _simd_f32x4_fmadd(const _simd_f32x4& q0, const _simd_f32x4& q1, const _simd_f32x4& q2) {
    return {q0[0] * q1[0] + q2[0], q0[1] * q1[1] + q2[1], q0[2] * q1[2] + q2[2], q0[3] * q1[3] + q2[3]};
}


inline _simd_f32x4x2 _simd_f32x4x2_load(const _simd_f32 *p) {
    return { p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] };
}

inline _simd_f32x4x2 _simd_f32x4x2_set(_simd_f32 a, _simd_f32 b, _simd_f32 c, _simd_f32 d,
                                     _simd_f32 e, _simd_f32 f, _simd_f32 g, _simd_f32 h) {
    return {a, b, c, d, e, f, g, h};
}

inline void _simd_f32x4x2_store(_simd_f32 *p, _simd_f32x4x2 m) {
    p[0] = m[0];p[1] = m[1];
    p[2] = m[2];p[3] = m[3];
    p[4] = m[4];p[5] = m[5];
    p[6] = m[6];p[7] = m[7];
}

inline _simd_f32x4x2 _simd_f32x4x2_add(_simd_f32x4x2 lhs, _simd_f32x4x2 rhs) {
    return { lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3], lhs[4] + rhs[4], lhs[5] + rhs[5], lhs[6] + rhs[6], lhs[7] + rhs[7]};
}

inline _simd_f32x4x2 _simd_f32x4x2_sub(_simd_f32x4x2 lhs, _simd_f32x4x2 rhs) {
    return { lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3], lhs[4] - rhs[4], lhs[5] - rhs[5], lhs[6] - rhs[6], lhs[7] - rhs[7]};
}

inline _simd_f32x4x2 _simd_f32x4x2_mul(_simd_f32x4x2 lhs, _simd_f32x4x2 rhs) {
    return { lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3], lhs[4] * rhs[4], lhs[5] * rhs[5], lhs[6] * rhs[6], lhs[7] * rhs[7]};
}

inline _simd_f32x4x2 _simd_f32x4x2_div(_simd_f32x4x2 lhs, _simd_f32x4x2 rhs) {
    return { lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3], lhs[4] / rhs[4], lhs[5] / rhs[5], lhs[6] / rhs[6], lhs[7] / rhs[7]};
}

inline _simd_f32x4x2 _simd_f32x4x2_fmadd(_simd_f32x4x2 q0, _simd_f32x4x2 q1, _simd_f32x4x2 q2) {
    return {q0[0] * q1[0] + q2[0], q0[1] * q1[1] + q2[1], q0[2] * q1[2] + q2[2], q0[3] * q1[3] + q2[3], q0[4] * q1[4] + q2[4], q0[5] * q1[5] + q2[5], q0[6] * q1[6] + q2[6], q0[7] * q1[7] + q2[7]};
}

#endif //TENSORSTACK_KERNELS_COMMON_SIMD_DEF_SIMD_BASE_DEF_H