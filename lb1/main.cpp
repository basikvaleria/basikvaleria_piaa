#include <iostream>
#include <vector>
#include <stack>
#include <random>
#include <png.h>
#include <climits>
#include <cstdint>

using namespace std;

struct Square {
    int x, y, w;
};

class BitGrid {
    int N;
    vector<uint32_t> rows;

public:
    BitGrid(int n) : N(n), rows(n, 0) {}

    bool canPlace(int x, int y, int size) const {
        if (x + size > N || y + size > N) return false;
        uint32_t mask = ((1U << size) - 1) << (N - y - size);
        for (int i = x; i < x + size; ++i) {
            if ((rows[i] & mask) != 0) return false;
        }
        return true;
    }

    void place(int x, int y, int size) {
        uint32_t mask = ((1U << size) - 1) << (N - y - size);
        for (int i = x; i < x + size; ++i) {
            rows[i] |= mask;
        }
    }

    void unplace(int x, int y, int size) {
        uint32_t mask = ~(((1U << size) - 1) << (N - y - size));
        for (int i = x; i < x + size; ++i) {
            rows[i] &= mask;
        }
    }

    int findFirstEmpty() const {
        for (int i = 0; i < N; ++i) {
            if (rows[i] != (1U << N) - 1) {
                for (int j = 0; j < N; ++j) {
                    if (!(rows[i] & (1U << (N - j - 1)))) {
                        return i * N + j;
                    }
                }
            }
        }
        return -1;
    }

    bool isFilled() const {
        uint32_t full = (1U << N) - 1;
        for (int i = 0; i < N; ++i) {
            if (rows[i] != full) return false;
        }
        return true;
    }
};

pair<int, int> ScaleSize(int N) {
    for (int d = 2; d * d <= N; ++d) {
        if (N % d == 0) return {d, N / d};
    }
    return {N, 1};
}

vector<Square> upscale(const vector<Square>& squares, int scale) {
    vector<Square> result;
    for (const auto& sq : squares) {
        int x = (sq.x - 1) * scale + 1;
        int y = (sq.y - 1) * scale + 1;
        int w = sq.w * scale;
        result.push_back({x, y, w});
    }
    return result;
}

vector<Square> solveOriginal(int N) {
    if (N == 1) return {{1, 1, 1}};

    stack<pair<BitGrid, vector<Square>>> stack;
    BitGrid initial(N);
    vector<Square> squares;

    int mainSize = (N + 1) / 2;
    initial.place(0, 0, mainSize);
    squares.push_back({1, 1, mainSize});

    if (N - mainSize > 0) {
        initial.place(0, mainSize, N - mainSize);
        squares.push_back({1, mainSize + 1, N - mainSize});
        initial.place(mainSize, 0, N - mainSize);
        squares.push_back({mainSize + 1, 1, N - mainSize});
    }

    stack.push({initial, squares});
    vector<Square> best;
    int minCount = INT_MAX;

    while (!stack.empty()) {
        auto top = stack.top();
        BitGrid grid = top.first;
        vector<Square> current = top.second;
        stack.pop();

        if (current.size() >= minCount) continue;
        if (grid.isFilled()) {
            if (current.size() < minCount) {
                minCount = current.size();
                best = current;
            }
            continue;
        }

        int pos = grid.findFirstEmpty();
        if (pos == -1) continue;

        int x = pos / N;
        int y = pos % N;

        for (int s = min(N - x, N - y); s >= 1; --s) {
            if (!grid.canPlace(x, y, s)) continue;

            BitGrid newGrid = grid;
            newGrid.place(x, y, s);
            vector<Square> newSquares = current;
            newSquares.push_back({x + 1, y + 1, s});

            stack.push({newGrid, newSquares});
        }
    }

    return best;
}

vector<Square> solveScaled(int N) {
    pair<int, int> scalePair = ScaleSize(N);
    int d = scalePair.first;
    int scale = scalePair.second;
    if (scale == 1) return solveOriginal(N);
    auto subSolution = solveOriginal(d);
    return upscale(subSolution, scale);
}

void saveImage(const string& filename, int N, const vector<Square>& squares, int scale = 50) {
    int width = N * scale;
    int height = N * scale;

    vector<unsigned char> pixels(width * height * 3, 255);

    mt19937 gen(42);
    uniform_int_distribution<int> dist(0, 255);

    for (const auto& sq : squares) {
        int x_start = (sq.x - 1) * scale;
        int y_start = (sq.y - 1) * scale;
        int size = sq.w * scale;

        int r = dist(gen);
        int g = dist(gen);
        int b = dist(gen);

        for (int y = y_start; y < y_start + size; ++y) {
            for (int x = x_start; x < x_start + size; ++x) {
                int index = (y * width + x) * 3;
                pixels[index] = r;
                pixels[index + 1] = g;
                pixels[index + 2] = b;
            }
        }

        for (int x = x_start; x < x_start + size; ++x) {
            int index = (y_start * width + x) * 3;
            pixels[index] = 0;
            pixels[index + 1] = 0;
            pixels[index + 2] = 0;

            index = ((y_start + size - 1) * width + x) * 3;
            pixels[index] = 0;
            pixels[index + 1] = 0;
            pixels[index + 2] = 0;
        }
        for (int y = y_start; y < y_start + size; ++y) {
            int index = (y * width + x_start) * 3;
            pixels[index] = 0;
            pixels[index + 1] = 0;
            pixels[index + 2] = 0;

            index = (y * width + x_start + size - 1) * 3;
            pixels[index] = 0;
            pixels[index + 1] = 0;
            pixels[index + 2] = 0;
        }
    }

    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        cerr << "Ошибка создания PNG структуры!" << endl;
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        cerr << "Ошибка создания PNG info!" << endl;
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        cerr << "Ошибка записи PNG!" << endl;
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    vector<png_bytep> rows(height);
    for (int y = 0; y < height; ++y) {
        rows[y] = &pixels[y * width * 3];
    }

    png_write_image(png, rows.data());
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(fp);
}


int main() {
    int N;
    cin >> N;
    auto result = solveScaled(N);
    cout << result.size() << endl;
    for (const auto& sq : result) {
        cout << sq.x << " " << sq.y << " " << sq.w << endl;
    }

    saveImage("output.png", N, result);
    return 0;
}