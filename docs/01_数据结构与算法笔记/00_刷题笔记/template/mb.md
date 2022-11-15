

汇总到 mbecmi 见导出词库
另配置cftool账号有问题 登录不上 


# 基本


## 数据结构




## 算法

#### 


```c++  

```


#### 


```c++  

```


#### 


```c++  

```


#### 


```c++  

```

#### 


```c++  

```

#### 


```c++  

```

#### 


```c++  

```



## 常用

#### mbhu


```c++  

```



#### 


```c++  

```

#### mbejvmj


```c++  

```

#### mbejvy


```c++  

```

#### mbdrlu


```c++  

```

#### 


```c++  

```

#### 


```c++  

```

#### 


```c++  

```







#### mbqjhb



```c++  
void merge(vector<PII>& segs)
{
	vector<PII> res;

	sort(segs.begin(), segs.end());

	int st = -2e9, ed = -2e9;
	for (auto seg : segs)
		if (ed < seg.first)
		{
			if (st != -2e9) res.push_back({ st, ed });
			st = seg.first, ed = seg.second;
		}
		else ed = max(ed, seg.second);

	if (st != -2e9) res.push_back({ st, ed });

	segs = res;
}
```



#### mbdalr



```c++  
vector<int> shuffle() {     // 对于 i = n-1 ~ 1, 交换 i ,(0 ~ i)
	for (int i = output.size() - 1; i >= 1; i--) { swap(output[i], output[rand() % (i + 1)]); }
	return output;
}
```


#### fgzfi
```c++  
string str = "...";
int n = str.size();
for (int i = 0; i < n; ++i) {
	int j = i;
	while (j < n && str[j] != ' ') j++;
	for (int k = i; k < j; ++k) cout << str[k];
	cout << endl;
	i = j;
}
```



## mbuqv 
(n - 1) / m + 1



### 大数

#### dujf


```c++  
vector<int> add(vector<int>& A, vector<int>& B)
{
	if (A.size() < B.size()) return add(B, A);

	vector<int> C;
	for (int i = 0, t = 0; i < A.size(); i++)
	{
		t += A[i];
		if (i < B.size()) t += B[i];
		C.push_back(t % 10);
		t /= 10;
	}

	if (t) C.push_back(t);
	return C;
}

auto C = add(A, B);
reverse(C.begin(), C.end());
```


#### dujf


```c++  
bool cmp(vector<int>& A, vector<int>& B)
{
	if (A.size() != B.size()) return A.size() > B.size();
	for (int i = A.size() - 1; i >= 0; i--) {
		if (A[i] != B[i]) return A[i] > B[i];
	}
	return true;
}

vector<int> sub(vector<int>& A, vector<int>& B)
{
	vector<int> C;
	for (int i = 0, t = 0; i < A.size(); i++)
	{
		t = A[i] - t;
		if (i < B.size()) t -= B[i];
		C.push_back((t + 10) % 10);
		if (t < 0) t = 1;
		else t = 0;
	}
	while (C.size() > 1 && C.back() == 0) C.pop_back();
	return C;
}

if (cmp(A, B)) {
	auto C = sub(A, B);
}else {
	auto C = sub(B, A);  
    cout << '-';
}
```


#### duif


```c++  
vector<int> mul(vector<int>& A, int b)
{
	vector<int> C;
	int t = 0;
	for (int i = 0; i < A.size() || t; i++)
	{
		if (i < A.size()) t += A[i] * b;
		C.push_back(t % 10);
		t /= 10;
	}
	while (C.size() > 1 && C.back() == 0) C.pop_back();
	return C;
}
```




```c++  
vector<int> mul(vector<int>& A, vector<int>& B) {
	vector<int> C(A.size() + B.size(), 0); 

	for (int i = 0; i < A.size(); i++)
		for (int j = 0; j < B.size(); j++)
			C[i + j] += A[i] * B[j];

	int t = 0;
	for (int i = 0; i < C.size(); i++) { 
		t += C[i];
		C[i] = t % 10;
		t /= 10;
	}

	while (C.size() > 1 && C.back() == 0) C.pop_back(); 
	return C;
}
```


#### duif



```c++  
vector<int> div(vector<int>& A, int b, int& r)
{
	vector<int> C;
	r = 0;
	for (int i = A.size() - 1; i >= 0; i--)
	{
		r = r * 10 + A[i];
		C.push_back(r / b);
		r %= b;
	}
	reverse(C.begin(), C.end());
	while (C.size() > 1 && C.back() == 0) C.pop_back();
	return C;
}
```



# 模板

## acm



```c++  
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <cstdlib>
using namespace std;
const int N = 100010;

int main()
{

	return 0;
}
```



