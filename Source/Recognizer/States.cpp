#include "Visitors.h"
#include "States.h"

using namespace std;

StateList::StateList(Data && list)
	: list(move(list))
{
	auto it = this->list.begin();
	my_hash = (*it)->my_hash + 0x9e3779b9;
	for (++it; it!=this->list.end(); ++it)
		my_hash^= (*it)->my_hash + 0x9e3779b9 + (my_hash<<6) + (my_hash>>2);
}

StateList::StateList(Data && list, size_t my_hash)
	: list(move(list))
{
	this->my_hash = my_hash;
}

StateCall::StateCall(StateList * left, StateNonTerm * right)
	: left(left)
	, right(right)
{
	my_hash = left->my_hash;
	my_hash ^= right->my_hash + 0x9e3779b9 + (my_hash<<6) + (my_hash>>2);
}

StateCall::StateCall(StateList * left, StateNonTerm * right, size_t my_hash)
	: left(left)
	, right(right)
{
	this->my_hash = my_hash;
}

StateNonTerm::StateNonTerm(int id, string && name)
	: id(id)
	, name(move(name))
{
	my_hash = id;
}

StateTerm::StateTerm(int id, string && name)
	: id(id)
	, name(move(name))
{
	my_hash = id;
}

StateFinal::StateFinal(string && name)
	: name(move(name))
{
	my_hash = std::hash<std::string>()(this->name);
}

void StateList::visit(Visitor & visitor) 	const { visitor.apply(*this); }
void StateCall::visit(Visitor & visitor) 	const { visitor.apply(*this); }
void StateFinal::visit(Visitor & visitor) 	const { visitor.apply(*this); }
void StateNonTerm::visit(Visitor & visitor) const { visitor.apply(*this); }
void StateTerm::visit(Visitor & visitor) 	const { visitor.apply(*this); }

State * StateList::clone() const {
	Data copyList;
	copyList.rehash(list.bucket_count());

	for (auto const & x : list)
		copyList.emplace(x->clone());

	return new StateList(move(copyList), my_hash);
}

State * StateCall::clone() const {
	return new StateCall(
		dynamic_cast<StateList * >	(left->clone()),
		dynamic_cast<StateNonTerm *>(right->clone()),
		my_hash
		);
}

State * StateFinal::clone() const {
	return new StateFinal(*this);
}

State * StateTerm::clone() const {
	return new StateTerm(*this);
}

State * StateNonTerm::clone() const {
	return new StateNonTerm(*this);
}

bool CustomEqual::operator()(Elem const & lhs, Elem const & rhs) const noexcept{
	if (auto pl = dynamic_cast<StateList *>(lhs.get()))
		if (auto pr = dynamic_cast<StateList *>(rhs.get()))
		return pl->list == pr->list;
		else return false;

	if (auto pl = dynamic_cast<StateCall *>(lhs.get()))
		if (auto pr = dynamic_cast<StateCall *>(rhs.get()))
		return pl->right->id == pr->right->id && pl->left->list == pr->left->list;
		else return false;

	if (auto pl = dynamic_cast<StateTerm *>(lhs.get()))
		if (auto pr = dynamic_cast<StateTerm *>(rhs.get()))
		return pl->id == pr->id;
		else return false;

	if (auto pl = dynamic_cast<StateNonTerm *>(lhs.get()))
		if (auto pr = dynamic_cast<StateNonTerm *>(rhs.get()))
		return pl->id == pr->id;
		else return false;

	if (auto pl = dynamic_cast<StateFinal *>(lhs.get()))
		if (auto pr = dynamic_cast<StateFinal *>(rhs.get()))
		return pl->my_hash == pr->my_hash;
		else return false;
	
	return true;
}