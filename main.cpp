#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
using namespace std;

// parameter
//////////////////
int individual_number = 40;   // 個体生成数
int generation_number = 2000; // 世代数
/////////////////

// あらかじめ配置を固定するキーを定義
/*
layout Index(A-Z + ;,./)
 .---------------------------------------------------------------------.
 |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  9   |
 |---------------------------------------------------------------------|
 |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
 |---------------------------------------------------------------------|
 |  20  |  21  |  22  |  23  |  24  |  25  |  26  |  27  |  28  |  29  |
 ,---------------------------------------------------------------------,
*/
// key, index
map<int, int> fix_key = {
    {(int)('Z' - 65), 20}, // Zを20番に固定
    {(int)('X' - 65), 21},
    {(int)('C' - 65), 22},
    {(int)('V' - 65), 23},
    {(int)('A' - 65), 10},
    {(int)('Q' - 65), 0},
    {(int)('W' - 65), 1},
    {(int)('J' - 65), 26},
    {(int)('K' - 65), 27},
    {(int)('H' - 65), 25},
    {(int)('L' - 65), 28},
    {(int)('O' - 65), 11},
    {(int)('E' - 65), 12},
    {(int)('I' - 65), 13},
    {(int)('U' - 65), 14},
};

/*
Scores of arrangement
 .-----------------------------------------------------------.
 |  1  |  3  |  4  |  3  |  1  |  1  |  3  |  4  |  3  |  1  |
 |-----------------------------------------------------------|
 |  5  |  6  |  6  |  6  |  3  |  3  |  6  |  6  |  6  |  5  |
 |-----------------------------------------------------------|
 |  1  |  2  |  2  |  3  |  1  |  1  |  3  |  2  |  2  |  1  |
 ,-----------------------------------------------------------,
*/
int scores[] = {
    1, 3, 4, 3, 1, 1, 3, 4, 3, 1,
    5, 6, 6, 6, 2, 2, 6, 6, 6, 5,
    1, 2, 2, 3, 1, 1, 3, 2, 2, 1};

// Template
template <class T>
void shuffle(T ary[], int size)
{
    for (int i = 0; i < size; i++)
    {
        bool shuffle_flg = true;
        int j = rand() % size;
        for (auto itr = fix_key.begin(); itr != fix_key.end(); ++itr)
            if (itr->second == i || itr->second == j)
                shuffle_flg = false;
        if (shuffle_flg)
        {
            T t = ary[i];
            ary[i] = ary[j];
            ary[j] = t;
        }
    }
}

struct keymap
{
    int key[30];
    double fitness;
};

void show_keymap(int keymap[])
{
    for (int i = 0; i < 30; i++)
    {
        if (i % 10 == 0)
            cout << ".-----------------------------------------------------------." << endl;
        cout << "|  " << (char)(keymap[i] + 65) << "  ";
        if (i % 10 == 9)
            cout << "|" << endl;
        if (i == 29)
            cout << ".-----------------------------------------------------------." << endl;
    }
}

void initialize(struct keymap keymaps[], bool do_shuffle = true)
{
    // initialize
    for (int i = 0; i < individual_number; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            keymaps[i].key[j] = -1;
        }
    }
   int sp[] = {(int)';' - 65, (int)',' - 65, (int)'.' - 65, (int)'/' - 65};
    for (int i = 0; i < individual_number; i++)
    {
        // fix keys
        for (auto itr = fix_key.begin(); itr != fix_key.end(); ++itr)
        {
            keymaps[i].key[itr->second] = itr->first;
        }
        // A-Z
        for (int j = 0; j < 26; j++) // alphabet loop
        {
            if (fix_key.find(j) == fix_key.end()) // not found
                for (int k = 0; k < 30; k++)      // array loop
                    if (keymaps[i].key[k] == -1)
                    {
                        keymaps[i].key[k] = j;
                        break;
                    }
        }
        // ;,./
        for (auto s : sp) // spetial word loop
        {
            if (fix_key.find(s) == fix_key.end())
                for (int j = 0; j < 30; j++) // array loop
                {
                    if (keymaps[i].key[j] == -1)
                    {
                        keymaps[i].key[j] = s;
                        break;
                    }
                }
        }
        if (do_shuffle)
            shuffle<int>(keymaps[i].key, 30);
    }
}

