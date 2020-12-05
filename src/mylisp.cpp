#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>


using namespace std;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// cell ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/*변경된 곳 1*/
enum cell_type { Symbol, Number, List, Proc, String, Lambda }; 
//cell내부에 포함된 celltype을 enum으로 정의. magic number를 쓰기보다 뜻을 알기 쉽게
//enum으로 정의해준다.

struct environment; // cell에서 environment를 참조하고, environment도 cell을 참조하므로
//구조체 전방선언을 해준다.

//다양한 리스프 값들을 받을 수 있는 구조체.
struct cell {
	typedef cell(*proc_type)(const vector<cell>&);//프로시저 타입별로, 해당하는 벡터를 인자로 하는 함수를 받는 함수 포인터
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
const cell true_sym(Symbol, "true"); //false_sym이 아닌 것들은 모두 true_sym이다.
const cell nil(Symbol, "nil");
const cell error(Symbol, "ERROR");

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// environment ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


//각 기호들을 해당 셀과 연결하고
//만약 추가로 함수를 정의한다면 outer를 이용하여 만들어는 dictionary이다
struct environment {
	// 변수 이름들을 셀로 매핑해준다.
	typedef map<string, cell> map;

	environment(environment* outer = 0) : outer_(outer) {}

	environment(const cells& parms, const cells& args, environment* outer)
		: outer_(outer)
	{
		cellit a = args.begin();
		for (cellit p = parms.begin(); p != parms.end(); ++p)
			env_[p->val] = *a++;
	}
	//string var이 나타나는 레퍼런스를 반환한다.
	map& find(const string& var)
	{
		if (env_.find(var) != env_.end())
			return env_; // symbol들이 위에서 매핑한 env에 들어있으므로, 이것을 리턴해줌.
		if (outer_)//사용자 정의함수가 생기는 순간, outer_가 0에서 1,2,3등의 값으로 바뀌므로
			//env_함수에서 해당 함수를 find하지 못했을 때 outer에서 찾기를 수행한다.
			return outer_->find(var); // "outer"에서도 symbol을 찾아줌
		cout << "unbound symbol '" << var << endl;//아무것도 찾지 못했을 때 출력.
		exit(1);
	}

	//입력인자 var에, 해당 env_의 셀의 주소자를 반환한다.
	cell& operator[] (const string& var)
	{
		return env_[var];
	}

private:
	map env_; // 셀로 맵핑해두었음.
	environment* outer_; //아우터 포인터는, 새로운 함수를 정의할 때 쓰인다.
};

string str(long n);
bool isdig(char c);
bool isfloat(string c);
bool check_float(const cellit& start, const cellit& end);
string lowercase(string up_string);


////////////////////// 구문을 파싱하고, 읽고 사용하는데에 필요.
list<string> tokenize(const string& str); cell atom(const string& token); cell read_from(list<string>& tokens);
cell read(const string& s); string to_string(const cell& exp); void repl(const string& prompt, environment* env);
void add_globals(environment& env); cell eval(cell x, environment* env);

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// functions ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//내장함수들
cell proc_add(const cells& c) {
	bool flag = check_float(c.begin(), c.end());//flag로 정수인지 소수인지 판단해준다.

	if (flag) {//소수이면 소수로 계산을 함
		float n(stof(c[0].val));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n += stof(i->val);
		return cell(Number, to_string(n));
	}
	else {//정수면 정수로 계산을 함
		long n(atol(c[0].val.c_str()));
		for (cellit i = c.begin() + 1; i != c.end(); ++i) n += atol(i->val.c_str());
		return cell(Number, str(n));
	}

}
cell proc_sub(const cells& c) {//flag로 정수인지 소수인지 판단
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
	float n(stof(c[0].val));
	for (cellit i = c.begin() + 1; i != c.end(); ++i) n /= stof(i->val);
	return cell(Number, to_string(n));
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
	return c[0].type == String ? true_sym : nil;
}
cell proc_print(const cells& c) {
	return c[0];
}



////////////////////// eval함수
cell eval(cell x, environment* env) {
	if (x.type == Symbol) {
		string lower_str = lowercase(x.val);
		return env->find(lower_str)[lower_str];
	}
	if (x.type == Number)
		return x;
	if (x.type == String)
		return x;
	if (x.list.empty())
		return nil;
	if (x.list[0].type == Symbol || x.val == "\'" || x.val == "\"" || x.val == "#") {
		if (x.val == "\'" || x.val == "#")
			return x.list[0];
		if (x.val == "\"") {
			x.list[0].val.pop_back();
			return x.list[0];
		}
		if (lowercase(x.list[0].val) == "if")         //cell로 맵핑하지 않은 함수중 if를 인식하는 역할을 한다.
			return eval(eval(x.list[1], env).val == "false" ? (x.list.size() < 4 ? nil : x.list[3]) : x.list[2], env);
		if (lowercase(x.list[0].val) == "cond") {
			int i;
			for (i = 1; i < x.list.size(); i++) {
				if (x.list[i].list.size() == 1) return eval(x.list[i].list[0], env);
				if (eval(x.list[i].list[0], env).val == "true") return eval(x.list[i].list[1], env);
			}
		}
		if (lowercase(x.list[0].val) == "setq")      //cell로 맵핑하지 않은 함수중 setq를 인식하는 역할을 함.
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
		/*변경된곳 2*/
		if (x.list[0].val == "lambda") {    // (lambda (var*) exp)
			x.type = Lambda;
			// keep a reference to the environment that exists now (when the
			// lambda is being defined) because that's the outer environment
			// we'll need to use when the lambda is executed
			x.env = env;
			return x;
		}
		//
		//cell로 모든 함수를 맵핑하려 했으나, if cond setq등 맵핑하는데 어려울 것 같은 함수들은 eval
		//함수 내에 해당 역할을 수행하는 if문을 작성하였음.
		//
	}
	cell proc(eval(x.list[0], env));
	cells exps;
	for (cell::iter exp = x.list.begin() + 1; exp != x.list.end(); ++exp)
		exps.push_back(eval(*exp, env));
	/*변경된곳 3*/
	if (proc.type == Lambda) {
		// Create an environment for the execution of this lambda function
		// where the outer environment is the one that existed* at the time
		// the lambda was defined and the new inner associations are the
		// parameter names with the given arguments.
		// *Although the environmet existed at the time the lambda was defined
		// it wasn't necessarily complete - it may have subsequently had
		// more symbols defined in that environment.
		return eval(/*body*/proc.list[2], new environment(/*parms*/proc.list[1].list, /*args*/exps, proc.env));
	}
	else if (proc.type == Proc)
		return proc.proc(exps);

	std::cout << "not a function\n";
	exit(1);

}

//숫자를 string으로 바꿔서 반환해주는 함수
string str(long n) {
	ostringstream os;
	os << n;
	return os.str();
}
//ostringstream이란 문자열 format을 조합하여 저장해 줄때 사용하는 class이다.


//입력 인자가 0,1,2,,,,9등 숫자일 때 true를 return 해준다
bool isdig(char c) {
	return isdigit(static_cast<unsigned char>(c)) != 0;
}


bool isfloat(string c) {
	return c.find('.') == string::npos ? false : true;
}


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


//while true 문을 통해서,
//계속 입력을 받아주도록 되어있는 repl 함수
void repl(const string& prompt, environment* env)
{
	while (true) {
		cout << prompt;
		string line; getline(cin, line);
		cout << to_string(eval(read(line), env)) << endl;
	}
}


//입력받은 식을 lisp식으로 반환해주는 함수
cell read(const string& s)
{
	list<string> tokens(tokenize(s));
	return read_from(tokens);
}


//입력 받은 str을 토큰화 하여 토큰 list로 반환해주는 함수.
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
		else if (*s == '\"') {
			tokens.push_back("\"");
			s++;
			const char* t = s;
			while (*t && *t != '(' && *t != ')') {
				++t;
			}
			tokens.push_back(string(s, t));
			s = t;
		}
		else if (*s == '#') {
			tokens.push_back("#");
			s++;
		}
		else {
			const char* t = s;
			while (*t && *t != ' ' && *t != '(' && *t != ')') {
				++t;
			}
			tokens.push_back(string(s, t));
			s = t;
		}
	}
	return tokens;
}


