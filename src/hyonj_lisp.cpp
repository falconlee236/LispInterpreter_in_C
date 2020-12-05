#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

////////////////////// cell

enum cell_type { Symbol, Number, List, Proc };

struct environment; // forward declaration; cell and environment reference each other

// a variant that can hold any kind of lisp value
struct cell {
	typedef cell(*proc_type)(const vector<cell>&);
	typedef vector<cell>::const_iterator iter;
	typedef map<string, cell> map;

	cell_type type;
	string val;
	vector<cell> list;
	proc_type proc;
	environment* env;

	cell(cell_type type = Symbol) : type(type), env(0) {}
	cell(cell_type type, const string& val) : type(type), val(val), env(0) {}
	cell(proc_type proc) : type(Proc), proc(proc), env(0) {}
};

typedef vector<cell> cells;
typedef cells::const_iterator cellit;

const cell false_sym(Symbol, "false");
const cell true_sym(Symbol, "true"); // anything that isn't false_sym is true
const cell nil(Symbol, "nil");
const cell error(Symbol, "ERROR");


////////////////////// environment

// a dictionary that (a) associates symbols with cells, and
// (b) can chain to an "outer" dictionary
struct environment {
	// map a variable name onto a cell
	typedef map<string, cell> map;

	environment(environment* outer = 0) : outer_(outer) {}

	environment(const cells& parms, const cells& args, environment* outer)
		: outer_(outer)
	{
		cellit a = args.begin();
		for (cellit p = parms.begin(); p != parms.end(); ++p)
			env_[p->val] = *a++;
	}

	// return a reference to the innermost environment where 'var' appears
	map& find(const string& var)
	{
		if (env_.find(var) != env_.end())
			return env_; // the symbol exists in this environment
		if (outer_)
			return outer_->find(var); // attempt to find the symbol in some "outer" env
		cout << "unbound symbol '" << var << endl;
		exit(1);
	}

	// return a reference to the cell associated with the given symbol 'var'
	cell& operator[] (const string& var)
	{
		return env_[var];
	}

private:
	map env_; // inner symbol->cell mapping
	environment* outer_; // next adjacent outer env, or 0 if there are no further environments
};

//(+ 3/4 3) 15/4
////////////////////// user-define fucntions
string str(long n);
bool isdig(char c);
bool isfloat(string c);
bool check_float(const cellit& start, const cellit& end);
string lowercase(string up_string);

////////////////////// built-in primitive procedures
cell proc_add(const cells& c); cell proc_sub(const cells& c); cell proc_mul(const cells& c);
cell proc_div(const cells& c); cell proc_greater(const cells& c); cell proc_less(const cells& c);
cell proc_less_equal(const cells& c); cell proc_length(const cells& c); cell proc_nullp(const cells& c);
cell proc_car(const cells& c); cell proc_cdr(const cells& c); cell proc_append(const cells& c);
cell proc_cons(const cells& c); cell proc_list(const cells& c);

/*insert user_define func*/
cell proc_caddr(const cells& c);

////////////////////// parse, read and user interaction
list<string> tokenize(const string& str); cell atom(const string& token); cell read_from(list<string>& tokens);
cell read(const string& s); string to_string(const cell& exp); void repl(const string& prompt, environment* env);
void add_globals(environment& env); cell eval(cell x, environment* env);

/*
function define
*/
////////////////////// built-in primitive procedures

