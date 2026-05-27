#include <bits/stdc++.h>
#include <flint/fmpz.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_poly_factor.h>
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
double eps = 1e-12;
vector<int> choose;

#define forn(i, e) for (ll i = 0; i < e; i++)
#define mp make_pair
#define pb push_back
#define endl '\n'
#define gcd(a, b) __gcd(a, b)

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
			//cout << "f(" << i << ") = " << value(i) << " is prime, so ";
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
				//cout << "f is Eisenstein at " << p << ", so ";
				return true;
			}
			while (temp % p == 0) temp /= p;
		}
	}
	if (temp > 1) {
		ull p = temp;
		if (f[deg] % p != 0 && f[0] % (p * p) != 0) {
			//cout << "f is Eisenstein at " << p << ", so ";
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
	if (supp < 2) return false;
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
	int diff = support[1] - support[0];
	for (int i = 2; i < supp; i++) {
		if (support[i] - support[i - 1] >= diff) return false;
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
			//cout <<i << v << p.first << "^" << p.second;
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
    		if (fmpz_poly_divides(h, F, g)) {
        		if (validPoly(h)) {
				ok = true;
				/*cout << "Factorization in N0: (";
				fmpz_poly_print_pretty(g, "x");
				cout << ") * (";
				fmpz_poly_print_pretty(h, "x");
				cout << ")" << endl;*/
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

bool reducibleN0(const fmpz_poly_t F, const fmpz_poly_factor_t fac) {
	choose.resize(fac->num);
	return dfsFactorization(0, F, fac);
}



int main() {
		ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
		std::ofstream file("coeff.txt");
		std::streambuf* original_buf = std::cout.rdbuf(); 
		std::cout.rdbuf(file.rdbuf());
		int numtested = 0;
	int maxdeg = 8;
	int maxcoeff = 5;
	cin>>maxdeg>>maxcoeff;
	long long totalnum = 1;
	for(int i=0; i<=maxdeg; i++) {
		totalnum = totalnum * (maxcoeff+1);
	}
	f.resize(maxdeg + 1);
	for(long long index=1; index<totalnum; index++) {
		if(numtested%6000 == 0)    cerr << "numtested = " << numtested << " / " << totalnum << '\n';
		numtested++;
		numodd = 0;
		maxnum = 0;
		maxindex = 0;
		choose.clear();
		supp = 0;
		long long tempindex = index;
		for(int j=0; j<=maxdeg; j++) {
			f[j] = tempindex % (maxcoeff + 1);
			tempindex /= (maxcoeff + 1);
		}
		deg = f.size() - 1;
		deg = maxdeg;
		while (deg >= 0 && f[deg] == 0) {
			deg--;
		}

		if (deg <= 1) continue;
		if (f[0] == 0) continue;

		support.clear();
		ull g = gcd((int)f[0], (int)f[1]);
		for (ull i = 2; i <= deg; i++) {
			g = gcd((int)g, (int)f[i]);
		}
		for (int i = deg; i >= 0; i--) {
			if (f[i] != 0) {
				support.pb(i);
				if (i % 2 == 1) numodd++;
			}
		}
		supp = support.size();
		if (g > 1) continue;
		for (int i = 0; i <= deg; i++) {
			if (f[i] > maxnum) {
				maxnum = f[i];
				maxindex = i;
			}
		}

		for(int i=deg; i>=0; i--) {
			if(f[i]!=0) {
				if(i!=deg) cout << " + ";
				if(f[i]!=1 || i==0)cout << f[i];
				cout << "x^" << i;
			}
		}
		cout << " ";
		if (primeValueTest()) cout << 1; else cout<<0;
		if (eisenstein()) cout << 1; else cout<<0;
		if (perron()) cout << 1; else cout<<0;
		if (ostrowski()) cout << 1; else cout<<0;
		if (bevelacqua()) cout << 1; else cout<<0;
		if (monolithic()) cout << 1; else cout<<0;
		if (crowdmath()) cout << 1; else cout<<0;
		if (kolekar()) cout << 1; else cout<<0;
		if (bonciocat()) cout << 1; else cout<<0;

		

		fmpz_poly_t poly;
		fmpz_poly_init(poly);
		forn(i, deg + 1) fmpz_poly_set_coeff_si(poly, i, f[i]);

		fmpz_poly_factor_t fac;
		fmpz_poly_factor_init(fac);
		fmpz_poly_factor(fac, poly);

		fmpz_t content;
		fmpz_init(content);
		fmpz_poly_content(content, poly);

		/*if (fmpz_cmp_ui(content, 1) != 0 || fac->num > 1 || (fac->num == 1 && fac->exp[0] > 1)) {
			cout<<"Factorization in Z: ";
			for (int i = 0; i < fac->num; i++) {
				if(i > 0) cout << " * ";
				cout << "(";
				fmpz_poly_print_pretty(fac->p + i, "x");
				cout << ")";
				if (fac->exp[i] > 1) {
					cout << "^" << fac->exp[i];
				}
			}
			cout << "\nReducible in Z[x]" << endl;
		} else {
			cout << "Irreducible in Z[x]" << endl;
		}*/

		fmpz_clear(content);

		if (!reducibleN0(poly, fac)) {
			cout << " Irreducible in N0" << endl;
		} else
			cout << " Reducible in N0" << endl;
		fmpz_poly_clear(poly);
		fmpz_poly_factor_clear(fac);
	}
}
