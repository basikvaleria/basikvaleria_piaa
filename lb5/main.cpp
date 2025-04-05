#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <queue>

using namespace std;

const string MODE_PROMPT = "Select mode (1 - multiple patterns, 2 - wildcard pattern, 3 - non-overlapping): ";
const string DEBUG_PROMPT = "Enable debug output? (1 - yes, 0 - no): ";

struct Node {
    map<char, Node*> children;
    Node* suffix_link;
    Node* terminal_link;
    vector<int> pattern_indices;
    int pattern_length;
    bool is_end;

    Node() : suffix_link(nullptr), terminal_link(nullptr), pattern_length(0),is_end(false) {}
};

class AhoCorasick {
private:
    Node* root;
    vector<string> patterns;
    bool is_wildcard_mode;
    bool debug_mode;

    void print_trie(Node* node, int depth = 0) {
        if (!debug_mode) return;
        for (auto& child : node->children) {
            string indent(depth * 2, ' ');
            cout << indent << "└─ '" << child.first << "' [patterns: ";
            for (int idx : child.second->pattern_indices) cout << idx << " ";
            if (child.second->is_end) cout << "(END)"; 
            cout << "]" << endl;
            print_trie(child.second, depth + 1);
        }
    }

    void print_automaton() {
        if (!debug_mode) return;
        queue<Node*> q;
        q.push(root);
        cout << "\nAutomaton Structure:\n";
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            cout << "Node " << current << ":\n";
            cout << "  Suffix: " << current->suffix_link << "\n";
            cout << "  Terminal: " << current->terminal_link << "\n";
            cout << "  Transitions: ";
            for (auto& ch : current->children) cout << ch.first << "→" << ch.second << " ";
            cout << "\n\n";
            for (auto& child : current->children) q.push(child.second);
        }
    }

    void build_trie() {
        for (int i = 0; i < patterns.size(); ++i) {
            Node* current = root;
            for (char c : patterns[i]) {
                if (!current->children.count(c)) {
                    current->children[c] = new Node();
                }
                current = current->children[c];
            }
            current->is_end = true;
            current->pattern_indices.push_back(i + 1); 
            current->pattern_length = patterns[i].size();
        }

        if (debug_mode) {
            cout << "\nTrie Structure:\n";
            print_trie(root);
        }
    }

    void build_links() {
        queue<Node*> q;
        root->suffix_link = root;

        for (auto& child : root->children) {
            child.second->suffix_link = root;
            q.push(child.second);
        }

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            for (auto& child : current->children) {
                char c = child.first;
                Node* node = child.second;
                Node* temp = current->suffix_link;

                while (temp != root && !temp->children.count(c)) {
                    temp = temp->suffix_link;
                }

                node->suffix_link = (temp->children.count(c)) ? temp->children[c] : root;
                q.push(node);
            }

            if (current->suffix_link->is_end) {
                current->terminal_link = current->suffix_link;
            } else {
                current->terminal_link = current->suffix_link->terminal_link;
            }
        }
        if (debug_mode) print_automaton();
    }

public:
    AhoCorasick(const vector<string>& pats, bool wildcard_mode = false, bool debug = false) 
        : patterns(pats), is_wildcard_mode(wildcard_mode), debug_mode(debug) {
        root = new Node();
        build_trie();
        build_links();
    }

    vector<pair<int, int>> search(const string& text, bool sort_results = true) {
        vector<pair<int, int>> results;
        Node* current = root;

        if (debug_mode) cout << "\nSearch Process:\n";

        for (int i = 0; i < text.size(); ++i) {
            char c = text[i];

            if (debug_mode) cout << "Char: '" << c << "' at pos " << i+1 << endl;
            while (current != root && !current->children.count(c)) {
                if (debug_mode) cout << "  Move to suffix: " << current->suffix_link << endl;
                current = current->suffix_link;
            }

            if (current->children.count(c)) {
                current = current->children[c];
                if (debug_mode) cout << "  New state: " << current << endl;
            } else {
                current = root;
            }

            Node* temp = current;
            while (temp != nullptr) {
                if (!temp->pattern_indices.empty()) {
                    int pos = i - temp->pattern_length + 2;
                    if (debug_mode) {
                        cout << "  Match found: pattern ";
                        for (int idx : temp->pattern_indices) cout << idx << " ";
                        cout << "at pos " << pos << endl;
                    }
                    for (int idx : temp->pattern_indices) {
                        results.emplace_back(pos, idx);
                    }
                }
                temp = temp->terminal_link;
            }
        }

        if (sort_results) {
            sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
                return (a.first != b.first) ? (a.first < b.first) : (a.second < b.second);
            });
        }

        return results;
    }

    ~AhoCorasick() {
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            Node* node = q.front();
            q.pop();
            for (auto& child : node->children) {
                q.push(child.second);
            }
            delete node;
        }
    }
};

