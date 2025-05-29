#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <limits>
using namespace std;

vector<int> computePi(const string &s, bool debug = false) {
    int n = s.size();
    vector<int> pi(n, 0);
    if (n > 0) pi[0] = 0;
    if (debug) {
        cout << "Вычисление префикс-функции для: " << s << endl;
        cout << "i\tj\tπ[i]\tСостояние" << endl;
    }

    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        if (debug) {
            cout << i << "\t" << j << "\t";
        }

        while (j > 0 && s[i] != s[j]) {
            if (debug) cout << "Несовпадение: s[" << i << "]=" << s[i] 
                            << " != s[" << j << "]=" << s[j] 
                            << " -> j = π[" << j-1 << "]=" << pi[j-1] << endl;
            j = pi[j - 1];
        }

        if (s[i] == s[j]) {
            if (debug) cout << "Совпадение: s[" << i << "]=" << s[i] 
                            << " == s[" << j << "]=" << s[j] 
                            << " -> j++" << endl;
            ++j;
        } else if (debug && j == 0) {
            cout << "Несовпадение при j=0" << endl;
        }

        pi[i] = j;
        if (debug) {
            cout << pi[i] << "\t";
            for (int k = 0; k <= i; k++) {
                cout << pi[k];
            }
            cout << endl;
        }
    }
    if (debug) {
        cout << "Итоговая префикс-функция: [";
        for (int i = 0; i < n; i++) {
            if (i) cout << ", ";
            cout << pi[i];
        }
        cout << "]" << endl;
    }
    return pi;
}

vector<int> kmpSearch(const string &P, const string &T, bool debug = false) {
    int n = P.size(), m = T.size();
    vector<int> pi = computePi(P, debug);
    vector<int> occurrences;
    int j = 0;

    if (debug) {
        cout << "\nПоиск образца \"" << P << "\" в тексте \"" << T << "\":" << endl;
        cout << "i\tT[i]\tj\tP[j]\tДействие" << endl;
    }

    for (int i = 0; i < m; ++i) {
        if (debug) {
            cout << i << "\t" << T[i] << "\t" << j << "\t";
            if (j < n) cout << P[j];
            else cout << "?";
            cout << "\t";
        }

        while (j > 0 && T[i] != P[j]) {
            if (debug) cout << "Несовпадение -> j = π[" << j-1 << "]=" << pi[j-1] << endl << "\t\t";
            j = pi[j - 1];
        }

        if (T[i] == P[j]) {
            if (debug) cout << "Совпадение -> j++" << endl;
            ++j;
        } else if (debug) {
            cout << "Несовпадение (j=0)" << endl;
        }

        if (j == n) {
            int pos = i - n + 1;
            occurrences.push_back(pos);
            if (debug) {
                cout << "!!! Найдено вхождение с позиции " << pos << " !!!" << endl;
                cout << "Текущий текст: ...";
                int start = max(0, pos-3);
                int end = min(m, pos+n+3);
                for (int k = start; k < end; k++) {
                    if (k == pos) cout << "[";
                    cout << T[k];
                    if (k == pos+n-1) cout << "]";
                }
                cout << endl;
            }
            j = pi[j - 1];
            if (debug) cout << "\t\t" << j << "\t" << (j < n ? string(1, P[j]) : "?") << "\tСдвиг j -> " << j << endl;
        }
    }
    if (debug) {
        if (occurrences.empty()) {
            cout << "Вхождений не найдено" << endl;
        } else {
            cout << "Всего найдено вхождений: " << occurrences.size() << endl;
        }
    }
    return occurrences;
}

int rotationSearch(const string &A, const string &B, bool debug = false) {
    int n = A.size();
    if (n != (int)B.size()) {
        if (debug) cout << "Длины строк не совпадают: " << n << " != " << B.size() << endl;
        return -1;
    }
    vector<int> pi = computePi(B, debug);
    string AA = A + A;
    int j = 0;

    if (debug) {
        cout << "\nПоиск циклического сдвига \"" << B << "\" в \"" << A << "\":" << endl;
        cout << "Строка для поиска: " << AA << endl;
        cout << "i\tAA[i]\tj\tB[j]\tДействие" << endl;
    }

    for (int i = 0; i < (int)AA.size(); ++i) {
        if (debug) {
            cout << i << "\t" << AA[i] << "\t" << j << "\t";
            if (j < n) cout << B[j];
            else cout << "?";
            cout << "\t";
        }

        while (j > 0 && AA[i] != B[j]) {
            if (debug) cout << "Несовпадение -> j = π[" << j-1 << "]=" << pi[j-1] << endl << "\t\t";
            j = pi[j - 1];
        }

        if (AA[i] == B[j]) {
            if (debug) cout << "Совпадение -> j++" << endl;
            ++j;
        } else if (debug) {
            cout << "Несовпадение (j=0)" << endl;
        }

        if (j == n) {
            int pos = i - n + 1;
            if (pos < n) {
                if (debug) {
                    cout << "!!! Найдено совпадение на позиции " << pos << " !!!" << endl;
                    cout << "Сдвиг: " << pos << endl;
                    cout << "Проверка: A[" << pos << "..] = " << A.substr(pos) 
                         << " + " << A.substr(0, pos) << " = " 
                         << A.substr(pos) + A.substr(0, pos) << endl;
                }
                return pos;
            }
            j = pi[j - 1];
            if (debug) cout << "\t\t" << j << "\t" << (j < n ? string(1, B[j]) : "?") << "\tСдвиг j -> " << j << endl;
        }
    }
    if (debug) cout << "Совпадений не найдено" << endl;
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Включить отладочный вывод? (1 - да, 0 - нет): ";
    int debug_mode;
    cin >> debug_mode;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Выберите задачу (1 — поиск вхождений, 2 — циклический сдвиг): ";
    int task;
    if (!(cin >> task)) return 0;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (task == 1) {
        cout << "Введите шаблон P: ";
        string P;
        getline(cin, P);
        cout << "Введите текст T: ";
        string T;
        getline(cin, T);

        auto occ = kmpSearch(P, T, debug_mode);
        if (occ.empty()) {
            cout << "-1\n";
        } else {
            for (size_t i = 0; i < occ.size(); ++i) {
                if (i) cout << ',';
                cout << occ[i];
            }
            cout << '\n';
        }
    }
    else if (task == 2) {
        cout << "Введите строку A: ";
        string A;
        getline(cin, A);
        cout << "Введите строку B: ";
        string B;
        getline(cin, B);

        int shift = rotationSearch(A, B, debug_mode);
        cout << shift << '\n';
    }
    else {
        cout << "Неверный номер задачи" << endl;
    }

    return 0;
}