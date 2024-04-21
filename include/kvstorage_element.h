/* Key-value storage implemented by hashset with open addressing
* main file: KVStorage.hpp
 *
 * (C) V.Tarasov, Lavrentyev Institute of Hydrodynamics SB RAS, 2023
 *     tarvlad@inbox.ru
 */


template <
    typename Key,
    typename Value
>
struct KVStorageElement {
public:
    Key key;
    Value value;
    bool exists;
    bool deleted;

    KVStorageElement() :
        exists(false) {

    }


    KVStorageElement(const KVStorageElement<Key, Value>& e) :
        key(e.key),
        value(e.value),
        deleted(e.deleted),
        exists(e.exists) {}


    KVStorageElement(KVStorageElement<Key, Value>&& e) :
        key(std::move(e.key)),
        value(std::move(e.value)),
        deleted(e.deleted),
        exists(e.exists) {}


    KVStorageElement(const Key& k, const Value& v) :
        deleted(false),
        key(k),
        value(v),
        exists(true) {}

    KVStorageElement(const Key&& k, Value&& v) :
        deleted(false),
        key(std::move(k)),
        value(std::move(v)),
        exists(true) {}


    KVStorageElement(Key&& k, const Value& v) :
        deleted(false),
        key(std::move(k)),
        value(v),
        exists(true) {}


    KVStorageElement(const Key& k, Value&& v) :
        deleted(false),
        key(k),
        value(std::move(v)),
        exists(true) {}


    KVStorageElement& operator=(const KVStorageElement& e) {
        key = e.key;
        value = e.value;
        deleted = e.deleted;
        exists = e.exists;

        return *this;
    }


    KVStorageElement& operator=(KVStorageElement&& e) {
        key = std::move(e.key);
        value = std::move(e.value);
        deleted = e.deleted;
        exists = e.exists;

        return *this;
    }
};
