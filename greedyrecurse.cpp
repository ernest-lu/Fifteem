#include<bits/stdc++.h>
using namespace std;
typedef long long LL;
typedef long double LD;
typedef pair<LL,LL> pii;
typedef pair<LL,pii> ppi;
typedef pair<pii,pii> ppp;
#define FOR(i, n) for(int i = 1; i<=n; i++)
#define F0R(i, n) for(int i = 0; i<n; i++)
#define all(x) x.begin(), x.end()
#define mp make_pair
#define pb push_back
#define f first
#define s second

template<typename A, typename B> ostream& operator<<(ostream &cout, pair<A, B> const &p) { return cout << "(" << p.f << ", " << p.s << ")"; }
template<typename A> ostream& operator<<(ostream &cout, vector<A> const &v) {cout << "["; F0R(i, v.size()) {if (i) cout << ", "; cout << v[i];} return cout << "]";}

//var 
const string inputSize = "small";
const LD eps = 0.1;
const LL numTeams = 241;
LL N, D = 30, Rs = 3, Rp = 8;
LL T;
vector<vector<LL>> cover;
vector<LD> optimalValues(numTeams, 1e9);
string optloc;
LL inputType = 0;
vector<vector<LL>> needFixing(3, vector<LL>(numTeams, 1));
vector<vector<LL>> fix = {{25, 118, 209, 212, 75, 127, 56, 116, 129, 187, 48, 3, 196, 153, 15, 87, 229, 85, 227, 127}, 
						  {233, 178, 152, 129, 103, 169, 183, 212, 60, 84, 193, 212, 39, 60, 84, 9, 16, 43, 56, 64, 147, 231, 38, 53, 85, 113, 171, 195, 197, 198, 3, 50, 99, 105, 133, 140, 174, 208, 211, 215, 216, 221, 6, 55, 100, 110, 137, 138, 145, 158, 187, 205, 225, 226, 148}, 
						  {227, 33, 10, 66, 79, 181, 129, 189, 219, 75, 204, 149, 197, 71, 109, 108, 233, 3, 4, 6, 8, 9, 11, 12, 15, 17, 21, 24, 27, 37, 41, 45, 55, 57, 58, 60, 68, 69, 77, 81, 86, 90, 92, 95, 124, 137, 140, 141, 148, 151, 152, 157, 158, 161, 163, 164, 185, 198, 208, 216, 228, 212, 231, 235, 239, 3}};

function<LL (LL, LL)> dist = [&](LL a, LL b) {
	LL x = a / D, y = a % D;
	LL nx = b / D, ny = b % D;
	return (nx - x) * (nx - x) + (ny - y) * (ny - y);
};	

void setCover() {
	cover.assign(D * D, vector<LL>());
	F0R (i, D * D) {
		F0R (j, D * D) {
			if (dist(i, j) <= Rs * Rs) {
				cover[i].pb(j);
			}
		} 
	}
}

function<vector<pii> (vector<LL>)> convertSolution = [&](vector<LL> res) {
	vector<pii> ret((int)res.size());
	F0R (i, (int)ret.size()) {
		ret[i] = {res[i] / D, res[i] % D};
	} return ret;
};

function<LD (vector<LL>)> score = [&](vector<LL> res) {
	LD retValue = 0;
	F0R (i, (int)res.size()) {
		LL cnt = 0;
		F0R (j, (int)res.size()) {
			if (i == j) continue;	
			if (dist(res[i], res[j]) <= Rp * Rp) {
				cnt++;
			}
		}
		retValue += exp((LD)0.17 * cnt);
	}
	return retValue * 170;
};

string toString(LL g) {
	string res = "000";
	res[2] = g % 10 + '0'; g /= 10;
	res[1] = g % 10 + '0'; g /= 10;
	res[0] = g % 10 + '0'; g /= 10;
	return res;
}

void writeOutput(LL inputNumber, vector<LL> solution) {
	//only do if valid solution
	LD oldScore = optimalValues[inputNumber - 1];
	LD newScore = score(solution);
	if (newScore - oldScore >= -eps) {
		//cout << "didn't change " << inputNumber << ": " << oScore << " " << newScore << "\n";
		return;
	}
	string outputLocation = "outputs/" + inputSize + "/" + toString(inputNumber);
	string hashtag = "#"; LD oScore = 1e9;

	if (newScore - oScore >= -eps) {
		optimalValues[inputNumber - 1] = oScore;
		return;
	}

	ofstream fout((outputLocation + ".out"));
	fout << "# " << newScore << '\n';
	vector<pii> converted = convertSolution(solution);
	fout << converted.size() << '\n';
	for (auto z : converted) {
		fout << z.f << " " << z.s << "\n";
	}

	if (newScore - oldScore < -eps) cout << "changed " << inputNumber << ": " << oldScore << " " << newScore << "\n";
	optimalValues[inputNumber - 1] = newScore;
}

