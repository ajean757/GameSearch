#pragma once
#include <algorithm>
#include <chrono>
#include <ctime>
#include "Date.h"
#include <fstream>
#include "GameDescriptors.h"
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using std::cerr;
using std::iostream;
using std::ifstream;
using std::map;
using std::multimap;
using std::pair;
using std::priority_queue;
using std::set;
using std::string;
using std::time;
using std::unordered_map;
using std::vector;

typedef unsigned int appid;
constexpr int LOAD_INTERVAL = 5000; // used for "progress bar" animation
const::string DATA_FILE = "steam_games_trimmed.csv";

struct SetContainer {
	set<appid> _set;
	size_t size;

	SetContainer(const set<appid>& s) : _set(s), size(s.size()) {};

	bool operator>(const SetContainer& rhs) const noexcept {
		return size > rhs.size;
	}
};

class GameLibrary {
	vector<Game> games;
	vector<string> descriptors; // if all searches were implemented, would be used to get title of every searchable element, however, some search items were ommited


	unordered_map<appid, string> name_lookup;

	pair<Date, Date> date_bounds; // Lower bound = .first upper bound = .second
	multimap<Date, appid> release_dates;

	vector<string> developers_list;
	unordered_map<string, vector<appid>> developers;

	vector<string> publishers_list;
	unordered_map<string, vector<appid>> publishers;

	vector<string> genre_types;
	unordered_map<string, vector<appid>> genres;

	multimap<int, appid> positive_reviews;
public:

	GameLibrary() {
		allocate_games();
		allocate_all_attributes();
	}

