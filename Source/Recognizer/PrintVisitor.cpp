#include "Visitors.h"

using namespace std;

void PrintVisitor::apply(StateList const & state){
	ss << '{';
	(*state.list.begin())->visit(*this);
	auto it = state.list.begin();
	++it;
	for (; it != state.list.end(); ++it){
		ss << ", ";
		(*it)->visit(*this);
	}
	ss << '}';
}

void PrintVisitor::apply(StateCall const & state){
	ss << '(';
	state.left->visit(*this);
	ss << ", ";
	state.right->visit(*this);
	ss << ')';
}

void PrintVisitor::apply(StateFinal const & state){
	ss << "F[" << state.name << ']';
}

void PrintVisitor::apply(StateTerm const & state){
	ss << state.name << '-' << state.id;
}

void PrintVisitor::apply(StateNonTerm const & state){
	ss << "S[" << state.name << '-' << state.id << ']';
}

string PrintVisitor::getResult(){
	return ss.str();
}

void PrintVisitor::reset(){
	ss.str("");
}