```c++  
// acm-full mode
// #include <bits/stdc++.h>
#include <cstdio>    
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iomanip>  
#include <limits.h>
#include <utility>
#include <cstdlib>
#include <numeric>

using namespace std;
#define IOS ios::sync_with_stdio(false)
#define TIE cin.tie(nullptr)

typedef long long LL;    
typedef unsigned long long ULL;   
typedef double D;
#define fi first         
#define se second                 
#define ins insert       
#define pb push_back
#define ALL(v) (v).begin(), (v).end()      
#define SORT(v) sort(ALL(v))       
#define SORTR(v) SORT(v), reverse(ALL(v))
#define mpr make_pair

using PII = pair<int, int>;   
using PIII = pair<PII, int>;

#define lowbit(x) ((x)&(-(x)))
template <typename T> void chkMax(T& x, T y) { if (y > x) x = y; }
template <typename T> void chkMin(T& x, T y) { if (y < x) x = y; }

typedef std::vector<int> VI;   
typedef std::vector<std::vector<int>> VII;
#define fir(i, a, b) for (int i = a;i <= b;i ++)     
#define firr(i, a, b) for (int i = a;i >= b;i --)
#define read(x) scanf("%d", &x)
#define read2(x, y) scanf("%d %d", &x, &y)
#define read3(x, y, z) scanf("%d %d %d", &x, &y, &z)
#define read4(x, y, z, w) scanf("%d %d %d %d", &x, &y, &z, &w)
#define readvn(x, n) vi x(n + 1); fir(i, 1, n) scanf("%d", &x[i])
#define readn(x, a, b) fir(i, a, b) scanf("%d", &x[i])
#define readnn(x, a, b, aa, bb) fir(i, a, b) fir(j, aa, bb) scanf("%d", &x[i][j])

void __print(int x) { cout << x; }
void __print(int x, int y) { cout << x << " " << y << endl; }
void __print(int x, int y, int z) { cout << x << " " << y << " " << z << endl; }
void __print(int x, int y, int z, int w) { cout << x << " " << y << " " << z << " " << w << endl; }
void __print(int x, int y, int z, int w, int v) { cout << x << " " << y << " " << z << " " << w << " " << v << endl; }
void __print(long long x) { cout << x; }     void __print(double x) { cout << x; }
void __print(bool x) { cout << (x ? "true" : "false"); }

template<typename T, typename V>
void __print(const pair<T, V>& x) { cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}'; }
template<typename T>
void __print(const T& x) { int f = 0; cout << '{'; for (auto& i : x) cout << (f++ ? ", " : ""), __print(i); cout << "}"; }
template<typename T>
void printn(const T& x, int l, int r) { for (int i = l; i <= r; ++i) cout << (i != l ? " " : ""), __print(x[i]); cout << endl; }
void print() { cout << endl; }
template <typename T, typename... V>
void _print(T t, V... v) { __print(t); if (sizeof...(v)) cout << ", "; _print(v...); }

template <typename A, typename B>
ostream& operator <<(ostream& out, const pair<A, B>& a) {	out << "(" << a.first << "," << a.second << ")";	return out;  }
template <typename T>
ostream& operator <<(ostream& out, const vector<T>& a) {	out << "["; bool first = true;   	for (auto v : a) { out << (first ? "" : ", "); out << v; first = 0; } out << "]";  	return out;    }

const int mod = 1e9 + 7, inf = 1e9 + 7;
const double eps = 1e-6;
int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};

const int N = 100010;

int main(){
  

	//system("pause");
	return 0;
}
```



## lc-core


```c++  
// lc-core-mid mode
// #include <bits/stdc++.h>
typedef long long LL;    
typedef unsigned long long ULL;   
typedef double D;
#define fi first         
#define se second                 
#define ins insert       
#define pb push_back
#define ALL(v) (v).begin(), (v).end()      
#define SORT(v) sort(ALL(v))       
#define SORTR(v) SORT(v), reverse(ALL(v))
#define mpr make_pair

using PII = pair<int, int>;   
using PIII = pair<PII, int>;

#define lowbit(x) ((x)&(-(x)))
template <typename T> void chkMax(T& x, T y) { if (y > x) x = y; }
template <typename T> void chkMin(T& x, T y) { if (y < x) x = y; }

typedef std::vector<int> VI;   
typedef std::vector<std::vector<int>> VII;
#define fir(i, a, b) for (int i = a;i <= b;i ++)     
#define firr(i, a, b) for (int i = a;i >= b;i --)

void __print(int x) { cout << x; }
void __print(int x, int y) { cout << x << " " << y << endl; }
void __print(int x, int y, int z) { cout << x << " " << y << " " << z << endl; }
void __print(int x, int y, int z, int w) { cout << x << " " << y << " " << z << " " << w << endl; }
void __print(int x, int y, int z, int w, int v) { cout << x << " " << y << " " << z << " " << w << " " << v << endl; }
void __print(long long x) { cout << x; }     void __print(double x) { cout << x; }
void __print(bool x) { cout << (x ? "true" : "false"); }

template<typename T, typename V>
void __print(const pair<T, V>& x) { cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}'; }
template<typename T>
void __print(const T& x) { int f = 0; cout << '{'; for (auto& i : x) cout << (f++ ? ", " : ""), __print(i); cout << "}"; }
template<typename T>
void printn(const T& x, int l, int r) { for (int i = l; i <= r; ++i) cout << (i != l ? " " : ""), __print(x[i]); cout << endl; }
void print() { cout << endl; }
template <typename T, typename... V>
void _print(T t, V... v) { __print(t); if (sizeof...(v)) cout << ", "; _print(v...); }

template <typename A, typename B>
ostream& operator <<(ostream& out, const pair<A, B>& a) {	out << "(" << a.first << "," << a.second << ")";	return out;  }
template <typename T>
ostream& operator <<(ostream& out, const vector<T>& a) {	out << "["; bool first = true;   	for (auto v : a) { out << (first ? "" : ", "); out << v; first = 0; } out << "]";  	return out;    }

const int mod = 1e9 + 7, inf = 1e9 + 7;
const double eps = 1e-6;
int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};

const int N = 100010;

```




