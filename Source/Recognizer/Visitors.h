#pragma once
#include "States.h"
#include <sstream>
#include <string>
#include <unordered_map>

class Visitor{
public:
	virtual ~Visitor() = default;

	virtual void apply(StateList const & state) = 0;
	virtual void apply(StateCall const & state) = 0;
	virtual void apply(StateFinal const & state) = 0;
	virtual void apply(StateTerm const & state) = 0;
	virtual void apply(StateNonTerm const & state) = 0;
};

class PrintVisitor : public Visitor{
public:
	void apply(StateList const & state) override;
	void apply(StateCall const & state) override;
	void apply(StateFinal const & state) override;
	void apply(StateTerm const & state) override;
	void apply(StateNonTerm const & state) override;
	
	std::string getResult();
	void reset();
private:
	std::stringstream ss;
};

class SectionVisitor : public Visitor{
	using NextStates = std::unordered_multimap<int, std::unique_ptr<State> >;
public:
	SectionVisitor(NextStates const & states);

	void setSectionString(std::string const & sectionString);

	void apply(StateList const & state) override;
	void apply(StateCall const & state) override;
	void apply(StateFinal const & state) override;
	void apply(StateTerm const & state) override;
	void apply(StateNonTerm const & state) override;
	
	std::unique_ptr<State> getResult();
private:
	std::unique_ptr<State> result;
	NextStates const & states;
	std::string s;
};

class CanonicVisitor : public Visitor{
	using NextStates = std::unordered_multimap<int, std::unique_ptr<State> >;
public:
	CanonicVisitor(NextStates const & states);

	void apply(StateList const & state) override;
	void apply(StateCall const & state) override;
	void apply(StateFinal const & state) override;
	void apply(StateTerm const & state) override;
	void apply(StateNonTerm const & state) override;
	
	std::unique_ptr<State> getResult();
private:
	std::unique_ptr<State> result;
	NextStates const & states;
};