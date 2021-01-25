#pragma once

#define SSTRINGIFY(X) #X
#define STRINGIFY(X) SSTRINGIFY(X)

#include <cstdint>



#include <exception>
#define CHK_EX(FAIL_CONDITION) { auto isFailed = FAIL_CONDITION; if( isFailed ) throw std::exception(__FUNCTION__ "(): " STRINGIFY(FAIL_CONDITION) ); }
#define CHK_PTR(NOT_NULL_RESULT) { auto isFailed = ((NOT_NULL_RESULT)==0); if( isFailed ) throw std::exception(__FUNCTION__ "(): " STRINGIFY(NOT_NULL_RESULT) ); }


#include <emmintrin.h>
#include <xmmintrin.h>



// to easy switch to AVX2..
#define MM_ALIGMENT 16
#define MMR __declspec(align(16)) mmr
typedef __declspec(align(16)) __m128 mmr;

#include <memory>

// aligned allocation with filling memory with zeroes
template< typename T=mmr > T* aalloc(size_t elementCount, size_t addrAligment = MM_ALIGMENT) {
	auto rawByteSize = elementCount * sizeof(T);
	return reinterpret_cast<T*>(memset(_mm_malloc(rawByteSize, addrAligment), 0, rawByteSize));
}

template<typename T=mmr > size_t mm_count(size_t nItems) {
	return nItems * sizeof(float) / sizeof(T);
}