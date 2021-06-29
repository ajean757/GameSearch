#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;





template<typename Key, typename Value>
class HashMap {
	struct Bucket {
		Key key;
		Value value;
		Bucket(const Key& key, const Value& value) : key(key), value(value) {};
	};

	static constexpr double max_load_factor = 0.5;


	vector<map<Key, Value>> buckets;
	int size_;

	size_t hashFunc(const Key& key) {
		return hash<Key>{}(key);
	}

public:
	HashMap() : size_(0) {
		buckets.resize(1);
	}

	int size() {
		return size_;
	}

	bool contains(const Key& key) {
		const map<Key, Value>& subset = buckets[hashFunc(key) % buckets.size()];
		return subset.find(key) != subset.end(); // error occurs cuz searching a set of buckets for a key
	}

	Value get(const Key& key) {
		if (!contains(key)) {
			return -1;
		}

		const map<Key, Value>& subset = buckets[hashFunc(key) % buckets.size()];
		return subset.at(key);
	}
	bool insert(const Key& key, const Value& value) {
		++size_;
		if (size_ > buckets.size() * max_load_factor) {
			rehash(4 * buckets.size());
		}

		return buckets[hashFunc(key) % buckets.size()].emplace(std::make_pair(key, value)).second;
	}

	void rehash(int new_num_buckets) {
		vector<map<Key, Value>> new_buckets;

		new_buckets.resize(new_num_buckets);

		for (const map<Key, Value>& bucket : buckets) {
			for (const auto& i : bucket) {
				new_buckets[hashFunc(i.first) % new_buckets.size()].insert(std::make_pair(i.first, i.second));
			}
		}

		buckets = std::move(new_buckets);
	}

};