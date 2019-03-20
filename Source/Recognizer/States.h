#pragma once
#include <memory>
#include <string>
#include <unordered_set>

class Visitor;

template<class Key>
struct CustomHash;

template<class T>
struct CustomHash<std::unique_ptr<T>>{
	size_t operator()(std::unique_ptr<T> const & v) const noexcept {return CustomHash<T*>()(v.get());}
};

class State{
public:
	virtual ~State() = default;

	virtual void visit(Visitor & visitor) const = 0;
	virtual State * clone() const = 0;

	size_t my_hash;
};

struct CustomEqual{
	using Elem = std::unique_ptr<State>;
	bool operator()(Elem const & lhs, Elem const & rhs) const noexcept;
};

class StateList : public State {
public:
	using Elem = CustomEqual::Elem;
	using Data = std::unordered_set<Elem, CustomHash<Elem>, CustomEqual>;

	StateList(Data && list);
	void visit(Visitor & visitor) const override;
	State * clone() const override;

//private:
	Data list;

private:
	StateList(Data && list, size_t my_hash);
};

class StateFinal : public State {
public:
	StateFinal(std::string && name);
	void visit(Visitor & visitor) const override;
	State * clone() const override;

//private:
	std::string name;
};

class StateNonTerm : public State {
public:
	StateNonTerm(int id, std::string && name);
	void visit(Visitor & visitor) const override;
	State * clone() const override;

//private:
	int id;
	std::string name;
};

class StateTerm : public State {
public:
	StateTerm(int id, std::string && name);
	void visit(Visitor & visitor) const override;
	State * clone() const override;

//private:
	int id;
	std::string name;
};

class StateCall : public State {
public:
	StateCall(StateList * left, StateNonTerm * right);
	void visit(Visitor & visitor) const override;
	State * clone() const override;

//private:
	std::unique_ptr<StateList> left;
	std::unique_ptr<StateNonTerm> right;

private:
	StateCall(StateList * left, StateNonTerm * right, size_t my_hash);
};

template<> 
struct CustomHash<State *>{
	size_t operator()(State * v) const noexcept{return v->my_hash;};
};

template<> 
struct CustomHash<StateList *>{
	size_t operator()(StateList * v) const noexcept{return v->my_hash;};
};

template<> 
struct CustomHash<StateFinal *>{
	size_t operator()(StateFinal * v) const noexcept{return v->my_hash;};
};

template<> 
struct CustomHash<StateNonTerm *>{
	size_t operator()(StateNonTerm * v) const noexcept{return v->my_hash;};
};

template<> 
struct CustomHash<StateTerm *>{
	size_t operator()(StateTerm * v) const noexcept{return v->my_hash;};
};

template<> 
struct CustomHash<StateCall *>{
	size_t operator()(StateCall * v) const noexcept{return v->my_hash;};
};
