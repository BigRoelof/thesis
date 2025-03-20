#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

vector<set<int>> readDimacs(const string& filename, int& numVars) {
    ifstream file(filename);
    string line;
    vector<set<int>> clauses;
    numVars = 0;

    while (getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') {
            istringstream iss(line);
            string tmp;
            iss >> tmp >> tmp >> numVars;
            continue;
        }
        istringstream iss(line);
        set<int> clause;
        int lit;
        while (iss >> lit && lit != 0) {
            clause.insert(lit);
        }
        clauses.push_back(clause);
    }
    return clauses;
}


set<int> resolve(const set<int>& clause1, const set<int>& clause2, int target) {
    set<int> newClause;
    
    for (int lit : clause1) {
        if (lit != target) newClause.insert(lit);
    }
    for (int lit : clause2) {
        if (lit != -target) newClause.insert(lit);
    }
    return newClause;
}

vector<set<int>> filterEssentials(vector<set<int>> clauses) {
    sort(clauses.begin(), clauses.end(), [](const set<int>& a, const set<int>& b) {
        return a.size() < b.size();
    });

    vector<set<int>> filteredClauses;

    for (size_t i = 0; i < clauses.size(); ++i) {
        bool isNecessary = true;

        for (size_t j = 0; j < filteredClauses.size(); ++j) {
            if (includes(clauses[i].begin(), clauses[i].end(), 
                         filteredClauses[j].begin(), filteredClauses[j].end())) {
                isNecessary = false;
                break;
            }
        }

        if (isNecessary) {
            filteredClauses.push_back(clauses[i]);
        }
    }
    return filteredClauses;
}

void printClauses(const vector<set<int>>& clauses) {
    for (const auto& clause : clauses) {
        vector<int> sortedClause(clause.begin(), clause.end());
        sort(sortedClause.begin(), sortedClause.end(), [](int a, int b) {
            return abs(a) < abs(b);
        });

        for (int lit : sortedClause) {
            cout << lit << " ";
        }
        cout << endl;
    }
}

bool iterativeResolution(vector<set<int>>& clauses) {
    set<set<int>> clauseSet(clauses.begin(), clauses.end());      
    bool changed;
    
    do {
        changed = false;
        vector<set<int>> newClauses;
        
        for (size_t i = 0; i < clauses.size(); ++i) {
            for (size_t j = i + 1; j < clauses.size(); ++j) {
                for (int lit : clauses[i]) {
                    if (clauses[j].count(-lit)) {
                        set<int> newClause = resolve(clauses[i], clauses[j], lit);
                        
                        if (newClause.empty()) {
                            cout << "UNSAT" << endl;
                            return false;
                        }

                        if (clauseSet.count(newClause) == 0) {
                            newClauses.push_back(newClause);
                            clauseSet.insert(newClause);
                            changed = true;
                        }

                    }
                }
            }
        }
        // Add all new clauses to the original list
        clauses.insert(clauses.end(), newClauses.begin(), newClauses.end());

    } while (changed);
    vector<set<int>> clauseVector(clauseSet.begin(), clauseSet.end());
    printClauses(clauseVector);
    return true;
}

bool recursiveResolution(vector<set<int>> clauses) {
    vector<set<int>> newClauses;

    for (size_t i = 0; i < clauses.size(); ++i) {
        for (size_t j = i + 1; j < clauses.size(); ++j) {
            for (int lit : clauses[i]) {
                if (clauses[j].count(-lit)) {
                    set<int> newClause = resolve(clauses[i], clauses[j], lit);
                    
                    if (newClause.empty()) {
                        cout << "UNSAT" << endl;
                        return false;
                    }

                    vector<set<int>> newClauses;
                    for (size_t k = 0; k < clauses.size(); ++k) {
                        if (k != i && k != j) {
                            newClauses.push_back(clauses[k]);
                        }
                    }

                    newClauses.push_back(newClause);

                    // cout << "Resolving clauses { ";
                    // for (int l : clauses[i]) cout << l << " ";
                    // cout << "} and { ";
                    // for (int l : clauses[j]) cout << l << " ";
                    // cout << "} on literal " << lit << endl;

                    return recursiveResolution(newClauses);
                }
            }
        }
    }
    clauses = filterEssentials(clauses);
    printClauses(clauses);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <dimacs_file>" << endl;
        return 1;
    }

    string filename = argv[1];
    int numVars;
    vector<set<int>> clauses = readDimacs(filename, numVars);

    if (iterativeResolution(clauses)) {
        cout << "possible SAT" << endl;
    }

    return 0;
}
