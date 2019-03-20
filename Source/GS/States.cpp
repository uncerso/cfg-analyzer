#include "States.h"

#ifdef __linux__
	#include <iostream>
#else
	#include <fstream>
#endif

using namespace std;

State * makeState(char op, State * left, State * right){
	State * res;
	switch (op){
		case ',': res = new StateConc(left, right); break;
		case ';': res = new StateUnion(left, right); break;
		case '#': res = new StateIter(left, right); break;
		default: 
			#ifdef __linux__
				auto & outp = cerr;
			#else
				ofstream outp("ERROR_LOG.txt");
			#endif
			outp << "unknown operation: " << op << '\n';
			outp.flush(); 
			exit(0);
	}
	return res;
}

int priority(char op){
    if (op == '#') return 4;
    if (op == ',') return 3;
    if (op == ';') return 2;
    // if (op == '*' || op == '+') return 1;
    return -1;
}

bool isOperation(char op){ return op == ',' || op == ';' || op == '#';}

StateTerm::StateTerm(string && s) : s(move(s)) {}

StateNonTerm::StateNonTerm(string && s) : s(move(s)) {}

StateConc::StateConc(State * left, State * right) 
    : left(left)
    , right(right)
{}

StateUnion::StateUnion(State * left, State * right) 
    : left(left)
    , right(right)
{}

StateIter::StateIter(State * left, State * right) 
    : left(left)
    , right(right)
{}

list<string> StateTerm::makeText(int startPos){ return {s}; }

list<string> StateNonTerm::makeText(int startPos){ return {"*", s}; }

list<string> StateConc::makeText(int startPos){ 
    auto res = left->makeText(startPos);
    res.splice(end(res), right->makeText(startPos + res.size()));
    return res;
}

list<string> StateUnion::makeText(int startPos){
    list<string> res = {"<"};
    auto tmp = left->makeText(startPos + 2);
    res.push_back(to_string(startPos + 4 + tmp.size()));
    startPos += 4 + tmp.size();
    res.splice(end(res), tmp);
    res.emplace_back("goto");
    tmp = right->makeText(startPos);
    res.push_back(to_string(startPos + tmp.size()));
    res.splice(end(res), tmp);
    return res;
}

list<string> StateIter::makeText(int startPos){
    auto res = left->makeText(startPos);
    res.emplace_back("<");
    auto tmp = right->makeText(startPos + res.size() + 1);
    res.push_back(to_string(startPos + res.size() + 3 + tmp.size()));
    res.splice(end(res), tmp);
    res.emplace_back("goto");
    res.push_back(to_string(startPos));
    return res;
}