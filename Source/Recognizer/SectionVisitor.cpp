#include "Visitors.h"
#include <iostream>

using namespace std;

template<class ...Args>
inline void check(bool value, Args const & ... msg){
	if (!value){
		(cerr << ... << msg) << '\n';
		exit(0);
	}
}

SectionVisitor::SectionVisitor(NextStates const & states)
	: states(states)
{}

void SectionVisitor::apply(StateList const & state){
	StateList::Data resList;

	for (auto const & x : state.list){
		x->visit(*this);
		if (!result) continue;
		if (auto ptr = dynamic_cast<StateList *>(result.get()))
			resList.merge(move(ptr->list));
		else
			resList.insert(move(result));
	}
	if (resList.empty()) result = nullptr;
	else result.reset(new StateList(move(resList)));
}

void SectionVisitor::apply(StateCall const & state){
	state.left->visit(*this);
	if (result) {
		result.reset(
			new StateCall(
				dynamic_cast<StateList *>(result.release()), 
				dynamic_cast<StateNonTerm *>(state.right->clone())
			)
		);
		return;
	}

	for (auto const & x : state.left->list)
		if (auto ptr = dynamic_cast<StateFinal *>(x.get()))
			if (ptr->name == state.right->name){
				state.right->visit(*this);
				return;
			}
	result = nullptr;
}

void SectionVisitor::apply(StateFinal const & state){
	result = nullptr;
}

void SectionVisitor::apply(StateTerm const & state){
	if (state.name != s) {result = nullptr; return;}
	auto it = states.find(state.id);
	check(it != states.end(), "not enought rules");
	result.reset(it->second->clone());
}

void SectionVisitor::apply(StateNonTerm const & state){
	auto it = states.find(state.id);
	check(it != states.end(), "not enought rules");
	it->second->visit(*this);
}

unique_ptr<State> SectionVisitor::getResult(){
	return move(result);
}

void SectionVisitor::setSectionString(std::string const & sectionString){
	s = sectionString;
}