void solve(LL inputNumber){
	string inputLocation = "inputs/" + inputSize + "/" + toString(inputNumber);
	ifstream fin((inputLocation + ".in"));

	mt19937 randy(chrono::steady_clock::now().time_since_epoch().count());
	string dummy; getline(fin, dummy);
	fin >> N >> D >> Rs >> Rp;
	if (cover.empty()) setCover();

	function<vector<LL> (vector<bool>, vector<LL>)> greedy = [&](vector<bool> a, vector<LL> baseAns) {
		bool allCovered = 0;
		LL uncovered = 0;
		F0R (i, D * D) {
			if (a[i]) {
				uncovered++;
			}
		}
		set<LL> ans;
		vector<LL> res;
        for (auto u : baseAns) {
            ans.insert(u);
            res.pb(u);
        }
		while (uncovered > 0) {
			LL maxCovering = 0; vector<LL> hasMaxCovering;
            vector<LL> bruhArray(D*D);
            iota(all(bruhArray), 0);
            
			/*sort(all(bruhArray), [&](LL t1, LL t2) {
				LL a1 = abs((t1 % D) - D / 2) + abs((t1 / D) - D / 2);
				LL a2 = abs((t2 % D) - D / 2) + abs((t2 / D) - D / 2);
				if (a1 == a2) {
					return (t1 / D) < (t2 / D);
				}
				return a1 < a2;
			}); */
			
			shuffle(all(bruhArray), randy);
			//reverse(all(bruhArray));
			LL maxx = 0, maxy = 0, maxPerimeterCount = 0, maxD = 0, minx = 1e9, miny = 1e9, maxF = -1e9;
			for (auto i : bruhArray) {
				if (!a[i]) continue;
				maxx = max(maxx, i / D);
				maxy = max(maxy, i % D);
				maxD = max(maxD, i / D + i % D);
				minx = min(minx, i / D);
				miny = min(miny, i % D);
				maxF = max(maxF, i / D - i % D);
			}

			for (auto i : bruhArray) {
				LL perimeter = 0;
				for (auto u : cover[i]) {
					if (a[u]) {
						if ((u % D) == maxy) {
							perimeter++;
						}
					}
				} maxPerimeterCount = max(maxPerimeterCount, perimeter);
			}

			for (auto i : bruhArray) {
				if (ans.count(i)) continue;
				LL cnt = 0;
				LL perimeter = 0;
				for (auto u : cover[i]) {
					if (a[u]) {
						cnt++;
						if ((u % D) == maxy) perimeter++;
					}
				}
				//if (perimeter != maxPerimeterCount) continue;
				if (cnt > maxCovering) {
					hasMaxCovering.clear();
					hasMaxCovering.pb(i);
					maxCovering = cnt;
				} else if (cnt == maxCovering) {
					hasMaxCovering.pb(i);
				}
			}

			assert(!hasMaxCovering.empty());
			
			LL minIntersections = 1e9, toAdd = -1;
			toAdd = hasMaxCovering[0];
			if (abs(maxCovering - uncovered) <= 1000) {
				for (auto u : hasMaxCovering) {
					LL intersections = 0;
					for (auto z : res) {
						if (dist(u, z) <= Rp * Rp) intersections++;
					}
					if (intersections < minIntersections) {
						minIntersections = intersections;
						toAdd = u;
					}
				}
			}

			for (auto u : cover[toAdd]) {
				if (a[u]) uncovered--;
				a[u] = 0;
			}
			ans.insert(toAdd);
			res.pb(toAdd);
		}

		return res;
 	};

 	vector<bool> a(D * D);
 	FOR (i, N) {
 		LL x = 0, y = 0;
 		fin >> x >> y;
 		a[x * D + y] = 1;
 	}

    string ogOutput = "outputs/" + inputSize + "/" + toString(inputNumber);
    ifstream oin(ogOutput + ".out");
    getline(oin, dummy);
    LL originalSolutionSize = 0; oin >> originalSolutionSize;
    vector<LL> originalIndexes(originalSolutionSize), originalValues(originalSolutionSize);
    for (auto& u : originalValues) {
        LL x = 0, y = 0;
        oin >> x >> y;
        u = x * D + y;
    };
    iota(all(originalIndexes), 0);
    //reverse(all(originalIndexes));
	shuffle(all(originalIndexes), randy);
    
    LL leftBound = max((originalSolutionSize / 2), 1LL), rightBound = max(1LL, originalSolutionSize - 1LL);
    LL chosenBound = ((randy()) % (rightBound - leftBound + 1)) + leftBound;
	chosenBound = max(1LL, chosenBound);
	//chosenBound = originalSolutionSize;
	chosenBound = 0;
    vector<LL> newBase(chosenBound);
    F0R (i, chosenBound) {
        newBase[i] = originalIndexes[i];
    } sort(all(newBase));
    for (auto& u : newBase) {
        u = originalValues[u];
        for (auto z : cover[u]) a[z] = 0;
    }

    vector<LL> solution = greedy(a, newBase);
    //cout << convertSolution(solution) << ' ' << score(solution) << "\n";
 	writeOutput(inputNumber, solution);
}

int main(){
	ios_base::sync_with_stdio(0); 
	cin.tie(0);

	if (inputSize == "large") {
		inputType = 2;
	} else if (inputSize == "medium") {
		inputType = 1;
	} else if (inputSize == "small") {
		inputType = 0;
	}

	for (auto u : fix[inputType]) {
		needFixing[inputType][u - 1] = 1;
	}

	optloc = inputSize + "opt.in"; 
	ifstream oppy(optloc);
	FOR (i, numTeams) oppy >> optimalValues[i - 1];

	FOR (i, numTeams) {
		if (!needFixing[inputType][i - 1]) continue;
		//cout << i << "\n";
		solve(i);
	}
	//solve(89);

	ofstream opp(optloc);
	FOR (i, numTeams) opp << optimalValues[i - 1] << '\n';

	cout.flush();
	return 0;
}