vector<pair<int, int>> filter_non_overlapping(const vector<pair<int, int>>& occurrences, const vector<string>& patterns) {
    vector<pair<int, int>> sorted = occurrences;
    sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    vector<pair<int, int>> result;
    int last_end = -1;

    for (const auto& entry : sorted) {
        int start = entry.first;
        int pattern_idx = entry.second - 1; 
        int pattern_len = patterns[pattern_idx].size(); 
        int end = start + pattern_len - 1;

        if (start > last_end) {
            result.push_back(entry);
            last_end = end;
        }
    }

    return result;
}

vector<string> split_pattern(const string& pattern, char wildcard, vector<int>& positions) {
    vector<string> parts;
    string current;
    for (int i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != wildcard) {
            current += pattern[i];
        } else if (!current.empty()) {
            parts.push_back(current);
            positions.push_back(i - current.size());
            current.clear();
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
        positions.push_back(pattern.size() - current.size());
    }
    return parts;
}

void handle_multiple_patterns(bool debug) {
    string text;
    int n;
    cin >> text >> n;
    vector<string> patterns(n);
    for (int i = 0; i < n; ++i) cin >> patterns[i];

    AhoCorasick ac(patterns, debug);
    auto results = ac.search(text);

    for (const auto& res : results) {
        cout << res.first << " " << res.second << endl;
    }
}

void handle_wildcard_pattern(bool debug) {
    string text, pattern;
    char wildcard;
    cin >> text >> pattern >> wildcard;

    vector<int> positions;
    vector<string> parts = split_pattern(pattern, wildcard, positions);
    if (parts.empty()) return;

    AhoCorasick ac(parts, debug);
    auto occurrences = ac.search(text);

    vector<int> C(text.size() + 1, 0);
    for (auto& occ : occurrences) {
        int expected_start = occ.first - positions[occ.second - 1];
        if (expected_start >= 1) C[expected_start]++;
    }

    int pattern_len = pattern.size();
    for (int i = 1; i <= text.size() - pattern_len + 1; ++i) {
        if (C[i] == parts.size()) cout << i << endl;
    }
}

void handle_non_overlapping(bool debug) {
    string text;
    int n;
    cin >> text >> n;
    vector<string> patterns(n);
    for (int i = 0; i < n; ++i) cin >> patterns[i];

    AhoCorasick ac(patterns, debug);
    auto results = ac.search(text);
    auto filtered = filter_non_overlapping(results, patterns);

    for (const auto& res : filtered) {
        cout << res.first << " " << res.second << endl;
    }
}

int main() {
    bool debug;
    cout << DEBUG_PROMPT;
    cin >> debug;

    int mode;
    cout << MODE_PROMPT;
    cin >> mode;
    cin.ignore();

    switch (mode) {
        case 1: 
            handle_multiple_patterns(debug);
            break;
        case 2: 
            handle_wildcard_pattern(debug);
            break;
        case 3: 
            handle_non_overlapping(debug);
            break;
        default:
            cerr << "Invalid mode selected!" << endl;
            return 1;
    }

    return 0;
}