cell proc_add(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n += stof(i->val);
		return cell(Number, to_string(n));
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n += atol(i->val.c_str());
		return cell(Number, str(n));
	}

}
cell proc_sub(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n -= stof(i->val);
		return cell(Number, to_string(n));
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n -= atol(i->val.c_str());
		return cell(Number, str(n));
	}

}
cell proc_mul(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n *= stof(i->val);
		return cell(Number, to_string(n));
	}
	else {
		long n(1);
		for (cellit i = c.begin(); i != c.end(); ++i) n *= atol(i->val.c_str());
		return cell(Number, str(n));
	}

}
cell proc_div(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n /= stof(i->val);
		return cell(Number, to_string(n));
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n /= atol(i->val.c_str());
		return cell(Number, str(n));
	}

}
cell proc_greater(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n <= stof(i->val))
				return false_sym;
		return true_sym;
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n <= atol(i->val.c_str()))
				return false_sym;
		return true_sym;
	}

}
cell proc_less(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n >= stof(i->val))
				return false_sym;
		return true_sym;
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n >= atol(i->val.c_str()))
				return false_sym;
		return true_sym;
	}

}
cell proc_less_equal(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n > stof(i->val))
				return false_sym;
		return true_sym;
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n > atol(i->val.c_str()))
				return false_sym;
		return true_sym;
	}

}
cell proc_greater_equal(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n < stof(i->val))
				return false_sym;
		return true_sym;
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n < atol(i->val.c_str()))
				return false_sym;
		return true_sym;
	}
}
cell proc_numberp(const cells& c) { return c[0].type == Number ? true_sym : false_sym; }
cell proc_atom(const cells& c) { return c[0].type == Symbol ? true_sym : false_sym; }
cell proc_length(const cells& c) { return cell(Number, str(c[0].list.size())); }
cell proc_null(const cells& c) { return c[0].list.empty() ? true_sym : false_sym; }
cell proc_car(const cells& c) { return c[0].list[0]; }
cell proc_cdr(const cells& c)
{
	if (c[0].list.size() < 2)
		return nil;
	cell result(c[0]);
	result.list.erase(result.list.begin());
	return result;
}
cell proc_caddr(const cells& c) {
	if (c[0].list.size() < 3)
		return nil;
	cell result(c[0]);
	result.list.erase(result.list.begin());
	result.list.erase(result.list.begin());
	return result.list[0];
}
cell proc_append(const cells& c) {
	cell result(List);
	result.list = c[0].list;
	for (int k = 1; k < c.size(); k++) {
		for (cellit i = c[k].list.begin(); i != c[k].list.end(); ++i) result.list.push_back(*i);
	}
	return result;
}
cell proc_cons(const cells& c)
{
	cell result(List);
	result.list.push_back(c[0]);
	for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) result.list.push_back(*i);
	return result;
}
cell proc_list(const cells& c)
{
	cell result(List); result.list = c;
	return result;
}
cell proc_reverse(const cells& c) {
	cell result(List);
	result.list = c[0].list;
	reverse(result.list.begin(), result.list.end());
	return result;
}
cell proc_member(const cells& c) {
	cell result(List);
	bool find = false;
	for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) {
		if (i->val == c[0].val || find) {
			find = true;
			result.list.push_back(*i);
		}
	}
	if (!find) return nil;
	return result;
}
cell proc_assoc(const cells& c) {
	for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) {
		if (i->list[0].val == c[0].val) {
			return *i;
		}
	}
	return nil;
}
cell proc_remove(const cells& c) {
	cell result(List);
	for (cellit i = c[1].list.begin(); i != c[1].list.end(); ++i) {
		if (i->val != c[0].val) result.list.push_back(*i);
	}
	return result;
}
cell proc_subst(const cells& c) {
	cell result(List);
	for (cellit i = c[2].list.begin(); i != c[2].list.end(); ++i) {
		if (i->val == c[1].val) result.list.push_back(c[0]);
		else result.list.push_back(*i);
	}
	return result;
}
cell proc_minusp(const cells& c) {
	if (c[0].type != Number) return error;
	return c[0].val.find('-') == string::npos ? false_sym : true_sym;
}
cell proc_zerop(const cells& c) {
	if (c[0].type != Number) return error;
	return c[0].val == "0" ? true_sym : false_sym;
}
cell proc_equal(const cells& c) {
	bool flag = check_float(c.begin(), c.end());

	if (flag) {
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n == stof(i->val))
				return true_sym;
		return false_sym;
	}
	else {
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i)
			if (n == atol(i->val.c_str()))
				return true_sym;
		return false_sym;
	}
}
cell proc_stringp(const cells& c) {
	return true_sym;
}




////////////////////// eval
cell eval(cell x, environment* env) {
	if (x.type == Symbol) {
		string lower_str = lowercase(x.val);
		return env->find(lower_str)[lower_str];
	}
	if (x.type == Number)
		return x;
	if (x.list.empty())
		return nil;
	if (x.list[0].type == Symbol || x.val == "\'") {
		if (x.val == "\'")
			return x.list[0];
		if (lowercase(x.list[0].val) == "if")         // (if test conseq [alt])
			return eval(eval(x.list[1], env).val == "#f" ? (x.list.size() < 4 ? nil : x.list[3]) : x.list[2], env);
		if (lowercase(x.list[0].val) == "setq")      // (setq var exp)
			return (*env)[x.list[1].val] = eval(x.list[2], env);
		if (lowercase(x.list[0].val) == "nth") {
			if (x.list[2].type != List || x.list[2].list[0].type == Symbol)
				return error;

			cells result(x.list[2].list[0].list);
			int val = stoi(x.list[1].val);

			if (result.size() < val)
				return nil;
			return result[val];
		}
	}
	cell proc(eval(x.list[0], env));
	cells exps;
	for (cell::iter exp = x.list.begin() + 1; exp != x.list.end(); ++exp)
		exps.push_back(eval(*exp, env));

	if (proc.type == Proc)
		return proc.proc(exps);

	std::cout << "not a function\n";
	exit(1);

}

// return given mumber as a string
string str(long n) {
	ostringstream os;
	os << n;
	return os.str();
}

