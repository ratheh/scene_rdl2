// Copyright 2023-2024 DreamWorks Animation LLC
// SPDX-License-Identifier: Apache-2.0

#pragma once

// Include this before any other includes!
#include <scene_rdl2/common/platform/Platform.h>

#include <atomic>

namespace scene_rdl2 {
namespace util {

template<typename T>
struct CACHE_ALIGN CacheLineAtomic
{
    std::atomic<T> value;

    CacheLineAtomic() : value() {}
    CacheLineAtomic(T v) : value(v) {}

    operator T() const { return value.load(); }
    T operator=(T v) { value.store(v); return v; }
    T operator++() { return ++value; }
    T operator++(int) { return value++; }
    T operator--() { return --value; }
    T operator--(int) { return value--; }
    T operator+=(T v) { return value += v; }
    T operator-=(T v) { return value -= v; }

    T load() const { return value.load(); }
    void store(T v) { value.store(v); }
    T exchange(T v) { return value.exchange(v); }
    bool compare_exchange_strong(T& expected, T desired) { return value.compare_exchange_strong(expected, desired); }
    bool compare_exchange_weak(T& expected, T desired) { return value.compare_exchange_weak(expected, desired); }
    T fetch_add(T v) { return value.fetch_add(v); }
    T fetch_sub(T v) { return value.fetch_sub(v); }
};

MNRY_STATIC_ASSERT(sizeof(CacheLineAtomic<uint32_t>) == CACHE_LINE_SIZE);

} // namespace util
} // namespace scene_rdl2


