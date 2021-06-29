#pragma once

#include <string>
#include <utility>
#include <vector>

using std::move;
using std::string;
using std::vector;


class Game {
	unsigned int id;
	string name; //TODO: issue with special characters
	vector<string> attributes;

public:


	Game(string data) {
		attributes = std::move(split_string(data));
		id = stoi(attributes[0]); 
		name = attributes[1];
	}

	unsigned int get_id() const {
		return id;
	}

	string get_name() const {
		return name;
	}

	vector<string> get_attributes() const {
		return attributes;
	}
	
	size_t num_attributes() const {
		return attributes.size();
	}

	//static helper methods

	static vector<string> split_string(const string& data, const char& delimeter = ',') {
		bool in_quotes = false;
		vector<string> result;
		string tmp = "";
		for (size_t i = 0; i <= data.length(); ++i) { // using <= to save the last tag
			if (i != data.length() && data[i] != ',' && data[i] != '\"' && !in_quotes) {
				tmp += data[i];    // add char if it's not a ',' delimiter or in  
			}
			else if (data[i] == '\"') {
				in_quotes = !in_quotes;    // toggle special behavior for an element in quotes
			}
			else if (in_quotes) {
				tmp += data[i];    // if an element is surrounded by quotes, add every element even if it's a ',' 
			}
			else {
				result.push_back(tmp);    // if the element is a ',' or we reached the end of the line, add element
				tmp = "";					// to attributes array and reset the string
			}
		}
		return result;
	}


	static vector<string> process_tags(const string& line) {
		string trimed_line = trim_entries(line, ',', 5);
		string tags = trimed_line.substr(0, trimed_line.find(','));
		vector<string> out;
		size_t num_tags = count(tags.begin(), tags.end(), ';') + 1;

		string tmp = "";
		for (size_t i = 0; i <= tags.length(); ++i) { // using <= to save the last tag
			if ((i != tags.length()) && tags[i] != ';') {
				tmp += tags[i];
			}
			else {
				out.push_back(tmp);
				tmp.clear();
			}
		}
		return out;
	}

};