	set<appid> search_by_date(const string& begin_date, const string& end_date) {
		if (release_dates.size() == 0) {
			allocate_dates();
		}
		auto start = std::chrono::steady_clock::now();

		set<appid> result;
		multimap<Date, appid>::iterator iter, begin_iter, end_iter;

		

		begin_iter = std::move(find_minimum_valid_date(begin_date));
		end_iter = std::move(find_maximum_valid_date(end_date));

		for (iter = begin_iter; iter != end_iter; ++iter) {
			result.insert((*iter).second);
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search by date took: " << elapsed_seconds.count() << "s" << endl;
		return result;
	}

	set<appid> search_by_developer(const string& keyword) {
		if (developers.size() == 0) {
			allocate_developers();
		}

		auto start = std::chrono::steady_clock::now();

		set<appid> result;
		if (developers.find(keyword) == developers.end()) {
			cout << "No developers called \"" << keyword << "\" found!" << endl;
			return result;
		} 

		for (size_t i = 0; i < developers.at(keyword).size(); ++i) {
			result.insert(developers.at(keyword)[i]);
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search by developer took: " << elapsed_seconds.count() << "s" << endl;
		return result;
	}

	set<appid> search_by_publisher(const string& keyword) {
		if (publishers.size() == 0) {
			allocate_publishers();
		}
		auto start = std::chrono::steady_clock::now();

		set<appid> result;
		if (publishers.find(keyword) == publishers.end()) {
			cout << "No publishers called \"" << keyword << "\" found!" << endl;
			return result;
		}

		for (size_t i = 0; i < publishers.at(keyword).size(); ++i) {
			result.insert(publishers.at(keyword)[i]);
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search by publisher took: " << elapsed_seconds.count() << "s" << endl;
		return result;
	}

	set<appid> search_by_genre(const string& keyword) {
		if (genres.size() == 0) {
			allocate_genres();
		}
		auto start = std::chrono::steady_clock::now();

		set<appid> result;
		if (genres.find(keyword) == genres.end()) {
			cout << "No genres called \"" << keyword << "\" found!" << endl;
			return result;
		}
		
		for (size_t i = 0; i < genres.at(keyword).size(); ++i) {
			result.insert(genres.at(keyword)[i]);
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search by tags took: " << elapsed_seconds.count() << "s" << endl;

		return result;
	}

	set<appid> search_by_positive_reviews(const string& num_positive_reviews) {
		auto start = std::chrono::steady_clock::now();

		set<appid> result;

		int num_reviews = 0;
		try {
			num_reviews = stoi(num_positive_reviews);
		} 
		catch (exception& e) {
			cout << "Incorrect Parameters: " << e.what() << endl;
			return result;
		}

		// find nearest number of review, should be pretty quick
		while (positive_reviews.find(num_reviews) == positive_reviews.end()) {
			++num_reviews;
		}

		multimap<int, appid>::iterator lower_bound = positive_reviews.find(num_reviews);

		while (lower_bound != positive_reviews.end()) {
			result.insert(lower_bound->second);
			++lower_bound;
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search by reviews took: " << elapsed_seconds.count() << "s" << endl;

		return result ;
	}

	static set<appid> merge_n_sets(vector<set<appid>>& sets) {
		auto start = std::chrono::steady_clock::now();
		if (sets.size() == 1) {
			return sets[0];
		}

		size_t num_ops = 0;
		
		for (size_t i = 0; i < sets.size() - 1; ++i) {

			size_t smaller_set = (sets[i].size() < sets[i + 1].size()) ? i : i + 1;
			size_t larger_set = (sets[i].size() > sets[i + 1].size()) ? i : i + 1;

			for (auto iter = sets[smaller_set].begin(); iter != sets[smaller_set].end(); ) {
				if (sets[larger_set].count(*iter) == 0) {
					iter = sets[smaller_set].erase(iter); // after erasing, sets iter to next iter if it exists
				}
				else {
					++iter;
				}
				++num_ops;
			}

			if (smaller_set == i) {
				sets[larger_set]= std::move(sets[smaller_set]);
			}
		}

		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "[Merge Function] Took: " << elapsed_seconds.count() << "s to merge data" << endl;
		cout << "[Merge Function] # of Operations: " << num_ops << endl;

		return sets[sets.size() - 1];
	}

	static set<appid> merge_n_sets(priority_queue<SetContainer, vector<SetContainer>, std::greater<SetContainer>>& sets) {
		auto start = std::chrono::steady_clock::now();
		if (sets.size() == 1) {
			return sets.top()._set;
		}
		size_t num_ops = 0; 

		set<appid> smallest_set = sets.top()._set;
		sets.pop();
		while (!sets.empty()) {
			for (auto iter = smallest_set.begin(); iter != smallest_set.end(); ) {
				if (sets.top()._set.count(*iter) == 0) {
					iter = smallest_set.erase(iter);
				}
				else {
					++iter;
				}
				++num_ops;
			}
			auto start = std::chrono::steady_clock::now();

			sets.pop();  // this is performance slowdown, 
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			cout << "[Priority Queue Merge Function] Took: " << elapsed_seconds.count() << "s to pop()" << endl;

		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "[Priority Queue Merge Function] Took: " << elapsed_seconds.count() << "s to merge data" << endl;
		cout << "[Priority Queue Merge Function] # of Operations: " << num_ops << endl;

		return smallest_set;
	}

	// TODO: leads to some extra (unwanted) results when searched
	static set<appid> merge_n_sets_intersection(priority_queue<SetContainer, vector<SetContainer>, std::greater<SetContainer>>& sets) {
		if (sets.size() == 1) {
			return sets.top()._set;
		}

		set<appid> smallest_set = sets.top()._set;
		sets.pop();
		vector<appid> v;
		for (const auto& elem : smallest_set) {
			v.push_back(elem);
		}
		std::sort(v.begin(), v.end());
		auto start = std::chrono::steady_clock::now();

		vector<appid>::iterator it;
		while (!sets.empty()) {
			
			it = std::set_intersection(v.begin(), v.end(), sets.top()._set.begin(), sets.top()._set.end(), v.begin());
			sets.pop();
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "[Priority Queue Intersection Merge Function] Took: " << elapsed_seconds.count() << "s to merge data" << endl;

		v.resize(it - v.begin());
		set<appid> result;
		for (it = v.begin(); it != v.end(); ++it) {
			result.insert(*it);
		}
		
		return result;
	}


	string get_name(const appid& id) {
		if (name_lookup.size() == 0) {
			allocate_names();
		}

		if (name_lookup.find(id) == name_lookup.end()) {
			cout << "No games with id: \"" << id << "\" found!" << endl;
			return "[error]";
		}
		
		return name_lookup.at(id);
	}

	pair<Date, Date> get_date_bounds() const {
		return date_bounds;
	}

	vector<string> get_developers_list() const {
		return developers_list;
	}

	vector<string> get_publishers_list() const {
		return publishers_list;
	}

	vector<string> get_genre_types() const {
		return genre_types;
	}

private:

	void allocate_games() {
		auto start = std::chrono::steady_clock::now();
		ifstream steam_games(DATA_FILE); // hardcoded for ease of access, can fix later
		string line;

		getline(steam_games, line);
		descriptors == std::move(allocate_descriptors(line));

		int i = 0;
		cout << "Reading csv file and allocating games";
		while (getline(steam_games, line)) {
			games.push_back(Game(line));
			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

		cout << "Finished allocating games" << endl;

		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;

	}

	vector<string> allocate_descriptors(const string& line) {
		vector<string> result;
		string tmp = "";
		for (size_t i = 0; i <= line.length(); ++i) {
			if (i != line.length() && line[i] != ',') {
				tmp += line[i];
			}
			else {
				result.push_back(tmp);
			}
		}
		return result;
	}

	void allocate_all_attributes() {
		cout << "Allocating all attributes for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;

		// using this method to get min/max date is a ~0.05s performance hit
		priority_queue<Date, vector<Date>, std::greater<Date>>  min_date;
		priority_queue<Date> max_date;


		for (Game g : games) {
			name_lookup.emplace(g.get_id(), g.get_name());


			Date tmpdate(g.get_attributes()[2]);
			release_dates.emplace(tmpdate, g.get_id());
			min_date.push(tmpdate);
			max_date.push(tmpdate);

			vector<string> devs = std::move(Game::split_string(g.get_attributes()[3], ';'));
			for (size_t j = 0; j < devs.size(); ++j) {
				if (developers.count(devs[j]) == 1) {
					developers.at(devs[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					developers.emplace(devs[j], tmp);
					developers_list.push_back(devs[j]);
				}
			}

			vector<string> pubs = std::move(Game::split_string(g.get_attributes()[4], ';'));
			for (size_t j = 0; j < pubs.size(); ++j) {
				if (publishers.count(pubs[j]) == 1) {
					publishers.at(pubs[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					publishers.emplace(pubs[j], tmp);
					publishers_list.push_back(pubs[j]);
				}
			}

			vector<string> tags = std::move(Game::process_tags(g.get_attributes()[5]));
			for (size_t j = 0; j < tags.size(); ++j) {
				if (genres.count(tags[j]) == 1) {
					genres.at(tags[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					genres.emplace(tags[j], tmp);
					genre_types.push_back(tags[j]);
				}
			}

			positive_reviews.emplace(stoi(g.get_attributes()[6]), g.get_id());

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

		date_bounds.first = min_date.top();
		date_bounds.second = max_date.top();

		cout << "Finished allocating attributes" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}


	// Redudnant Method, used for "dynamic" allocation
	void allocate_names() {
		cout << "Allocating names set for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;

		for (Game g : games) {
			name_lookup.emplace(g.get_id(), g.get_name());

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

	
		cout << "Finished allocating names" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}

	// Redudnant Method, used for "dynamic" allocation
	void allocate_dates() {
		cout << "Allocating release dates set for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;


		// using this method to get min/max date is a ~0.05s performance hit
		priority_queue<Date, vector<Date>, std::greater<Date>>  min_date;
		priority_queue<Date> max_date;


		for (Game g : games) {
			
			Date tmpdate(g.get_attributes()[2]);

			release_dates.emplace(tmpdate, g.get_id());
			min_date.push(tmpdate);
			max_date.push(tmpdate);

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

		date_bounds.first = min_date.top();
		date_bounds.second = max_date.top();

		
		cout << "Finished allocating release dates" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}

	multimap<Date, appid>::iterator find_minimum_valid_date(const string& s_date) {
		Date date;
		try {
			date = Date(s_date);
		}
		catch (exception& e) {
			cout << "invalid date: " << e.what() << ". Will switch to default minimum date" << endl;
			return release_dates.find(date_bounds.first);
		}

		if (date < date_bounds.first) {
			cout << date.to_string() << " is an invalid date, mininum search date set to default earliest date: "
				<< date_bounds.first .to_string() << endl;
			date = date_bounds.first;
		}

		while (release_dates.find(date) == release_dates.end()) {
			++date;
		}

		return release_dates.find(date);
	}

	multimap<Date, appid>::iterator find_maximum_valid_date(const string& s_date) {
		Date date;

		try {
			date = Date(s_date);
		}
		catch (exception& e) {
			cout << "Invalid date: " << e.what() << ". Will switch to default maximum date" << endl;
			return release_dates.find(date_bounds.second);
		}

		if (date > date_bounds.second) {
			cout << date.to_string() << " is an invalid date, maximum search date set to default latest date: "
				<< date_bounds.second.to_string() << endl;
			date = date_bounds.second;
		}
		
		while (release_dates.find(date) == release_dates.end()) {
			--date;
		}

		return release_dates.find(date);
	}

	// Redudnant Method, used for "dynamic" allocation
	void allocate_developers() {
		cout << "Allocating developers set for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;

		for (Game g : games) {
			vector<string> devs = std::move(Game::split_string(g.get_attributes()[3], ';'));
			for (size_t j = 0; j < devs.size(); ++j) {	
				if (developers.count(devs[j]) == 1) {
					developers.at(devs[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					developers.emplace(devs[j], tmp);
					developers_list.push_back(devs[j]);
				}
			}

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}
		cout << "Finished allocating developers" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}

	// Redudnant Method, used for "dynamic" allocation
	void allocate_publishers() {
		cout << "Allocating publishers set for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;

		for (Game g : games) {
			vector<string> pubs = std::move(Game::split_string(g.get_attributes()[4], ';'));
			for (size_t j = 0; j < pubs.size(); ++j) {
				if (publishers.count(pubs[j]) == 1) {
					publishers.at(pubs[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					publishers.emplace(pubs[j], tmp);
					publishers_list.push_back(pubs[j]);
				}
			}

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

		cout << "Finished allocating publishers" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}

	// Redudnant Method, used for "dynamic" allocation
	void allocate_genres() {
		cout << "Allocating genres set for search";
		auto start = std::chrono::steady_clock::now();
		int i = 0;

		for (Game g : games) {
			vector<string> tags = std::move(Game::process_tags(g.get_attributes()[5]));

			for (size_t j = 0; j < tags.size(); ++j) {
				if (genres.count(tags[j]) == 1) {
					genres.at(tags[j]).push_back(g.get_id());
				}
				else {
					vector<appid> tmp = { g.get_id() };
					genres.emplace(tags[j], tmp);
					genre_types.push_back(tags[j]);
				}
			}

			++i;
			if (i % LOAD_INTERVAL == 0) { // "pretty animations"
				cout << ".";
			}
		}

		cout << "Finished allocating genres" << endl;
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "  Took: " << elapsed_seconds.count() << "s to store data" << endl;
	}


};