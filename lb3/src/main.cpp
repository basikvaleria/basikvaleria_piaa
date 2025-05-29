#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <tuple>
#include <iomanip>
using namespace std;

const long long INF = 1e18;

void printDP(const vector<vector<long long>>& dp, const string& A, const string& B) {
    int n = A.size();
    int m = B.size();
    
    cout << "        ";
    for (int j = 0; j <= m; j++) {
        if (j == 0) cout << "ε" << "\t";
        else cout << B[j-1] << "\t";
    }
    cout << endl;
    
    for (int i = 0; i <= n; i++) {
        if (i == 0) cout << "ε ";
        else cout << A[i-1] << " ";
        
        cout << "| ";
        for (int j = 0; j <= m; j++) {
            if (dp[i][j] == INF) cout << "∞" << "\t";
            else cout << dp[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void task1() {
    string line;
    getline(cin, line);
    istringstream iss(line);
    vector<int> costs;
    int cost_val;
    while (iss >> cost_val) {
        costs.push_back(cost_val);
    }

    int cost_replace, cost_insert, cost_delete, cost_double_insert;
    bool has_double_insert = false;
    if (costs.size() == 4) {
        has_double_insert = true;
        cost_replace = costs[0];
        cost_insert = costs[1];
        cost_delete = costs[2];
        cost_double_insert = costs[3];
        cout << "Параметры: Замена=" << cost_replace << ", Вставка=" << cost_insert
             << ", Удаление=" << cost_delete << ", ДвойнаяВставка=" << cost_double_insert << endl;
    } else if (costs.size() == 3) {
        cost_replace = costs[0];
        cost_insert = costs[1];
        cost_delete = costs[2];
        cout << "Параметры: Замена=" << cost_replace << ", Вставка=" << cost_insert
             << ", Удаление=" << cost_delete << endl;
    } else {
        return;
    }

    string A, B;
    getline(cin, A);
    getline(cin, B);
    cout << "Строка A: " << A << endl;
    cout << "Строка B: " << B << endl << endl;

    int n = A.size();
    int m = B.size();

    vector<vector<long long>> dp(n + 1, vector<long long>(m + 1, 0));

    for (int i = 1; i <= n; i++) {
        dp[i][0] = dp[i - 1][0] + cost_delete;
    }
    for (int j = 1; j <= m; j++) {
        dp[0][j] = dp[0][j - 1] + cost_insert;
    }

    cout << "Матрица после инициализации:" << endl;
    printDP(dp, A, B);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            cout << "--- Обработка (" << i << "," << j << "): A[" << i-1 << "]=" << A[i-1] 
                 << ", B[" << j-1 << "]=" << B[j-1] << " ---" << endl;
                 
            long long option1 = (A[i - 1] == B[j - 1]) ? dp[i - 1][j - 1] : dp[i - 1][j - 1] + cost_replace;
            long long option2 = dp[i][j - 1] + cost_insert;
            long long option3 = dp[i - 1][j] + cost_delete;

            cout << "  Вариант 1 (совпадение/замена): " << option1 << endl;
            cout << "  Вариант 2 (вставка): " << option2 << endl;
            cout << "  Вариант 3 (удаление): " << option3 << endl;

            dp[i][j] = min({option1, option2, option3});
            cout << "  Минимум: " << dp[i][j] << endl;

            if (has_double_insert && j >= 2 && B[j - 1] == B[j - 2]) {
                long long option4 = dp[i][j - 2] + cost_double_insert;
                cout << "  Вариант 4 (двойная вставка): " << option4;
                if (option4 < dp[i][j]) {
                    dp[i][j] = option4;
                    cout << " -> НОВЫЙ МИНИМУМ!" << endl;
                } else {
                    cout << " -> не минимум" << endl;
                }
            }
            cout << "  DP[" << i << "][" << j << "] = " << dp[i][j] << endl << endl;
        }
        cout << "Состояние матрицы после строки " << i << ":" << endl;
        printDP(dp, A, B);
    }

    cout << "Финальная стоимость преобразования: " << dp[n][m] << endl << endl;
}

void task2() {
    string line;
    getline(cin, line);
    istringstream iss(line);
    vector<int> costs;
    int cost_val;
    while (iss >> cost_val) {
        costs.push_back(cost_val);
    }

    int cost_replace, cost_insert, cost_delete, cost_double_insert;
    bool has_double_insert = false;
    if (costs.size() == 4) {
        has_double_insert = true;
        cost_replace = costs[0];
        cost_insert = costs[1];
        cost_delete = costs[2];
        cost_double_insert = costs[3];
        cout << "Параметры: Замена=" << cost_replace << ", Вставка=" << cost_insert
             << ", Удаление=" << cost_delete << ", ДвойнаяВставка=" << cost_double_insert << endl;
    } else if (costs.size() == 3) {
        cost_replace = costs[0];
        cost_insert = costs[1];
        cost_delete = costs[2];
        cout << "Параметры: Замена=" << cost_replace << ", Вставка=" << cost_insert
             << ", Удаление=" << cost_delete << endl;
    } else {
        return;
    }

    string A, B;
    getline(cin, A);
    getline(cin, B);
    cout << "Строка A: " << A << endl;
    cout << "Строка B: " << B << endl << endl;

    int n = A.size();
    int m = B.size();

    vector<vector<long long>> dp(n + 1, vector<long long>(m + 1, INF));
    vector<vector<tuple<int, int, char>>> parent(n + 1, vector<tuple<int, int, char>>(m + 1, make_tuple(-1, -1, ' ')));

    dp[0][0] = 0;

    for (int i = 1; i <= n; i++) {
        dp[i][0] = dp[i - 1][0] + cost_delete;
        parent[i][0] = make_tuple(i - 1, 0, 'D');
    }
    for (int j = 1; j <= m; j++) {
        dp[0][j] = dp[0][j - 1] + cost_insert;
        parent[0][j] = make_tuple(0, j - 1, 'I');
    }

    cout << "Матрица после инициализации:" << endl;
    printDP(dp, A, B);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            cout << "--- Обработка (" << i << "," << j << "): A[" << i-1 << "]=" << A[i-1] 
                 << ", B[" << j-1 << "]=" << B[j-1] << " ---" << endl;
                 
            if (A[i - 1] == B[j - 1]) {
                if (dp[i - 1][j - 1] < dp[i][j]) {
                    dp[i][j] = dp[i - 1][j - 1];
                    parent[i][j] = make_tuple(i - 1, j - 1, 'M');
                    cout << "  Совпадение (M): " << dp[i][j] << endl;
                }
            } else {
                if (dp[i - 1][j - 1] + cost_replace < dp[i][j]) {
                    dp[i][j] = dp[i - 1][j - 1] + cost_replace;
                    parent[i][j] = make_tuple(i - 1, j - 1, 'R');
                    cout << "  Замена (R): " << dp[i][j] << endl;
                }
            }

            if (dp[i][j - 1] + cost_insert < dp[i][j]) {
                dp[i][j] = dp[i][j - 1] + cost_insert;
                parent[i][j] = make_tuple(i, j - 1, 'I');
                cout << "  Вставка (I): " << dp[i][j] << endl;
            }

            if (dp[i - 1][j] + cost_delete < dp[i][j]) {
                dp[i][j] = dp[i - 1][j] + cost_delete;
                parent[i][j] = make_tuple(i - 1, j, 'D');
                cout << "  Удаление (D): " << dp[i][j] << endl;
            }

            if (has_double_insert && j >= 2 && B[j - 1] == B[j - 2]) {
                if (dp[i][j - 2] + cost_double_insert < dp[i][j]) {
                    dp[i][j] = dp[i][j - 2] + cost_double_insert;
                    parent[i][j] = make_tuple(i, j - 2, 'W');
                    cout << "  Двойная вставка (W): " << dp[i][j] << endl;
                }
            }
            
            cout << "  DP[" << i << "][" << j << "] = " << dp[i][j] << endl;
            cout << "  Операция: " << get<2>(parent[i][j]) << endl << endl;
        }
        cout << "Состояние матрицы после строки " << i << ":" << endl;
        printDP(dp, A, B);
    }

    int i = n, j = m;
    string ops = "";

    cout << "Восстановление операций:" << endl;
    while (i > 0 || j > 0) {
        tuple<int, int, char> current = parent[i][j];
        char op = get<2>(current);
        ops += op;
        int pi = get<0>(current);
        int pj = get<1>(current);
        
        cout << "  Шаг: (" << i << "," << j << ") -> (" << pi << "," << pj << ") операция: ";
        switch (op) {
            case 'M': cout << "Совпадение"; break;
            case 'R': cout << "Замена"; break;
            case 'I': cout << "Вставка"; break;
            case 'D': cout << "Удаление"; break;
            case 'W': cout << "Двойная вставка"; break;
        }
        cout << endl;
        
        i = pi;
        j = pj;
    }

    reverse(ops.begin(), ops.end());
    cout << "Последовательность операций: " << ops << endl;
}

