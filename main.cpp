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

bool isPrime(ull n) {
	if (n <= 1) return false;
	for (ull i = 2; i * i <= n; i++) {
		if (n % i == 0) return false;
	}
	return true;
}

bool primeValueTest() {
	for (int i = 2; i <= 100; i++)
		if (isPrime(value(i))) {
			cout << "f(" << i << ") = " << value(i) << " is prime, so ";
			return true;
		}
	return false;
}

bool eisenstein() {
	ull g = gcd((int)f[0], (int)f[1]);
	for (ull i = 2; i < deg; i++) {
		g = gcd((int)g, (int)f[i]);
	}
	if (g < 2) return false;
	ull temp = g;
	for (ull p = 2; p * p <= temp; p++) {
		if (temp % p == 0) {
			if (f[deg] % p != 0 && f[0] % (p * p) != 0) {
				cout << "f is Eisenstein at " << p << ", so ";
				return true;
			}
			while (temp % p == 0) temp /= p;
		}
	}
	if (temp > 1) {
		ull p = temp;
		if (f[deg] % p != 0 && f[0] % (p * p) != 0) {
			cout << "f is Eisenstein at " << p << ", so ";
			return true;
		}
	}
	return false;
}

bool perron() {
	if (f[deg] == 1) {
		int ans = f[deg - 1] - 1;
		forn(i, deg - 1) {
			ans -= f[i];
		}
		return ans > 0;
	}
	return false;
}

bool ostrowski() {
	if (isPrime(f[0])) {
		int ans = f[0];
		for (int i = 1; i <= deg; i++) {
			ans -= f[i];
		}
		return ans > 0;
	}
	return false;
}

bool bevelacqua() {
	bool applicable = false;
	if (isPrime(f[0])) applicable = true;
	if (!applicable && f[0] != 1) {
		int smallestprime;
		for (int i = 2; i * i <= f[0]; i++) {
			if (f[0] % i == 0) {
				smallestprime = i;
				break;
			}
		}
		if (isPrime(f[deg]) && smallestprime * f[deg] >= f[0]) applicable = true;
	}
	if (!applicable) return false;
	for (int i = 0; i < deg; i++) {
		if (f[i] < f[i + 1]) return false;
	}
	int g = 0;
	int run = 1;
	for (int i = 1; i <= deg; i++) {
		if (f[i] == f[i - 1])
			run++;
		else {
			g = gcd(g, run);
			run = 1;
		}
	}
	g = gcd(g, run);
	return (g == 1);
}

bool monolithic() {
	if (support[1] < 0.5 * (support[0] + support[supp - 1])) {
		//cout << "f is monolithic by the first criterion, so ";
		return true;
	}
	if (supp > 3 && support[1] <= 0.5 * (support[0] + support[supp - 1])) {
		//cout << "f is monolithic by the second criterion, so ";
		return true;
	}
	if (support[supp - 2] > 0.5 * (support[0] + support[supp - 1])) {
		//cout << "f is monolithic by the third criterion, so ";
		return true;
	}
	if (supp > 3 && support[supp - 2] >= 0.5 * (support[0] + support[supp - 1])) {
		//cout << "f is monolithic by the fourth criterion, so ";
		return true;
	}
	int diff = support[supp-2] - support[supp-1];


	for (int i = 1; i < supp-1; i++) {
		if (support[i-1]-support[i] <= diff) return false;
	}
	//cout << "f is monolithic by the fifth criterion, so ";
	return true;
}

bool crowdmath() {
	if (supp > 3 && numodd == 1) {
		//cout << "f has only one odd degree term, so ";
		return true;
	}
	if (numodd == supp) {
		//cout << "f has no even degree terms, so ";
		return true;
	}
	return false;
}

bool kolekar() {
	ull m = 0;
	for (int i = 0; i <= deg; i++) {
		if (i != maxindex && f[i] > m) m = f[i];
	}
	return (maxnum > (ull)(0.5 * (deg + 2) * m * m));
}

bool isInteger(double x) {
	return fabs(x - round(x)) < 1e-9;
}

pair<int, int> isPrimePower(ull n) {
	for (int i = 2; i <= 63; i++) {
		if (pow(n, 1.0 / i) < 2) break;
		if (isInteger(pow(n, 1.0 / i)) && isPrime(round(pow(n, 1.0 / i))))
			return {round(pow(n, 1.0 / i)), i};
	}
	return {-1, -1};
}

bool derivativeNotDivide(ull n, ull p) {
	ull ans = 0;
	for (int i = 1; i <= deg; i++) {
		ans += i * f[i] * pow(n, i - 1);
	}
	return ans % p != 0;
}

bool bonciocat() {
	for (int i = maxnum + 1; i <= maxnum + 100; i++) {
		ull v = value(i);
		pair<int, int> p = isPrimePower(v);
		if (p.first != -1 && derivativeNotDivide(i, p.first)) {
			cout << "f(" << i << ") = " << v << " is a prime power" << p.first << "^"
			     << p.second << ", and f'(" << i << ") is not divisible by "
			     << p.first << ", so ";
			return true;
		}
	}
	return false;
}
// Primitive check *TODO*

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
			if (fmpz_poly_divides(h, F, g) && validPoly(h)) {
				ok = true;
			}

			/*cout << "Factorization in N0: ";
			fmpz_poly_print_pretty(g, "x");
			cout << " * ";
			fmpz_poly_print_pretty(h, "x");
			cout << endl;*/
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



bool hasMinkowskiDecomp(vector<int> support) {
	if (support.size() < 3) return false;
	if (support.back() != 0) return false;
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


	if (numComplements == 0) return false;

	if (numComplements == 1) {
		int a = complementPair.first;
		int b = complementPair.second;
		int minPos = support[supp - 2];

		if (!has(a + minPos) && !has(b + minPos)) {
			return false;
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
			if (coversAll(A, Bmask,supportMask)) return true;
		}
	}
	return false;
}

int main() {
	std::ofstream file("out.txt");
	std::streambuf* original_buf = std::cout.rdbuf();
	std::cout.rdbuf(file.rdbuf());
	double avgsize = 0;
	int DEG = 20;
    cin>> DEG;
	deg = DEG;
	mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
	int skippedBySupportCriteria = 0;
	int testedExactly = 0;

	for (int mask = 0; mask < (1 << (DEG - 1)); mask++) {
		support.clear();
		supp = 2;
		numodd = 0;
		support.pb(0);
		for(int i=1; i<DEG; i++) {
			if(mask&(1<<(i-1))) {
				support.pb(i);
				supp++;
				if(i%2==1) numodd++;
			}
		}
		support.pb(DEG);
		if(DEG%2==1) numodd++;

		reverse(support.begin(), support.end());
		int totalIrred = 0;
		int totalTested = 0;
		if(!monolithic() && !crowdmath()) {
			if(hasMinkowskiDecomp(support)) {
				continue;
			}
			forn(i,support.size()) {
				cout << support[i] << " ";
			}
			cout << endl;

		}
	}
    std::cout.rdbuf(original_buf);
    return 0;
}
