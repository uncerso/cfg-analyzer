#pragma once
#include <memory>
#include <string>
#include <list>

class State{
public:
    virtual ~State() = default;

    virtual std::list<std::string> makeText(int startPos) = 0;
};

State * makeState(char op, State * left, State * right);
int priority(char op);
bool isOperation(char op);
// , - conc
// ; - union
// # - iter

class StateConc : public State {
public:
    StateConc(State *left, State *right);
    std::list<std::string> makeText(int startPos) override;

private:
    std::unique_ptr<State> left;
    std::unique_ptr<State> right;
};

class StateUnion : public State {
public:
    StateUnion(State *left, State *right);
    std::list<std::string> makeText(int startPos) override;

private:
    std::unique_ptr<State> left;
    std::unique_ptr<State> right;
};

class StateIter : public State {
public:
    StateIter(State *left, State *right);
    std::list<std::string> makeText(int startPos) override;

private:
    std::unique_ptr<State> left;
    std::unique_ptr<State> right;
};

class StateTerm : public State {
public:
    StateTerm(std::string && s);
    std::list<std::string> makeText(int startPos) override;

private:
    std::string s;
};

class StateNonTerm : public State {
public:
    StateNonTerm(std::string && s);
    std::list<std::string> makeText(int startPos) override;

private:
    std::string s;
};