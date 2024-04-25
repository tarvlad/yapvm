#pragma once

/* Key-value storage implemented by hashset with open addressing
 *
 * (C) V.Tarasov, Lavrentyev Institute of Hydrodynamics SB RAS, 2023
 *     tarvlad@inbox.ru
 */

#include <functional>
#include <cstddef>
#include <memory>
#include <array>
#include <cassert>
#include "kvstorage_element.h"
#include <iostream>
#include <optional>

#define ERROR_RUNTIME_NULLPTR_DEREFERENCE 17


#define NULLCHECK(ptr) \
if (ptr == nullptr) { \
	std::cerr << "ERROR: runtime null pointer dereference\n"; \
	exit(ERROR_RUNTIME_NULLPTR_DEREFERENCE); \
}


template <
	typename Key,
	typename Value,
	size_t OccupacyRateReallocUpPercentage = 70,
	size_t OccupacyRateReallocDownPercentage = 30,
	typename Hash = std::hash<Key>,
	size_t ReduceDeletedPercentage = 10,
	typename KeyEqual = std::equal_to<Key>,
	typename DataAllocator = std::allocator<KVStorageElement<Key, Value>>
>
class KVStorage {
	static_assert(OccupacyRateReallocUpPercentage < 100);
	static_assert(OccupacyRateReallocDownPercentage < OccupacyRateReallocUpPercentage);
	static_assert(ReduceDeletedPercentage < 50);

	//near to 2^32
	constexpr static std::array<size_t, 26> _capacities = {
		53,
		97,
		193,
		389,
		769,
		1543,
		3079,
		6151,
		12289,
		24593,
		49157,
		98317,
		196613,
		393241,
		786433,
		1572869,
		3145739,
		6291469,
		12582917,
		25165843,
		50331653,
		100663319,
		201326611,
		402653189,
		805306457,
		1610612741
	};
	size_t _capacitiesIdx;


	DataAllocator _dataAllocator;
	KVStorageElement<Key, Value>* _data;
	size_t _size;


	size_t capacity() const {
		return _capacities[_capacitiesIdx];
	}


	size_t _hashPos(const Key key) const {
		return Hash{}(key) % capacity();
	}


	size_t _nextPos(const size_t pos) const {
		return (pos + 1) % capacity();
	}


	size_t _deletedCtr;


	void __rehashData(const size_t prevCapacity) {
		KVStorageElement<Key, Value>* prevData = _data;
		_data = _dataAllocator.allocate(capacity());
		NULLCHECK(_data);
		for (size_t i = 0; i < capacity(); i++) {
			_data[i].exists = false;
		}

		_size = 0;
		_deletedCtr = 0;

		for (size_t i = 0; i < prevCapacity; i++) {
			if (prevData[i].exists) {
				if (!prevData[i].deleted) {
					__add(std::move(prevData[i].key), std::move(prevData[i].value), false);

				}
				std::destroy_at(&prevData[i]);
			}
		}

		_dataAllocator.deallocate(prevData, prevCapacity);
	}


	void __resize() {
		double usage = static_cast<double>(_size) / static_cast<double>(capacity());
		size_t prevCapacity = capacity();

		if (usage > static_cast<double>(OccupacyRateReallocUpPercentage) / static_cast<double>(100)) {
			if (_capacitiesIdx + 1 == _capacities.size()) {
				return;
			}

			_capacitiesIdx++;
		}
		else if (usage < static_cast<double>(OccupacyRateReallocDownPercentage) / static_cast<double>(100)) {
			if (_capacitiesIdx == 0) {
				return;
			}

			_capacitiesIdx--;
		}

		if (prevCapacity != capacity()) {
			__rehashData(prevCapacity);
		}
	}


	void __reduceDeleted() {
		if (static_cast<double>(_deletedCtr) / static_cast<double>(capacity()) >
				static_cast<double>(ReduceDeletedPercentage) / static_cast<double>(100)) {
			__rehashData(capacity());
		}
	}


