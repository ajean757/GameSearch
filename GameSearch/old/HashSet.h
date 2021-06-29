#pragma once

#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;


size_t name_hash(const string& name) {
	return hash<string>{}(name);
}
class HashSet {

	static constexpr double max_load_factor = 0.5;
	vector<set<string>> names;

	int size_;

public:
	HashSet() : size_(0) {
		names.resize(1);
	}

	
	int size() {
		return size_;
	}

	bool contains(const string& name) {
		const set<string>& subset = names[name_hash(name) % names.size()];
		return subset.find(name) != subset.end();
	}

	bool insert(const string& name) {
		++size_;
		if (size_ > names.size() * max_load_factor) {
			rehash(4 * names.size());
		}

		return names[name_hash(name) % names.size()].insert(name).second;
	}

	void rehash(int new_num_buckets) {
		cout << "rehash | new_size: " << new_num_buckets << endl;
		vector<set<string>> new_names;

		new_names.resize(new_num_buckets);
		for (const set<string>& bucket : names) {
			for (const string& name : bucket) {
				new_names[name_hash(name) % new_names.size()].insert(name);
			}
		}

		names = std::move(new_names);
	}
};