// 単語の出現割合を返す(http://www7.plala.or.jp/dvorakjp/hinshutu.htm)
double get_word_freqency(int key)
{
    map<int, int> index;
    for (int i = 0; i < 26; i++)
    {
        index[i] = i;
    }
    index[-6] = 26;
    index[-21] = 27;
    index[-19] = 28;
    index[-18] = 29;
    int word_freqency[] = {
        3452, 672, 1306, 1582, 4663, 811, 744, 1739, 2914, 84,
        366, 1583, 1092, 2767, 2882, 827, 34, 2543, 2850, 3445,
        1207, 408, 694, 72, 733, 32, 32, 440, 661, 0};
    int sum_of_word_freqency = 40869;

    int rome_freqency[] = {
        25130, 1683, 0, 3939, 13794, 39, 3791, 5365, 23949, 0,
        12097, 0, 5028, 15233, 21651, 389, 0, 9544, 10856, 13626,
        18122, 16, 4366, 0, 7605, 2818, 5, 2905, 2012, 0};
    int sum_of_rome_freqency = 203984;

    return (double)word_freqency[index[key]] / sum_of_word_freqency + (double)rome_freqency[index[key]] / sum_of_rome_freqency;
}

void GA(struct keymap keymaps[])
{
    int crossing_number = individual_number / 2; // 1世代で交叉する数
    int elite_index = 0;                         // エリートのインデックス
    struct local
    {
        // エリート以外を交叉(というより近傍解生成)
        static void do_crossing(int crossing_number, struct keymap keymaps[], int elite)
        {
            for (int i = 0; i < crossing_number; i++)
            {
                int cross_index = elite;
                while (cross_index == elite)
                    cross_index = rand() % individual_number;

                int cross1 = -1, cross2 = -1;
                while (cross1 == cross2)
                {
                    cross1 = rand() % 30;
                    cross2 = rand() % 30;
                    for (auto itr = fix_key.begin(); itr != fix_key.end(); ++itr)
                        if (itr->second == cross1 || itr->second == cross2)
                        {
                            cross1 = cross2 = -1;
                            break;
                        }
                }
                int tmp = keymaps[cross_index].key[cross1];
                keymaps[cross_index].key[cross1] = keymaps[cross_index].key[cross2];
                keymaps[cross_index].key[cross2] = tmp;
            }
        }
        // 適応度計算
        static void calculate_fitness(struct keymap keymaps[])
        {
            for (int i = 0; i < individual_number; i++)
            {
                double fitness = 0.0;
                for (int j = 0; j < 30; j++)
                {
                    fitness += scores[j] * get_word_freqency(keymaps[i].key[j]);
                }
                keymaps[i].fitness = fitness;
            }
        }
    };

    for (int loop = 0; loop < generation_number; loop++)
    {
        local::do_crossing(crossing_number, keymaps, elite_index);
        local::calculate_fitness(keymaps);
        // エリート保存
        double max_fitness = -1;
        for (int j = 0; j < individual_number; j++)
        {
            if (keymaps[j].fitness > max_fitness)
            {
                max_fitness = keymaps[j].fitness;
                elite_index = j;
            }
        }

        // エリート表示
        if (loop % 100 == 99)
        {
            cout << "Generation: " << loop + 1 << " fitness: " << keymaps[elite_index].fitness << endl;
            show_keymap(keymaps[elite_index].key);
        }
    }
    return;
}

int main(int argc, char const *argv[])
{
    srand((unsigned)time(NULL));
    struct keymap keymaps[individual_number];
    initialize(keymaps, true);
    GA(keymaps);

    return 0;
}