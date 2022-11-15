// acm-full mode
// #include <bits/stdc++.h>
#include <cstdio>    
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>  
#include <limits.h>
#include <utility>
#include <cstdlib>
#include <numeric>

#include <map>
#include <set>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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
#define mpr make_pair
#define ALL(v) (v).begin(), (v).end()      
#define SORT(v) sort(ALL(v))       
#define SORTR(v) SORT(v), reverse(ALL(v))
#define LB(v, x) lower_bound(ALL(v), x) - (v).begin()
#define UB(v, x) upper_bound(ALL(v), x) - (v).begin()
#define MST(v, x) memset((v), x, sizeof (v))

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

------------------------------


// acm-std mode
// #include <bits/stdc++.h>
#include <cstdio>    
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>  
#include <limits.h>
#include <utility>
#include <cstdlib>
#include <numeric>

#include <map>
#include <set>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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
#define mpr make_pair
#define ALL(v) (v).begin(), (v).end()      
#define SORT(v) sort(ALL(v))       
#define SORTR(v) SORT(v), reverse(ALL(v))
#define LB(v, x) lower_bound(ALL(v), x) - (v).begin()
#define UB(v, x) upper_bound(ALL(v), x) - (v).begin()
#define MST(v, x) memset((v), x, sizeof (v))

using PII = pair<int, int>;   
using PIII = pair<PII, int>;

#define lowbit(x) ((x)&(-(x)))
template <typename T> void chkMax(T& x, T y) { if (y > x) x = y; }
template <typename T> void chkMin(T& x, T y) { if (y < x) x = y; }

typedef std::vector<int> VI;   
typedef std::vector<std::vector<int>> VII;
#define fir(i, a, b) for (int i = a;i <= b;i ++)     
#define firr(i, a, b) for (int i = a;i >= b;i --)

const int mod = 1e9 + 7, inf = 1e9 + 7;
const double eps = 1e-6;
int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};

const int N = 100010;

int main()
{


	//system("pause");
	return 0;
}

