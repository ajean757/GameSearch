#include "Date.h"
#include "GameDescriptors.h"
#include "GameLibrary.h"

using std::cin;

void print_str_list(const vector<string>& list) {
	for (const string& str : list) {
		cout << str << ", ";
	} 
	cout << endl;

}
void display_search_terms(const GameLibrary& lib) {
	string input;
	int choice = 0;
	cout << "Would you like to view:" << endl;
	cout << "  1. Date Boundaries" << endl;
	cout << "  2. List of Developers (" << lib.get_developers_list().size() << " items)" << endl;
	cout << "  3. List of Publishers (" << lib.get_publishers_list().size() << " items)" << endl;
	cout << "  4. List of Genres (" << lib.get_genre_types().size() << " items)" << endl;
	cout << "Enter a number: ";


	try {
		cin >> input;
		choice = stoi(input);
	}
	catch (exception& e) {
		cout << "Error, invalid input: " << e.what() << endl;
	}

	while (choice < 1 || choice > 4) {
		cout << "Incorrect choice selected. Would you like to view:" << endl;
		cout << "  1. Date Boundaries" << endl;
		cout << "  2. List of Developers (" << lib.get_developers_list().size() << " items)" << endl;
		cout << "  3. List of Publishers (" << lib.get_publishers_list().size() << " items)" << endl;
		cout << "  4. List of Genres (" << lib.get_genre_types().size() << " items)" << endl;
		cout << "Enter a number: ";

		try {
			cin >> input;
			choice = stoi(input);
		}
		catch (exception& e) {
			cout << "Error, invalid input: " << e.what() << endl;
		}

	}
	switch (choice) {
	case 1: 
		cout << lib.get_date_bounds().first.to_string() << " and " << lib.get_date_bounds().second.to_string() << endl;
		break;
	case 2:
		cout << "List of Developers: ";
		print_str_list(lib.get_developers_list());
		break;
	case 3:
		cout << "List of Publishers: ";
		print_str_list(lib.get_publishers_list());
		break;
	case 4:
		cout << "List of Genres: ";
		print_str_list(lib.get_genre_types());
		break;
	}
}

void search_for_game(GameLibrary& lib) {
	vector<string> search_terms = {"  Date Bounds (enter two dates separated by a space, format yyyy-mm-dd): ", "  Developer: ", "  Publisher: ", "  Genre: ", "  Number of Positive Reviews: "};
	vector<string> user_input; // this will only have 4 elements
	string tmp;

	getline(cin, tmp); //clears previous cin or something, results in first item in search_terms being skipped if this line is deleted

	for (const string& term : search_terms) {
		cout << term;
		getline(cin, tmp);
		user_input.push_back(tmp);
	}

	//// Priority Queue method
	//priority_queue < SetContainer, vector<SetContainer>, std::greater<SetContainer>> search_sets;

	//// hard coded for simplicity, won't get out of bounds error since each item is either an empty string or a string
	//if (!user_input[0].empty()) {
	//	string date1 = user_input[0].substr(0, user_input[0].find(' '));
	//	string date2 = user_input[0].substr(user_input[0].find(' ')+1);
	//	search_sets.push(std::move(lib.search_by_date(date1, date2)));
	//}
	//if (!user_input[1].empty()) {
	//	search_sets.push(std::move(lib.search_by_developer(user_input[1])));

	//}
	//if (!user_input[2].empty()) {
	//	search_sets.push(std::move(lib.search_by_publisher(user_input[2])));
	//}
	//if (!user_input[3].empty()) {
	//	search_sets.push(std::move(lib.search_by_genre(user_input[3])));
	//}
	//if (!user_input[4].empty()) {
	//	search_sets.push(std::move(lib.search_by_positive_reviews(user_input[4])));
	//}



	// Vector method
	vector<set<appid>> search_sets;
	// hard coded for simplicity, won't get out of bounds error since each item is either an empty string or a string
	if (!user_input[0].empty()) {
		string date1 = user_input[0].substr(0, user_input[0].find(' '));
		string date2 = user_input[0].substr(user_input[0].find(' ') + 1);
		search_sets.push_back(std::move(lib.search_by_date(date1, date2)));
	}
	if (!user_input[1].empty()) {
		search_sets.push_back(std::move(lib.search_by_developer(user_input[1])));

	}
	if (!user_input[2].empty()) {
		search_sets.push_back(std::move(lib.search_by_publisher(user_input[2])));
	}
	if (!user_input[3].empty()) {
		search_sets.push_back(std::move(lib.search_by_genre(user_input[3])));
	}
	if (!user_input[4].empty()) {
		search_sets.push_back(std::move(lib.search_by_positive_reviews(user_input[4])));
	}

	set<appid> union_set;
	union_set = std::move(GameLibrary::merge_n_sets(search_sets));

	for (const auto& elem : union_set) {
		cout << lib.get_name(elem) << endl;
	}

}


int main() {
	cout << "Welcome to Steam Game Search" << endl;

	GameLibrary library;

	int choice = 0;
	string input;
	do {
		cout << "  1. Search for a game" << endl;
		cout << "  2. View possible search terms" << endl;
		cout << "  3. Exit" << endl;
		cout << "Enter a number: ";
		try {
			cin >> input;
			choice = stoi(input);
		}
		catch (exception& e) {
			cout << "Error, invalid input: " << e.what() << endl;
		}

		while (choice < 1 || choice > 3) {
			cout << "Incorrect choice selected. Would you like to 1. search for a game or 2. view possible search terms? (enter 1 or 2): ";
			try {
				cin >> input;
				choice = stoi(input);
			}
			catch (exception& e) {
				cout << "Error, invalid input: " << e.what() << endl;
			}
		}

		if (choice == 1) {
			cout << "Here are some possible queries, press \"enter\" to skip search term" << endl;
			search_for_game(library);
		}
		else if (choice == 2) {
			display_search_terms(library);
		}

	} while (choice != 3);
	
}
