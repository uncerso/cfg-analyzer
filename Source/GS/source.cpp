#include <iostream>
#include <iomanip>
#include <string>
#include <stack>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <filesystem>
#include "States.h"

#ifndef __linux__
	#include <fstream>
#endif

using namespace std;

template<class ...Args>
void check(bool value, Args const & ... msg) {
	if (!value) {
		#ifdef __linux__
			auto & outp = cerr;
		#else
			ofstream outp("ERROR_LOG.txt");
		#endif
		(outp<< ... << msg);
		outp.flush();
		exit(0);
	}
}

State * compile(string const & s, unordered_map<string, int> const & declaredFuncs) {
	stack<State *> vars;
	stack<char> ops;
	auto const applyOp = [&vars](char op) {
		check(vars.size() > 1, "incorrect sequence\n");
		auto right = vars.top(); vars.pop();
		auto left = vars.top(); vars.pop();
		vars.push(makeState(op, left, right));
	};

	for (int pos = 0; pos < s.size(); ++pos) {
		if (s[pos] == '(') ops.push('(');
		else if (s[pos] == ')') {
			check(!ops.empty(), "incorrect brackets sequence\n");
			while (ops.top() != '(') {
				applyOp(ops.top()), ops.pop();
				check(!ops.empty(), "incorrect brackets sequence\n");
			}
			ops.pop();
		}
		else if (isOperation(s[pos])) {
			char const curOp = s[pos];
			while (!ops.empty() && (priority(ops.top()) >= priority(curOp)))
				applyOp(ops.top()), ops.pop();
			ops.push(curOp);
		}
		else {
			int startPos = pos;
			bool isWord = false;
			while (pos < s.size() && (isWord || (!isOperation(s[pos]) && s[pos] != ')' /*&& s[pos] != '('*/))) {
				if (s[pos] == '\'') isWord = !isWord;
				++pos;
			}
			auto tmp = s.substr(startPos, pos - startPos);
			--pos;
			if (declaredFuncs.find(tmp) != declaredFuncs.end())
				vars.push(new StateNonTerm(move(tmp)));
			else
				vars.push(new StateTerm(move(tmp)));
		}
	}

	while (!ops.empty())
		applyOp(ops.top()), ops.pop();
	check(vars.size() == 1, "incorrect sequence\n");
	return vars.top();
}

list<string> link(unordered_map<string, int> &declaredFuncs, vector<string> const & names, vector<State*> const & objs) {
	assert(declaredFuncs.size() == names.size() && names.size() == objs.size());

	list<string> res;
	int pos = res.size();
	for (int i = 0; i < names.size(); ++i) {
		declaredFuncs[names[i]] = pos;
		auto && text = objs[i]->makeText(pos);
		pos += text.size() + 1;
		res.splice(end(res), move(text));
		res.push_back("." + names[i]);
	}

	bool func = false;
	for (auto &x : res) {
		if (func) {
			x = to_string(declaredFuncs[x]);
			func = false;
		}
		else if (x == "*") func = true;
	}

	assert(!func);
	return res;
}

string readGram(istream & inp){
	stringstream ss;
	char c = inp.get();
	while(c == ' ' || c == '\n' || c == '\r' || c == '\t') 
		c = inp.get();
	
	if (c != ':' && c != '.')
		ss << c;

	c = inp.get();
	while(c != '.') {
		if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
			ss << c;
		check(!inp.eof(), "Missed . in the rules.\n");
		c = inp.get();
	}
	return ss.str();
}

int main(int argc, char * argv[]) {
#ifdef __linux__
	auto & inp = cin;
#else
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
#endif

	int n;
	inp >> n;
	vector<string> texts(n);
	vector<string> names(n);
	vector<State*> objs;
	unordered_map<string, int> declaredFuncs;
	for (int i = 0; i < n; ++i) {
		inp >> names[i];
		if (names[i].back() == ':') names[i].pop_back();
		auto const & s = names[i];
		check(declaredFuncs.find(s) == declaredFuncs.end(), s, " already declared!\n");
		declaredFuncs.emplace(s, -1);
		texts[i] = readGram(inp);
	}
	for (auto &x : texts) objs.push_back(compile(x, declaredFuncs));
	auto program = link(declaredFuncs, names, objs);
	for (auto x : objs) delete x;


#ifdef __linux__
	auto & outp = cout;
#else
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
#endif

	int pos = 0, cnt = 0, corruptedSpace = 0;
	constexpr auto intend = 8;
	outp << "  ";
	for (int i = 0; i < 10; ++i) outp << "  " << setw(intend) << i;
	for (auto &x : program) {
		if (pos == 0) {
			outp << '\n' << setw(2) << cnt;
			corruptedSpace = 0;
		}
		outp << "  " << setw(intend - corruptedSpace) << x;
		corruptedSpace = max(0, static_cast<int>(x.size() - intend + corruptedSpace));
		++pos;
		if (pos == 10) pos = 0, ++cnt;
	}
	outp << '\n';
	return 0;
}