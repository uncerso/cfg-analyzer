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

CanonicVisitor::CanonicVisitor(NextStates const & states)
	: states(states)
{}

void CanonicVisitor::apply(StateList const & state){
	StateList::Data resList;

	for (auto const & x : state.list){
		x->visit(*this);
		if (!result) continue;
		if (auto ptr = dynamic_cast<StateList *>(result.get()))
			resList.merge(move(ptr->list));
		else
			resList.insert(move(result));
	}

	result.reset(new StateList(move(resList)));
}

void CanonicVisitor::apply(StateCall const & state){
	state.left->visit(*this);

	for (auto const & x : dynamic_cast<StateList *>(result.get())->list)
		if (auto ptr = dynamic_cast<StateFinal *>(x.get()))
			if (ptr->name == state.right->name){
				auto it = states.find(state.right->id);
				check(it != states.end(), "not enought rules");
				result.reset(it->second->clone());
				return;
			}
	result.reset(state.clone());
}

void CanonicVisitor::apply(StateFinal const & state){
	result.reset(state.clone());
}

void CanonicVisitor::apply(StateTerm const & state){
	result.reset(state.clone());
}

void CanonicVisitor::apply(StateNonTerm const & state){
	result.reset(state.clone());
}

unique_ptr<State> CanonicVisitor::getResult(){
	return move(result);
}
