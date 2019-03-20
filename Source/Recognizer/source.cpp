#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <sstream>
#include <stack>
#include <iterator>
#include "States.h"
#include "Visitors.h"
#include <unordered_map>

using namespace std; 

template<class ...Args>
inline void check(bool value, Args const & ... msg){
	if (!value){
		(cerr << ... << msg) << '\n';
		exit(0);
	}
}

State * makeState(string && s){
	check(s.size() >= 3, "incorect input");
	if (s[0] == 'F' && s[1] == '[' && s.back() == ']')
		return new StateFinal(s.substr(2, s.size() - 3));
	auto pos = s.find_last_of('-');
	check(pos != string::npos, "incorect string");
	State * ans;
	try{
		if (s[0] == 'S' && s[1] == '[' && s.back() == ']')
			ans = new StateNonTerm(stoi(s.substr(pos + 1, s.size() - pos - 2)), s.substr(2, pos - 2));
		else
			ans = new StateTerm(stoi(s.substr(pos + 1, s.size() - pos - 1)), s.substr(0, pos));
	}
	catch(std::invalid_argument & err) {check(false, "incorect input");}
	catch(std::out_of_range & err) {check(false, "too big int");}
	return ans;
}

State * parse(string::const_iterator inp, string::const_iterator const & end){
	stack<State *> vars;
	stack<char> ops;
	for (; inp != end; ++inp){
		char c = *inp;
		if (c == ' ') continue;
		if (c == '{' || c == '(' || c == ',') 
			ops.push(c);
		else if (c == ')'){
			check(vars.size() >= 2, "incorect input");
			check(ops.top() == ',', "incorect input");
			ops.pop();
			check(ops.top() == '(', "incorect input");
			ops.pop();

			auto right = dynamic_cast<StateNonTerm *>(vars.top()); vars.pop();
			auto left = dynamic_cast<StateList *>(vars.top()); vars.pop();
			check(right && left, "incorect input");
			vars.push(new StateCall(left, right));
		} 
		else if (c == '}'){
			check(!vars.empty() && !ops.empty(), "incorect input");
			StateList::Data lst;
			lst.emplace(vars.top()); vars.pop();
			while (ops.top() != '{'){
				check(ops.top() == ',' && !vars.empty(), "incorect input");
				lst.emplace(vars.top()); vars.pop(); ops.pop();
				check(!ops.empty(), "incorect input");
			}
			ops.pop();
			vars.push(new StateList(move(lst)));
		} 
		else{
			stringstream ss;
			for (; inp != end && *inp != ',' && *inp != '}' && *inp != ')'&& *inp != ' '; ++inp)
				ss << *inp;
			--inp;
			vars.push(makeState(ss.str()));
		}
	}
	
	check(vars.size() == 1, "incorect input");
	return vars.top();
}

string iterCopy(string::const_iterator inp, string::const_iterator const & end){
	stringstream ss;
	for (; inp != end; ++inp)
		ss << *inp;
	return ss.str();
}

int main(){
	int n;
	string tmp;
	list<string> text;
	cin >> n;
	for (int i = 0; i < n; ++i) {
		check((bool)(cin >> tmp), "incorect input");
		text.push_back(move(tmp));
	}

	getline(cin, tmp);
	while(tmp.empty() && getline(cin, tmp));

	check(tmp.size() >= 11 && tmp[0] == 'S' && tmp[1] == 'E' && tmp[2] == '[', "incorect input");
	auto it = find(begin(tmp), end(tmp), '=');
	check(it != tmp.end(), "incorect string: ", tmp);
	auto itStart = tmp.begin() + 3;
	auto itEnd = find(itStart, it, ']'); //LAST
	check(itStart != itEnd, "incorect string: ", tmp);

	auto nameOfInitState = iterCopy(itStart, itEnd);
	unique_ptr<State> expression(parse(it + 1, end(tmp)));

	list<string> data;
	while (getline(cin, tmp)) data.push_back(move(tmp));
	
	unordered_multimap<int, unique_ptr<State> > states;
	for (auto const & x : data){
		check(x.size() >= 12 && x[0] == 'S' && x[1] == '[', "incorect input: ", x);
		auto it = find(begin(x), end(x), '=');
		check(it != x.end(), "incorect string: ", x);
		auto itStart = x.begin() + 2;
		auto itEnd = find(itStart, it, ']'); //LAST
		check(itStart != itEnd, "incorect string: ", x);
		auto itSep = find(itStart, itEnd, '-'); //LAST
		check(itStart != itSep, "incorect string: ", x);
		try { states.emplace(stoi(iterCopy(itSep+1, itEnd)), parse(it + 1, end(x))); }
		catch(std::invalid_argument & err) {check(false, "incorect input: ", x);}
		catch(std::out_of_range & err) {check(false, "too big int: ", x);}
	}
	
	PrintVisitor pv;
	SectionVisitor sv(states);
	
	expression->visit(pv);
	cout << "S0 = " << pv.getResult() << '\n';
	pv.reset();

	int cnt = 0;
	for (auto const & x : text){
		sv.setSectionString(x);

		expression->visit(sv);
		expression = sv.getResult();
		if (!expression) {
			cout << "No\n";
			return 0;
		}

		expression->visit(pv);
		cout << 'S' << cnt + 1 << " = " << 'S' << cnt << '/' << x << " = " << pv.getResult() << '\n';
		pv.reset();
		++cnt;
	}

	CanonicVisitor cv(states);
	expression->visit(cv);
	expression = cv.getResult();
	expression->visit(pv);
	cout << 'S' << cnt << " = " << pv.getResult() << '\n';

	for (auto const & x : dynamic_cast<StateList *>(expression.get())->list)
		if (auto ptr = dynamic_cast<StateFinal *>(x.get()))
			if (ptr->name == nameOfInitState){
				cout << "Yes\n";
				return 0;
			}
	cout << "No\n";
	return 0;
}