```c++  
// lc-core-std mode
// #include <bits/stdc++.h>
typedef long long LL;
typedef unsigned long long ULL;
typedef double D;
#define fi first
#define se second
#define ins insert
#define pb push_back
#define all(v) (v).begin(), (v).end()
#define sort(v) sort(all(v))
#define sortr(v) sort(v), reverse(all(v))
#define mpr make_pair

using PII = pair<int, int>;
using PIII = pair<PII, int>;

#define lowbit(x) ((x)&(-(x)))
template <typename T> void chkMax(T& x, T y) { if (y > x) x = y; }
template <typename T> void chkMin(T& x, T y) { if (y < x) x = y; }

typedef std::vector<int> VI;
typedef std::vector<std::vector<int>> VII;
#define fir(i, a, b) for (int i = a;i <= b;i ++)
#define firr(i, a, b) for (int i = a;i >= b;i --)

void __print(int x) { cout << x; }
void __print(int x, int y) { cout << x << " " << y << endl; }
void __print(int x, int y, int z) { cout << x << " " << y << " " << z << endl; }
void __print(int x, int y, int z, int w) { cout << x << " " << y << " " << z << " " << w << endl; }
void __print(int x, int y, int z, int w, int v) { cout << x << " " << y << " " << z << " " << w << " " << v << endl; }
void __print(long x) { cout << x; }
void __print(long long x) { cout << x; }
void __print(unsigned x) { cout << x; }
void __print(unsigned long x) { cout << x; }
void __print(unsigned long long x) { cout << x; }
void __print(float x) { cout << x; }
void __print(double x) { cout << x; }
void __print(long double x) { cout << x; }
void __print(char x) { cout << '\'' << x << '\''; }
void __print(const char* x) { cout << '\"' << x << '\"'; }
void __print(const string& x) { cout << '\"' << x << '\"'; }
void __print(bool x) { cout << (x ? "true" : "false"); }

template<typename T, typename V>
void __print(const pair<T, V>& x) { cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}'; }
template<typename T>
void __print(const T& x) { int f = 0; cout << '{'; for (auto& i : x) cout << (f++ ? ", " : ""), __print(i); cout << "}"; }
template<typename T>
void printn(const T& x, int l, int r) { for (int i = l; i <= r; ++i) cout << (i != l ? " " : ""), __print(x[i]); cout << endl; }
void print() { cout << endl; }
template <typename T, typename... V>
void _print(T t, V... v) { __print(t); if (sizeof...(v)) cout << ", "; _print(v...); }

template <typename A, typename B>
ostream& operator <<(ostream& out, const pair<A, B>& a) {
	out << "(" << a.first << "," << a.second << ")";
	return out;
}
template <typename T>
ostream& operator <<(ostream& out, const vector<T>& a) {
	out << "["; bool first = true;
	for (auto v : a) { out << (first ? "" : ", "); out << v; first = 0; } out << "]";
	return out;
}

const int mod = 1e9 + 7;
const int inf = 1e9 + 7;
const double eps = 1e-6;
int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

const int N = 100010;


```

## 读写

### mbre



```c++  
typedef std::vector<int> vi;
typedef std::vector<std::vector<int>> vii;
#define fir(i, a, b) for (int i = a;i <= b;i ++)
#define firr(i, a, b) for (int i = a;i >= b;i --)
#define read(x) scanf("%d", &x)
#define read2(x, y) scanf("%d %d", &x, &y)
#define read3(x, y, z) scanf("%d %d %d", &x, &y, &z)
#define read4(x, y, z, w) scanf("%d %d %d %d", &x, &y, &z, &w)
#define readvn(x, n) vi x(n + 1); fir(i, 1, n) scanf("%d", &x[i])
#define readn(x, a, b) fir(i, a, b) scanf("%d", &x[i])
#define readnn(x, a, b, aa, bb) fir(i, a, b) fir(j, aa, bb) scanf("%d", &x[i][j])
```


