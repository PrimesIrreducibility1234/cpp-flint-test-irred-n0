#include <bits/stdc++.h>
#include <flint/fmpz.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_poly_factor.h>
#include <fstream>
#define ull unsigned long long
using namespace std;
vector<int> f;
int deg;
int supp;
vector<int> support;
#define ll long long
int numodd = 0;
ull maxnum = 0;
int maxindex = 0;
ll MOD = 1000000007;
vector<bool> inSupport;
double eps = 1e-12;
vector<int> choose;

#define forn(i, e) for (ll i = 0; i < e; i++)
#define mp make_pair
#define pb push_back
#define endl '\n'
#define gcd(a, b) __gcd(a, b)

bool has(int z) {
    return 0 <= z && z <= deg && inSupport[z];
}

ull value(ull n) {
	ull ans = 0;
	for (int i = deg; i >= 0; i--) {
		ans = ans * n + f[i];
	}
	return ans;
}


bool nonnegPoly(const fmpz_poly_t P) {
	fmpz_t c;
	fmpz_init(c);
	long d = fmpz_poly_degree(P);
	for (long i = 0; i <= d; i++) {
		fmpz_poly_get_coeff_fmpz(c, P, i);
		if (fmpz_sgn(c) < 0) {
			fmpz_clear(c);
			return false;
		}
	}
	fmpz_clear(c);
	return true;
}

bool validPoly(const fmpz_poly_t P) {
	if (fmpz_poly_is_zero(P)) return false;
	if (fmpz_poly_is_one(P)) return false;
	return nonnegPoly(P);
}

bool dfsFactorization(int idx, const fmpz_poly_t F, const fmpz_poly_factor_t fac) {
	int r = fac->num;

	if (idx == r) {
		bool allZero = true;
		bool allFull = true;
		for (int i = 0; i < r; i++) {
			if (choose[i] != 0) allZero = false;
			if (choose[i] != fac->exp[i]) allFull = false;
		}
		if (allZero || allFull) return false;

		fmpz_poly_t g, h, tmp;
		fmpz_poly_init(g);
		fmpz_poly_init(h);
		fmpz_poly_init(tmp);

		fmpz_poly_one(g);

		for (int i = 0; i < r; i++) {
			for (int k = 0; k < choose[i]; k++) {
				fmpz_poly_mul(tmp, g, fac->p + i);
				fmpz_poly_swap(g, tmp);
			}
		}

		bool ok = false;

		if (validPoly(g)) {
	if (fmpz_poly_divides(h, F, g)) {
		if (validPoly(h)) {
			ok = true;
			/*cout << "Factorization in N0: ";
			fmpz_poly_print_pretty(g, "x");
			cout << " * ";
			fmpz_poly_print_pretty(h, "x");
			cout << endl;*/
		}
	}
}

		fmpz_poly_clear(g);
		fmpz_poly_clear(h);
		fmpz_poly_clear(tmp);

		return ok;
	}

	for (int e = 0; e <= fac->exp[idx]; e++) {
		choose[idx] = e;
		if (dfsFactorization(idx + 1, F, fac)) {
			return true;
		}
	}

	return false;
}

int coeffContent() {
	int g = 0;
	for (int x : f) {
		if (x != 0) g = gcd(g, x);
	}
	return g;
}

bool reducibleN0(const fmpz_poly_t F, const fmpz_poly_factor_t fac) {
	choose.assign(fac->num, 0);
	return dfsFactorization(0, F, fac);
}

bool irreducibleN0Exact() {
	fmpz_poly_t poly;
	fmpz_poly_init(poly);
	for (int i = 0; i <= deg; i++) {
		fmpz_poly_set_coeff_si(poly, i, f[i]);
	}
	fmpz_poly_factor_t fac;
	fmpz_poly_factor_init(fac);
	fmpz_poly_factor(fac, poly);
	bool ans = !reducibleN0(poly, fac);
	fmpz_poly_clear(poly);
	fmpz_poly_factor_clear(fac);
	return ans;
}

auto coversAll (const vector<int>& A, ull Bmask, ull supportMask) {
	ull seen = 0;
	for (int a : A) {
		seen |= Bmask << a;
	}
	return seen == supportMask;
}

