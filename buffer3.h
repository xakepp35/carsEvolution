/*
	typed triple buffer (pointer swapping) engine
	operator[] is availble to producer(writer) and consumer(reader) threads
	atomic exchanges are used to locklessly transfer data from writer to reader
	you just have to track relevance, may be easily implemented by incorporating frameNumber in user_data structure

*/
#pragma once

#include <atomic>
#include <array>

template< typename user_data >
class buffer3
{
public:

	enum kind : size_t {
		Writer,
		Reader,
		Count,
	};

	typedef user_data		value;
	typedef user_data*		pointer;


	buffer3() :
		_ptrExchange(nullptr),
		_ownedPtrs({ nullptr }),
		_bufStore()
	{
		for (size_t i = 0; i < static_cast<size_t>(kind::Count); i++)
			_ownedPtrs[i] = &_bufStore[i];
		_ptrExchange.store(&_bufStore.back());
	}

	// sync method, call once to obtain "latest" data reference, before you are going read and after you wrote
	value& operator[](const kind& workerKind) {
		auto& currentPtr = _ownedPtrs[static_cast<size_t>(workerKind)];
		currentPtr = _ptrExchange.exchange(currentPtr);
		return *currentPtr;
	}

protected:
	
	typedef std::atomic< pointer >											atom_ptr;
	typedef std::array< pointer, static_cast<size_t>(kind::Count) >			buf_ptrs;
	typedef std::array< value, static_cast<size_t>(kind::Count) + 1 >		buf_store;

	atom_ptr		_ptrExchange;
	buf_ptrs		_ownedPtrs;
	buf_store		_bufStore;
	
};