### mbdb


```c++  
void __print(int x) { cout << x; }
void __print(int x, int y) { cout << x << " " << y << endl; }
void __print(int x, int y, int z) { cout << x << " " << y << " " << z << endl; }
void __print(int x, int y, int z, int w) { cout << x << " " << y << " " << z << " " << w << endl; }
void __print(int x, int y, int z, int w, int v) { cout << x << " " << y << " " << z << " " << w << " " << v << endl; }

template<typename T, typename V>
void __print(const pair<T, V>& x) { cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}'; }
template<typename T>
void __print(const T& x) { int f = 0; cout << '{'; for (auto& i : x) cout << (f++ ? ", " : ""), __print(i); cout << "}"; }
template<typename T>
void printn(const T& x, int l, int r) { for (int i = l; i <= r; ++i) cout << (i != l ? " " : ""), __print(x[i]); cout << endl; }
void print() { cout << endl; }
template <typename T, typename... V>
void _print(T t, V... v) { __print(t); if (sizeof...(v)) cout << ", "; _print(v...); }
```



```c++  
void __print(int x) { cout << x; }
void __print(int x, int y) { cout << x << " " << y << endl; }
void __print(int x, int y, int z) { cout << x << " " << y << " " << z << endl; }
void __print(int x, int y, int z, int w) { cout << x << " " << y << " " << z << " " << w << endl; }
void __print(int x, int y, int z, int w, int v) { cout << x << " " << y << " " << z << " " << w << " " << v << endl; }
void __print(long x) { cout << x; }
void __print(long long x) { cout << x; }
void __print(unsigned x) { cout << x; }
void __print(unsigned long x) { cout << x; }
void __print(unsigned long long x) { cout << x; }
void __print(float x) { cout << x; }
void __print(double x) { cout << x; }
void __print(long double x) { cout << x; }
void __print(char x) { cout << '\'' << x << '\''; }
void __print(const char* x) { cout << '\"' << x << '\"'; }
void __print(const string& x) { cout << '\"' << x << '\"'; }
void __print(bool x) { cout << (x ? "true" : "false"); }

template<typename T, typename V>
void __print(const pair<T, V>& x) { cout << '{'; __print(x.first); cout << ", "; __print(x.second); cout << '}'; }
template<typename T>
void __print(const T& x) { int f = 0; cout << '{'; for (auto& i : x) cout << (f++ ? ", " : ""), __print(i); cout << "}"; }
template<typename T>
void printn(const T& x, int l, int r) { for (int i = l; i <= r; ++i) cout << (i != l ? " " : ""), __print(x[i]); cout << endl; }
void print() { cout << endl; }
template <typename T, typename... V>
void _print(T t, V... v) { __print(t); if (sizeof...(v)) cout << ", "; _print(v...); }

template <typename A, typename B>
ostream& operator <<(ostream& out, const pair<A, B>& a) {
	out << "(" << a.first << "," << a.second << ")";
	return out;
}
template <typename T>
ostream& operator <<(ostream& out, const vector<T>& a) {
	out << "["; bool first = true;
	for (auto v : a) { out << (first ? "" : ", "); out << v; first = 0; } out << "]";
	return out;
}
```



### mbgetline



```c++  
#include <iostream>
#include <string>
#include <sstream>


string input;
while (getline(cin, input)) {
	if (input.size() > 0) {
		stringstream stringin(input);
		int num;
		while (stringin >> num) {
			// 
		}
	}
}
```



```c++  
#include <iostream>
#include <string>
#include <sstream>

string input;
char* tok;
while (getline(cin, input)) {
	if (input.size() > 0) {
		vector<int> a;
		tok = strtok((char*)input.c_str(), " ,[]");
		while (tok != NULL) {
			a.push_back(stoi(tok));
			tok = strtok(NULL, " ,[]");
		}
	}
}
```



```c++  
istringstream iss(s);
string temp;
char split = ' ';
vector<string> data;
while (getline(iss, temp, split)) {
	data.push_back(temp);
}
```

```c++  
#include <cstring>

string input;
char* sp = " !";
char* token;
token = strtok((char*)input.c_str(), sp);
while( token != NULL ) {
    string t(token); //
    token = strtok(NULL, sp);
}
```

# 其他


## 便捷网站
### gtbj
https://afengcodes.gitee.io/learning_note/
### lcpm
https://lcpredictor.herokuapp.com/


### lcjs
https://leetcode-cn.com/contest/

### acjs
https://www.acwing.com/activity/1/competition/