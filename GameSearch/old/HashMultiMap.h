#pragma once

#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;





template<typename Key, typename Value>
class HashMultiMap {
	struct Bucket {
		Key key;
		Value value;
		Bucket(const Key& key, const Value& value) : key(key), value(value) {};
	};

	static constexpr double max_load_factor = 0.5;

public:
	vector<multimap<Key, Value>> buckets;
	int size_;

	size_t hashFunc(const Key& key) {
		return hash<Key>{}(key);
	}

public:
	HashMultiMap() : size_(0) {
		buckets.resize(1);
	}

	int size() {
		return size_;
	}

	bool contains(const Key& key) {
		const multimap<Key, Value>& subset = buckets[hashFunc(key) % buckets.size()];
		return subset.find(key) != subset.end(); // error occurs cuz searching a set of buckets for a key
	}

	vector<Value> get(const Key& key) {
		if (!contains(key)) {
			return -1;
		}
		vector<Value> result;
		const multimap<string, int>& subset = buckets[hashFunc(key) % buckets.size()];
		std::pair<std::multimap<string, int>::iterator, std::multimap<string, int>::iterator> range;
		range = subset.equal_range(key);
		for (multimap<string, int>::iterator it = range.first; it != range.second; ++it) {
			cout << it->second;
			result.push_back(it->second);
		}
		return result; // BIG PROBLEMS HERE, we need a better data structure or method to sture multiple arrays/values within a set
	}
	void insert(const Key& key, const Value& value) {
		++size_;
		if (size_ > buckets.size() * max_load_factor) {
			rehash(4 * buckets.size());
		}

		buckets[hashFunc(key) % buckets.size()].emplace(std::make_pair(key, value));
	}

	void rehash(int new_num_buckets) {
		vector<multimap<Key, Value>> new_buckets;

		new_buckets.resize(new_num_buckets);

		for (const multimap<Key, Value>& bucket : buckets) {
			for (const auto& i : bucket) {
				new_buckets[hashFunc(i.first) % new_buckets.size()].insert(std::make_pair(i.first, i.second));
			}
		}

		buckets = std::move(new_buckets);
	}

};