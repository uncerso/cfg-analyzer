#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <variant>
#include <unordered_map>
#include <filesystem>

using namespace std; 
using StrOrInt = variant<string, int>;

template<class ...Args>
void check(bool value, Args const & ... msg) {
	if (!value) {
		ofstream outp("ERROR_LOG.txt");
		(outp << ... << msg);
		outp.flush();
		exit(0);
	}
}

inline bool isSpecSymbol(string const & s){
	return s == "<" || s == "goto" || s == "*";
} 

inline bool isEndOfRule(string const & s){
	return s[0] == '.';
}

inline bool isTerminal(string const & s){
	return !(isSpecSymbol(s) || isEndOfRule(s));
}

inline string strTail(string const & s){
	return s.substr(1, s.size() - 1);
}

string getNextState(vector<StrOrInt> const & data, unordered_map<int, string> const & namesOfrules, int pos, int recursionLever = 100){
	check(recursionLever > 0, "recursion in rules detected\n");
	static unordered_map<int, string> cache;
	auto const & s = get<string>(data[pos]);
	if (isTerminal(s)) return s + '-' + to_string(pos);
	if (auto it = cache.find(pos); it != cache.end()) return it->second;
	//cerr << cache.size() << '\n';
	if (isEndOfRule(s)) return "F["s + strTail(s)+']';
	if (s == "*") 
		return 	"({"s + 
				getNextState(data, namesOfrules, get<int>(data[pos + 1]), recursionLever - 1) + 
				"}, S["s + 
				namesOfrules.find(get<int>(data[pos + 1]))->second +
				'-' + to_string(pos) +
				"])";
	if (s == "<") 
		return 	cache[pos] = getNextState(data, namesOfrules, pos + 2, recursionLever) + 
				", " +
			 	getNextState(data, namesOfrules, get<int>(data[pos + 1]), recursionLever);
	if (s == "goto")
		return 	getNextState(data, namesOfrules, get<int>(data[pos + 1]), recursionLever);
	return "~!~";
}

list<string> solve(vector<StrOrInt> const & data, unordered_map<int, string> const & namesOfrules){
	list<string> ans = {"SE["s + namesOfrules.find(0)->second+ "] = {"s + getNextState(data, namesOfrules, 0) + '}'};
	
	for (int pos = 0; pos < data.size(); ++pos){
		auto ps = get_if<string>(&data[pos]);
		if (ps == nullptr) continue;
		auto const & s = *ps;
		if (isTerminal(s))
			ans.push_back("S["s + s + '-' + to_string(pos) + "] = {" + getNextState(data, namesOfrules, pos + 1) + '}');
		else if (s == "*") {
			auto && tmp = namesOfrules.find(get<int>(data[pos + 1]))->second; 
			ans.push_back("S["s + move(tmp) + '-' + to_string(pos) + "] = {" + getNextState(data, namesOfrules, (pos + 1) + 1) + '}');
		}
	}

	return ans;
}

int main(int argc, char * argv[]){
	cout << "Enter file name: ";
	cout.flush();
	string name;
	cin >> name;

	ifstream inp(name);
	while (!inp.is_open()) {
		cout << "File " << name << " not found, try again:" << endl;
		cin >> name;
		inp.open(name);
	}


	vector<StrOrInt> data;
	string tmp;
	for (int i=0; i<10; ++i) inp >> tmp; //skip 10 integ
	int cnt = 0;
	while (inp >> tmp) {
		if (cnt == 11) cnt = 0;
		if (cnt != 0)
			data.push_back(tmp);
		++cnt;
	}

	check(!data.empty() && isEndOfRule(get<string>(data.back())), "incorect input\n");

	try{
		bool nextValueShouldBeInt = false;
		for (auto & x : data){
			if (nextValueShouldBeInt){
				x = stoi(get<string>(x));
				nextValueShouldBeInt = false;
			}
			else{
				auto const & s = get<string>(x); 
				nextValueShouldBeInt = isSpecSymbol(s);
			}
		}
	} 
	catch(std::invalid_argument & err) {check(false, "incorect input\n");}
	catch(std::out_of_range & err) {check(false, "too big int\n");}

	unordered_map<int, string> namesOfrules; // start index of rule -> name of rule
	int startIndexOfRule = 0;
	for (int pos = 1; pos < data.size(); ++pos){
		auto s = get_if<string>(&data[pos]);
		if (s && isEndOfRule(*s)){
			namesOfrules.emplace(startIndexOfRule, strTail(*s));
			startIndexOfRule = pos + 1;
		}
	}

	filesystem::path p(filesystem::current_path());

	string ext;
	if (int pos = name.find_last_of('.'); pos != string::npos) {
		ext = name.substr(pos);
		name = name.substr(0, pos);
	}
	name += "-out";
	int fileNum = 0;
	while (filesystem::exists(p / (name + to_string(fileNum) + ext))) ++fileNum;
	ofstream outp(name + to_string(fileNum) + ext);

	for (auto && x : solve(data, namesOfrules))
		outp << x << '\n';
	return 0;
}