// return true iff given character is '0'..'9'
bool isdig(char c) {
	return isdigit(static_cast<unsigned char>(c)) != 0;
}

//made by LSY
bool isfloat(string c) {
	return c.find('.') == string::npos ? false : true;
}

//made by LSY
bool check_float(const cellit& start, const cellit& end) {
	for (cellit i = start; i != end; i++) {
		if (isfloat(i->val)) {
			return true;
		}
	}
	return false;
}

string lowercase(string up_string) {
	transform(up_string.begin(), up_string.end(), up_string.begin(), tolower);
	return up_string;
}

////////////////////// parse, read and user interaction

// the default read-eval-print-loop
void repl(const string& prompt, environment* env)
{
	while (true) {
		cout << prompt;
		string line; getline(cin, line);
		cout << to_string(eval(read(line), env)) << endl;
	}
}

// return the Lisp expression represented by the given string
cell read(const string& s)
{
	list<string> tokens(tokenize(s));
	return read_from(tokens);
}

// convert given string to list of tokens
list<string> tokenize(const string& str) {
	list<string> tokens;
	const char* s = str.c_str();
	while (*s) {
		while (*s == ' ') {
			++s;
		}

		if (*s == '(' || *s == ')')
			tokens.push_back(*s++ == '(' ? "(" : ")");
		else if (*s == '\'') {
			tokens.push_back("\'");
			s++;
		}
		else {
			const char* t = s;
			while (*t && *t != ' ' && *t != '(' && *t != ')') {
				++t;
			}
			tokens.push_back(lowercase(string(s, t)));
			s = t;
		}
	}
	return tokens;
}
// return the Lisp expression in the given tokens
cell read_from(list<string>& tokens) {
	const string token(tokens.front());
	tokens.pop_front();

	if (token == "(") {
		cell c(List);
		while (tokens.front() != ")") {
			c.list.push_back(read_from(tokens));
		}
		tokens.pop_front();

		return c;
	}
	else if (token == "\'") {
		cell c(List, "\'");
		c.list.push_back(read_from(tokens));
		return c;
	}
	else if ((token.substr(0, 2) == "ca" || token.substr(0, 2) == "cd") && (token.size()>2 && token[2]!= 'r' )) {
		cell c(List);
		string temp = token.substr(0, 2);
		temp.insert(temp.end(), 'r');
		c.list.push_back(cell(Symbol, temp));
		cell s = cell(Symbol, temp);
		temp = token;
		temp.erase(temp.begin()+1);
		tokens.push_front(temp);
		tokens.push_front("(");
		tokens.push_back(")");
		return s;
	}
	else
		return atom(token);
}

// numbers become Numbers; every other token is a Symbol
cell atom(const string& token) {
	if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
		return cell(Number, token);
	return cell(Symbol, token);
}

// convert given cell to a Lisp-readable string
string to_string(const cell& exp)
{
	if (exp.type == List) {
		string s("(");
		for (cell::iter e = exp.list.begin(); e != exp.list.end(); ++e)
			s += to_string(*e) + ' ';
		if (s[s.size() - 1] == ' ')
			s.erase(s.size() - 1);
		return s + ')';
	}
	else if (exp.type == Proc)
		return "<Proc>";
	return exp.val;
}

// define the bare minimum set of primintives necessary to pass the unit tests
void add_globals(environment& env)
{
	env["nil"] = nil;   env["#f"] = false_sym;  env["#t"] = true_sym;
	env["append"] = cell(&proc_append);   env["car"] = cell(&proc_car);
	env["cdr"] = cell(&proc_cdr);      env["cons"] = cell(&proc_cons);
	env["length"] = cell(&proc_length);   env["list"] = cell(&proc_list);
	env["member"] = cell(&proc_member);   env["assoc"] = cell(&proc_assoc);
	env["remove"] = cell(&proc_remove);   env["subst"] = cell(&proc_subst);
	env["null"] = cell(&proc_null);    env["+"] = cell(&proc_add);
	env["-"] = cell(&proc_sub);      env["*"] = cell(&proc_mul);
	env["/"] = cell(&proc_div);      env[">"] = cell(&proc_greater);
	env["<"] = cell(&proc_less);     env["<="] = cell(&proc_less_equal);
	env[">="] = cell(&proc_greater_equal);
	env["reverse"] = cell(&proc_reverse); env["ERROR"] = error;
	env["atom"] = cell(&proc_atom); env["numberp"] = cell(&proc_numberp);
	env["zerop"] = cell(&proc_zerop); env["minusp"] = cell(&proc_minusp);
	env["equal"] = cell(&proc_equal); env["stringp"] = cell(&proc_stringp);
}

int main()
{
	environment global_env;
	add_globals(global_env);
	repl("90> ", &global_env);
}
