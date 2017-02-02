#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <set>
#ifndef ONLINE_JUDGE
#       include <fstream>
#endif

using namespace std;

struct slot {
    int column;
    int row;
    bool down;
};

struct node {
    unsigned char matrix[10][10];
    unsigned int used_words; // a vector seems to be slower
    unsigned char last_filled_slot;
};

vector<slot> slots;
vector<string> words;
string output = "";

void print_matrix(const struct node& n) {
        for (int i = 0; i < 10; ++i) {
                for (int j = 0; j < 10; ++j) {
                        const auto c = n.matrix[i][j];
                        if (c == '\0')
                                cout << ' ';
                        else
                                cout << c;
                }
                cout << "\n";
        }
}

// to avoid creating some can_generate_next_node() method
static bool error;
inline struct node generate_next_node(const struct node& n, const int w)
{
        error = false;
        const auto word = words[w];
        const auto slot = slots[n.last_filled_slot];
        struct node next = {0};
        memcpy(next.matrix, n.matrix, 100);
        if (slot.down) {
                for (int i = 0; i < word.length(); i++) {
                        const auto dd = n.matrix[slot.row + i][slot.column];
                        if (dd != 0 && dd != word[i]) {
                                error = true;
                                return next;
                        }
                        next.matrix[slot.row + i][slot.column] = word[i];
                }
        } else {
                for (int i = 0; i < word.length(); i++) {
                        const auto dd = n.matrix[slot.row][slot.column + i];
                        if (dd != 0 && dd != word[i]) {
                                error = true;
                                return next;
                        }
                        next.matrix[slot.row][slot.column + i] = word[i];
                }
        }
        next.used_words = n.used_words;
        next.used_words |= 1 << w;
        next.last_filled_slot = n.last_filled_slot + 1;
        return next;
}

inline vector<struct node> get_children(const struct node& n) {
        vector<node> result;
        for (int i = 0; i < words.size(); ++i) {
                const auto b = (n.used_words >> i) & 1;
                if (b == 0) {
                        const auto newn = generate_next_node(n, i);
                        if (!error) {
                                result.push_back(newn);
                        }
                }
        }
       return result;
}

// we can assume that the result will not be empty!
vector<node> get_roots() {
        vector<node> result;
        const auto slot = slots[0];
        for (int i = 0; i < words.size(); ++i) {
                struct node n = {0};
                const auto word = words[i];
                if (slot.down) {
                        for (int j = 0; j < word.length(); j++) {
                                n.matrix[slot.row + j][slot.column] = word[j];
                        }
                } else {
                        for (int j = 0; j < word.length(); j++) {
                                n.matrix[slot.row][slot.column + j] = word[j];
                        }
                }
                n.used_words |= 1 << i;
                n.last_filled_slot = 1;
                result.push_back(n);
        }
        return result;
}

void processBacktracking() {
        auto stack = get_roots();
        set<unsigned int> results;
        while (!stack.empty()) {
                const auto n = stack.back();
                stack.pop_back();
                //cout << "\n"; print_matrix(n);
                if (n.last_filled_slot == words.size() - 1) {
                        //cout << "\nResult: \n"; print_matrix(n);
                        results.insert(n.used_words);
                } else {
                        const auto b = get_children(n);
                        for (const auto& bb : b) {
                                stack.push_back(bb);
                        }
                }
        }
        if (results.empty()) {
                output += " Impossible";
        } else {
                for (const auto& n : results) {
                        for (int i = 0; i < words.size(); ++i) {
                                const auto b = (n >> i) & 1;
                                if (b == 0) {
                                        output += " " + words[i];
                                        break;
                                }
                        }
                }
        }
}

int main() {
#ifndef ONLINE_JUDGE
        ifstream cin("test-input.txt");
#endif
        int n = 0;
        int testCase = 0;

        while (cin >> n && n != 0) {
                slots.clear();
                words.clear();

                int row, column;
                char direction;
                for (int i = 0; i < n; ++i) {
                        cin >> row >> column >> direction;
                        slots.push_back(slot{column - 1, row - 1,
                                             direction == 'D'});
                }

                for (int i = 0; i <= n; i++) {
                        string word;
                        cin >> word;
                        words.push_back(word);
                }

                output += "Trial " + to_string(++testCase) + ":";
                processBacktracking();
                output += "\n\n";
        }
        cout << output;

        return 0;
}