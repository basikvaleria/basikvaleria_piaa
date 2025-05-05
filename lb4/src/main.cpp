#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <limits>
using namespace std;

vector<int> computePi(const string &s) {
    int n = s.size();
    vector<int> pi(n);
    pi[0] = 0;
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) {
            j = pi[j - 1];
        }
        if (s[i] == s[j]) {
            ++j;
        }
        pi[i] = j;
    }
    return pi;
}

vector<int> kmpSearch(const string &P, const string &T) {
    int n = P.size(), m = T.size();
    vector<int> pi = computePi(P);
    vector<int> occurrences;
    int j = 0;
    for (int i = 0; i < m; ++i) {
        while (j > 0 && T[i] != P[j]) {
            j = pi[j - 1];
        }
        if (T[i] == P[j]) {
            ++j;
        }
        if (j == n) {
            occurrences.push_back(i - n + 1);
            j = pi[j - 1];
        }
    }
    return occurrences;
}

int rotationSearch(const string &A, const string &B) {
    int n = A.size();
    if (n != (int)B.size()) return -1;
    vector<int> pi = computePi(B);
    string AA = A + A;
    int j = 0;
    for (int i = 0; i < (int)AA.size(); ++i) {
        while (j > 0 && AA[i] != B[j]) {
            j = pi[j - 1];
        }
        if (AA[i] == B[j]) {
            ++j;
        }
        if (j == n) {
            int pos = i - n + 1;
            if (pos < n) {
                return pos;
            }
            j = pi[j - 1];
        }
    }
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Выберите задачу (1 — поиск вхождений, 2 — циклический сдвиг):" << endl;
    int task;
    if (!(cin >> task)) return 0;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (task == 1) {
        cout << "Введите шаблон P:" << endl;
        string P;
        getline(cin, P);
        cout << "Введите текст T:" << endl;
        string T;
        getline(cin, T);

        auto occ = kmpSearch(P, T);
        if (occ.empty()) {
            cout << -1 << '\n';
        } else {
            for (size_t i = 0; i < occ.size(); ++i) {
                if (i) cout << ',';
                cout << occ[i];
            }
            cout << '\n';
        }
    }
    else if (task == 2) {
        cout << "Введите строку A:" << endl;
        string A;
        getline(cin, A);
        cout << "Введите строку B:" << endl;
        string B;
        getline(cin, B);

        int shift = rotationSearch(A, B);
        cout << shift << '\n';
    }
    else {
        cout << "Неверный номер задачи" << endl;
    }

    return 0;
}
