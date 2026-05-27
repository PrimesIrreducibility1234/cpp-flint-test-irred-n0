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
		ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
		std::ofstream file("coeff1.txt");
		std::streambuf* original_buf = std::cout.rdbuf(); 
		std::cout.rdbuf(file.rdbuf());
		int numtested = 0;
	int maxdeg = 8;
	int maxcoeff = 5;
	//cin>>maxdeg>>maxcoeff;
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
		bool irreducible = false;;
		if (primeValueTest()) {cout << 1; irreducible = true;} else cout<<0;
		if (eisenstein()) {cout << 1; irreducible = true;} else cout<<0;
		if (perron()) {cout << 1; irreducible = true;} else cout<<0;
		if (ostrowski()) {cout << 1; irreducible = true;} else cout<<0;
		if (bevelacqua()) {cout << 1; irreducible = true;}	 else cout<<0;
		if (monolithic()) {cout << 1; irreducible = true;} else cout<<0;
		if (crowdmath()) {cout << 1; irreducible = true;} else cout<<0;
		if (kolekar()) {cout << 1; irreducible = true;} else cout<<0;
		if (bonciocat()) {cout << 1; irreducible = true;} else cout<<0;

		
		if(irreducible){
			cout << " Irreducible in N0" << endl;
		}
		else{
		fmpz_poly_t poly;
		fmpz_poly_init(poly);
		forn(i, deg + 1) fmpz_poly_set_coeff_si(poly, i, f[i]);

		fmpz_poly_factor_t fac;
		fmpz_poly_factor_init(fac);
		fmpz_poly_factor(fac, poly);

		fmpz_t content;
		fmpz_init(content);
		fmpz_poly_content(content, poly);

		fmpz_clear(content);

		if (!reducibleN0(poly, fac)) {
			cout << " Irreducible in N0" << endl;
		} else
			cout << " Reducible in N0" << endl;
		fmpz_poly_clear(poly);
		fmpz_poly_factor_clear(fac);
		}
	}
	cout.rdbuf(original_buf);
	file.close();

	return 0;
}