ull validBMaskForA (vector<int> A, int bmax, ull supportMask) {
	ull Bmask = supportMask & ((1ULL << (bmax + 1)) - 1);
	for (int a : A) {
		ull good = 0;
		for (int y:support) {
			if (y>bmax) continue;
			if (a+y<=deg && inSupport[a+y]) {
				good |= 1ULL<< y;
			}
		}
		Bmask &= good;
	}
	return Bmask;
}

struct MinkowskiResult {
	bool exists;
	vector<int> A;
	vector<int> B;
};

vector<int> maskToVec(ull mask) {
	vector<int> v;
	for (int i = 0; i <= deg; i++) {
		if ((mask >> i) & 1ULL) v.pb(i);
	}
	return v;
}

MinkowskiResult getMinkowskiDecomp(vector<int> support) {
	if (support.size() < 3) return {false,{},{}};
	if (support.back() != 0) return {false,{},{}};
	inSupport.assign(deg + 1, false);
	ull supportMask = 0;
	for (int x : support) {
		inSupport[x] = true;
		supportMask |= 1ULL << x;
	}

	int numComplements = 0;
	pair<int,int> complementPair = {-1,-1};

	for (int x : support) {
		if (x == 0 || x == deg) continue; 

		int y = deg - x;
		if (y <= 0 || y >= deg) continue;

		if (x <= y && inSupport[y]) {
			numComplements++;
			complementPair = {x, y};
		}
	}

	
	if (numComplements == 0) return {false,{},{}};

	if (numComplements == 1) {
		int a = complementPair.first;
		int b = complementPair.second;
		int minPos = support[supp - 2];

		if (!has(a + minPos) && !has(b + minPos)) {
			return {false,{},{}};
		}
	}

	for (int amax : support) {
		if (amax == 0 || deg - amax <= 0 || amax > deg - amax || !inSupport[deg - amax]) continue;
		int bmax = deg - amax;

		vector<int> candA;

		for (int x : support) {
			if (x > amax) continue;
			if (x + bmax <= deg && inSupport[x + bmax]) {
				candA.push_back(x);
			}
		}
		vector<char> inCandA(deg + 1, false);
		for (int x : candA) inCandA[x] = true;
		if (!inCandA[0]||!inCandA[amax]) continue;

		vector<int> middle;
		for (int x:candA) {
			if (x != 0 && x != amax) {
				middle.pb(x);
			}
		}
		int m = middle.size();

		for (unsigned long long mask = 0; mask < (1ULL << m); mask++) {
			vector<int> A = {0, amax};
			for (int i = 0; i < m; i++) {
				if (mask & (1ULL << i)) {
					A.pb(middle[i]);
				}
			}
			ull Bmask = validBMaskForA(A, bmax, supportMask);
			if (__builtin_popcountll(Bmask) < 2 || ((Bmask >> 0) & 1ULL) == 0 || ((Bmask >> bmax) & 1ULL) == 0 ||  (long long)A.size() * __builtin_popcountll(Bmask) < (long long)support.size()) continue;
			if (coversAll(A, Bmask, supportMask)) {
                vector<int> B = maskToVec(Bmask);

                sort(A.begin(), A.end(), greater<int>());
                sort(B.begin(), B.end(), greater<int>());

                return {true, A, B};
            }
		}
	}
	return {false,{},{}};
}

int main() {
	ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL); 

	double avgsize = 0;
	int DEG = 0;
    int x;
    while(cin>>x){
        DEG= max(DEG,x);
        support.pb(x);
    }
	deg = DEG;
    sort(support.begin(),support.end(),greater<int>());
    supp = support.size();
	mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
	int skippedBySupportCriteria = 0;
	int testedExactly = 0;
    MinkowskiResult res = getMinkowskiDecomp(support);

if (!res.exists) {
	cout << "No Minkowski Decomposition: ";
	for (int x : support) cout << x << " ";
	cout << "\n";
} else {
	cout << "Has Minkowski Decomposition\n";
	cout << "A = ";
	for (int x : res.A) cout << x << " ";
	cout << "\n";
	cout << "B = ";
	for (int x : res.B) cout << x << " ";
	cout << "\n";
	cout << "A + B = ";
	set<int, greater<int>> sums;
	for (int a : res.A) {
		for (int b : res.B) {
			sums.insert(a + b);
		}
	}
	for (int x : sums) cout << x << " ";
	cout << "\n";
}

}