	bool __add(const Key& key, const Value& value, const bool needResize) {
		size_t pos = _hashPos(key);
		size_t ctr = 0;

		for (;;) {
			if (ctr++ > capacity()) {
				return false;
			}
			if (!_data[pos].exists) {
				std::construct_at(&_data[pos], key, value);

				break;
			}
			if (_data[pos].deleted) {
				std::construct_at(&_data[pos], key, value);
				_deletedCtr--;

				break;
			}

			pos = _nextPos(pos);
		}

		_size++;
		if (needResize) {
			__resize();
			__reduceDeleted();
		}

		return true;
	}


	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> __find(const Key& key) {
		size_t pos = _hashPos(key);
		std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> ret;
		size_t ctr = 0;

		while (ctr++ <= capacity()) {
			if (!_data[pos].exists) {
				break;
			}
			if (!_data[pos].deleted && KeyEqual{}(_data[pos].key, key)) {
				ret.emplace(std::reference_wrapper<KVStorageElement<Key, Value>>(_data[pos]));

				break;
			}

			pos = _nextPos(pos);
		}

		return ret;
	}


	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> __del(const Key& key) {
		size_t pos = _hashPos(key);
		std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> ret;
		size_t ctr = 0;

		while (ctr++ <= capacity()) {
			if (!_data[pos].exists) {
				break;
			}
			if (!_data[pos].deleted && KeyEqual{}(_data[pos].key, key)) {
				ret.emplace(std::reference_wrapper<KVStorageElement<Key, Value>>(_data[pos]));
				_data[pos].deleted = true;

				_deletedCtr++;
				_size--;

				break;
			}

			pos = _nextPos(pos);
		}


		__resize();
		__reduceDeleted();
		return ret;
	}


public:
	size_t size() const {
		return _size;
	}


	double usage() const {
		return static_cast<double>(_size) / static_cast<double>(capacity());
	}


	KVStorage()
			: _capacitiesIdx(0), _dataAllocator(), _size(0), _deletedCtr(0) {
		_data = _dataAllocator.allocate(capacity());
		NULLCHECK(_data);

		for (size_t i = 0; i < capacity(); i++) {
			_data[i].exists = false;
		}
	}


	~KVStorage() {
		NULLCHECK(_data);
		for (size_t i = 0; i < capacity(); i++) {
			if (_data[i].exists) {
				std::destroy_at(&_data[i]);
			}
		}

		_dataAllocator.deallocate(_data, capacity());
	}


	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> find(const Key& key) {
		return __find(key);
	}

	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> find(Key&& key) {
		return __find(std::move(key));
	}


	bool add(Key&& key, Value&& value) {
		return __add(std::move(key), std::move(value), true);
	}


	bool add(const Key& key, const Value& value) {
		return __add(key, value, true);
	}


	bool add(Key&& key, const Value& value) {
		return __add(std::move(key), value, true);
	}


	bool add(const Key& key, Value&& value) {
		return __add(key, std::move(value), true);
	}


	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> del(const Key& key) {
		return __del(key);
	}


	std::optional<std::reference_wrapper<KVStorageElement<Key, Value>>> del(Key&& key) {
		return __del(std::move(key));
	}


    std::optional<std::reference_wrapper<Value>> operator[](const Key &key) {
        using element_t = std::reference_wrapper<KVStorageElement<Key, Value>>;
        if (std::optional<element_t> kv_element = find(key); kv_element.has_value()) {
            return std::optional{ std::reference_wrapper{ kv_element.value().get().value } };
        }
        return std::nullopt;
	}

    std::vector<std::pair<Key *, Value *>> get_live_entries() const {
        std::vector<std::pair<Key *, Value *>> live_entries;
	    for (size_t i = 0; i < capacity(); i++) {
	        if (_data[i].exists && !_data[i].deleted) {
	            live_entries.emplace_back({ &_data[i].key, &_data[i].value });
	        }
	    }
	    return live_entries;
	}

    std::vector<Key *> get_live_entries_keys() const {
	    std::vector<Key *> live_entries;
	    for (size_t i = 0; i < capacity(); i++) {
	        if (_data[i].exists && !_data[i].deleted) {
	            live_entries.emplace_back(&_data[i].key);
	        }
	    }
	    return live_entries;
	}

    std::vector<Value *> get_live_entries_values() const {
	    std::vector<Value *> live_entries;
	    for (size_t i = 0; i < capacity(); i++) {
	        if (_data[i].exists && !_data[i].deleted) {
	            live_entries.emplace_back(&_data[i].value);
	        }
	    }
	    return live_entries;
	}
};