void task3() {
    string S, T;
    getline(cin, S);
    getline(cin, T);
    cout << "Строка S: " << S << endl;
    cout << "Строка T: " << T << endl << endl;

    int n = S.size();
    int m = T.size();

    vector<int> prev(m + 1);
    for (int j = 0; j <= m; ++j) {
        prev[j] = j;
    }

    cout << "Инициализация: ";
    for (int j = 0; j <= m; ++j) cout << prev[j] << " ";
    cout << endl << endl;

    for (int i = 1; i <= n; ++i) {
        vector<int> curr(m + 1);
        curr[0] = i;
        for (int j = 1; j <= m; ++j) {
            cout << "Обработка (" << i << "," << j << "): S[" << i-1 << "]=" << S[i-1] 
                 << ", T[" << j-1 << "]=" << T[j-1];
            
            if (S[i - 1] == T[j - 1]) {
                curr[j] = prev[j - 1];
                cout << " -> Совпадение: " << curr[j] << endl;
            } else {
                curr[j] = min({prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + 1});
                cout << " -> Минимум: " << curr[j] 
                     << " (Удаление:" << prev[j]+1 
                     << ", Вставка:" << curr[j-1]+1 
                     << ", Замена:" << prev[j-1]+1 << ")" << endl;
            }
        }
        prev = curr;
        cout << "После строки " << i << ": ";
        for (int j = 0; j <= m; ++j) cout << prev[j] << " ";
        cout << endl << endl;
    }

    cout << "Редакционное расстояние: " << prev[m] << endl;
}

int main() {
    int choice;
    cout << "Выберите задачу (1, 2, 3): ";
    cin >> choice;
    cin.ignore(); 

    switch (choice) {
        case 1:
            task1();
            break;
        case 2:
            task2();
            break;
        case 3:
            task3();
            break;
        default:
            cout << "Неверный выбор" << endl;
            return 1;
    }

    return 0;
}