#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
using namespace std;

// Template
template <class T>
void shuffle(T ary[], int size)
{
    for (int i = 0; i < size; i++)
    {
        int j = rand() % size;
        T t = ary[i];
        ary[i] = ary[j];
        ary[j] = t;
    }
}
/*
QWERTY layout
 .-----------------------------------------------------------.
 |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |
 |-----------------------------------------------------------|
 |  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  ;  |
 |-----------------------------------------------------------|
 |  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |
 ,-----------------------------------------------------------,
*/

/*
ASCII Index(A-Z + ;,./)
 .------------------------------------------------------------------------.
 |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7    |  8    |  9    |
 |------------------------------------------------------------------------|
 |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17   |  18   |  19   |
 |------------------------------------------------------------------------|
 |  20  |  21  |  22  |  23  |  24  |  25  |  -6  |  -21  |  -19  |  -18  |
 ,------------------------------------------------------------------------,
*/

/*
Scores of arrangement
 .-----------------------------------------------------------.
 |  1  |  3  |  4  |  3  |  2  |  2  |  3  |  4  |  3  |  1  |
 |-----------------------------------------------------------|
 |  5  |  5  |  5  |  5  |  3  |  3  |  5  |  5  |  5  |  5  |
 |-----------------------------------------------------------|
 |  1  |  2  |  2  |  3  |  2  |  2  |  3  |  2  |  2  |  1  |
 ,-----------------------------------------------------------,
*/
int scores[] = {
    1, 3, 4, 3, 2, 2, 3, 4, 3, 1,
    5, 5, 5, 5, 3, 3, 5, 5, 5, 5,
    1, 2, 2, 3, 2, 2, 3, 2, 2, 1};

// parameter
//////////////////
int individual_number = 60; // 個体生成数
int generation_number = 8000; // 世代数
/////////////////

void show_keymap(int keymap[])
{
    cout << "Fitness: " << keymap[30] << endl
         << endl;
    for (int i = 0; i < 30; i++)
    {
        if (i % 10 == 0)
            cout << ".-----------------------------------------------------------." << endl;
        cout << "|  " << (char)(keymap[i] + 65) << "  ";
        // cout << "|  " << keymap[i] << "  ";
        if (i % 10 == 9)
            cout << "|" << endl;
        if (i == 29)
            cout << ".-----------------------------------------------------------." << endl;
    }
}

void initialize(int keymap[], bool do_shuffle = true)
{
    // A-Z
    for (int i = 0; i < 26; i++)
    {
        keymap[i] = i;
    }
    // ; , . /
    keymap[26] = 59 - 65;
    keymap[27] = 44 - 65;
    keymap[28] = 46 - 65;
    keymap[29] = 47 - 65;

    if (do_shuffle)
        shuffle<int>(keymap, 30);
}

// 単語の出現回数を返す(http://www7.plala.or.jp/dvorakjp/hinshutu.htm)
int get_word_freqency(int key)
{
    int word_freqency[] = {
        3452, 672, 1306, 1582, 4663, 811, 744, 1739, 2914, 84,
        366, 1583, 1092, 2767, 2882, 827, 34, 2543, 2850, 3445,
        1207, 408, 694, 72, 733, 32};

    switch (key)
    {
    case -6:
        return 32; // ;
    case -21:
        return 440; // ,
    case -19:
        return 661; // .
    case -18:
        0; // /
    default:
        return word_freqency[key];
    }

    return 0;
}

void GA(int keymap[][31])
{
    int crossing_number = individual_number / 2; // 1世代で交叉する数
    int elite_index = 0;                         // エリートのインデックス
    struct local
    {
        // エリート以外を交叉(というより近傍解生成)
        static void do_crossing(int crossing_number, int keymap[][31], int elite)
        {
            for (int i = 0; i < crossing_number; i++)
            {
                int cross_index = elite;
                while (cross_index == elite)
                    cross_index = rand() % individual_number;
                int cross1 = 0, cross2 = 0;
                while (cross1 == cross2)
                {
                    cross1 = rand() % 30;
                    cross2 = rand() % 30;
                }
                int tmp = keymap[cross_index][cross1];
                keymap[cross_index][cross1] = keymap[cross_index][cross2];
                keymap[cross_index][cross2] = tmp;
            }
        }
        // 適応度計算
        static void calculate_fitness(int keymap[][31])
        {
            for (int i = 0; i < individual_number; i++)
            {
                int fitness = 0;
                for (int j = 0; j < 30; j++)
                {
                    fitness += scores[j] * get_word_freqency(keymap[i][j]);
                }
                keymap[i][30] = fitness;
            }
        }
    };

    for (int loop = 0; loop < generation_number; loop++)
    {
        local::do_crossing(crossing_number, keymap, elite_index);
        local::calculate_fitness(keymap);
        // エリート保存
        int max_fitness = -1;
        for (int j = 0; j < individual_number; j++)
        {
            if (keymap[j][30] > max_fitness)
            {
                max_fitness = keymap[j][30];
                elite_index = j;
            }
        }

        // エリート表示
        cout << "Generation: " << loop << endl;
        show_keymap(keymap[elite_index]);
    }
}

int main(int argc, char const *argv[])
{
    srand((unsigned)time(NULL));
    int keymap[individual_number][31] = {}; // keymap30 + fitness

    for (int i = 0; i < individual_number; i++)
        initialize(keymap[i], false);

    // GA開始
    GA(keymap);

    return 0;
}