//토큰 list에서 lisp 식을 반환해주는 함수.
cell read_from(list<string>& tokens) {
	const string token(tokens.front());
	tokens.pop_front();

	if (token == "(") {
		cell c(List);
		while (tokens.front() != ")")
			c.list.push_back(read_from(tokens));
		tokens.pop_front();
		return c;
	}
	else if (token == "\'") {
		cell c(List, "\'");
		c.list.push_back(read_from(tokens));
		return c;
	}
	else if (token == "\"") {
		cell c(List, "\"");
		c.list.push_back(read_from(tokens));
		return c;
	}
	else if (token == "#") {
		cell c(List, "#");
		c.list.push_back(read_from(tokens));
		return c;
	}
	else
		return atom(token);
}



//위에서 정의해준 enum대로, 숫자면 enum의 Numbers, string이면 String
//다른 토큰들은 Symbol이라는 속성을 부여한 cell로 바꾸어준다.
cell atom(const string& token) {
	if (isdig(token[0]) || (token[0] == '-' && isdig(token[1])))
		return cell(Number, token);
	else if (!(token.find('\"') == string::npos))
		return cell(String, token);
	return cell(Symbol, token);
}


//cell속성으로 입력받은 인자들을, lisp string으로 반환한다.
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
	/*변경된곳 4*/
	else if (exp.type == Lambda)
		return "<Lambda>";
	else if (exp.type == Proc)
		return "<Proc>";
	return exp.val;
}


//입력받은 environment env가, [] 괄호에 해당하는 내용일경우, 해당하는 함수 포인터를 cell()에
//집어넣어 cell화 시킨후,env[]에 return한다.

//예를 들어 설명하면, environment & env에 append라는 값이 들어오면,
//env["append"]에 해당하므로, env["append"] = cell(&proc_append); 라는 줄에 의해
//proc_append함수의 포인터를 cell화 시켜서 대입해준다.
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
	env[">="] = cell(&proc_greater_equal); env["="] = cell(&proc_equal);
	env["caddr"] = cell(&proc_caddr);
	env["reverse"] = cell(&proc_reverse); env["ERROR"] = error;
	env["atom"] = cell(&proc_atom); env["numberp"] = cell(&proc_numberp);
	env["zerop"] = cell(&proc_zerop); env["minusp"] = cell(&proc_minusp);
	env["equal"] = cell(&proc_equal); env["stringp"] = cell(&proc_stringp);
	env["print"] = cell(&proc_print);
}

int main()
{
	environment global_env;
	add_globals(global_env);
	repl(">>> ", &global_env);

}
