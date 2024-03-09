#include <iostream>
#include <vector>
#include <random>
#include <Windows.h>

using namespace std;

// Структура для зберігання раціонального числа
struct Rational {
    int num; // чисельник
    int den; // знаменник
    Rational() { num = 0; den = 1; }
    Rational(int num, int den) : num(num), den(den) {}
};

// Функція для обчислення найбільшого спільного дільника двох чисел
int gcd(int num, int den) {
    if (den == 0) return num;
    return gcd(den, num % den);
}

// Функція для скорочення раціонального числа
void reduce(Rational& r) {
    int g = gcd(r.num, r.den);
    r.num /= g;
    r.den /= g;
}

// Функція для порівняння двох раціональних чисел
bool equal(Rational& r1, Rational& r2) {
    return r1.num == r2.num && r1.den == r2.den;
}

// Функція для виводу раціонального числа
void print(Rational& r) {
    cout << r.num << "/" << r.den;
}

// Функція для видалення повторів з вектора раціональних чисел
void remove_duplicates(vector<Rational>& v) {
    vector<Rational> s;
    bool repetition = false;

    for (Rational& r : v)
    {
        reduce(r);
        for (Rational& l : s)
            if (r.num == l.num && r.den == l.den) repetition = true;
        if (!repetition) s.push_back(r);
        repetition = false;
    }

    v.clear();

    for (const Rational r : s)
        v.push_back(r);
}

// Хеш-функція для раціонального числа
int hash_function(Rational& r, int m, int p, int a, int b) {
    if (m == 0 || p == 0) return 0;

    return ((r.num * a + r.den * b) % p) % m; // обчислюємо хеш-код за формулою ((r.num * a + r.den * b) mod p) mod m
}

// Функція для створення хеш-таблиці з масиву раціональних чисел
vector<vector<Rational>> create_hash_table(vector<Rational>& arr, vector<vector<int>>& coefficient, int p, int a, int b) {
    vector<vector<Rational>> table(arr.size());
    for (Rational& r : arr) {
        int h = hash_function(r, table.size(), p, a, b);
        coefficient[h][0] = 1;
        table[h].push_back(r);
    }
    return table;
}

// Створення підтаблиць для колізій
void rehash_function(vector<vector<Rational>>& table, vector<vector<int>>& coefficient, int p) {
    for (int i = 0; i < table.size(); i++)
        if (table[i].size() > 1)
        {
            int size = table[i].size() * table[i].size();
            bool collision = true;
            Rational l;
            vector<Rational> fixBuffer(size, l);

            coefficient[i][0] = size;

            // Цикл для запобігання колізій в кошиках
            while (collision)
            {
                vector<Rational> buffer(size, l);

                random_device rd; // генератор випадкових чисел
                mt19937 gen(rd()); // генератор випадкових чисел з рівномірним розподілом
                uniform_int_distribution<> dis(1, p - 1); // рівномірний розподіл від 1 до p-1

                int a = dis(gen);
                int b = dis(gen);

                coefficient[i][1] = a;
                coefficient[i][2] = b;

                for (Rational& r : table[i]) {
                    int h = hash_function(r, buffer.size(), p, a, b);

                    if (buffer[h].num != l.num && buffer[h].den != l.den) collision = true;
                    else collision = false;

                    buffer[h] = r;
                }

                fixBuffer = buffer;
            }

            table[i] = fixBuffer;
        }
}

// Функція для пошуку раціонального числа в хеш-таблиці
bool search(Rational& r, vector<vector<Rational>> table, vector<vector<int>> coefficient, int p, int a, int b) {
    reduce(r);
    int h1 = hash_function(r, table.size(), p, a, b);
    int h2 = hash_function(r, coefficient[h1][0], p, coefficient[h1][1], coefficient[h1][2]);
    if (equal(table[h1][h2], r)) return true;

    return false;
}

// Функція для виводу хеш-таблиці у консоль
void print_hash_table(vector<vector<Rational>> table, vector<vector<int>> coefficient) {
    for (int i = 0; i < table.size(); i++) {
        for (int j = 0; j < 3; j++)
            cout << coefficient[i][j] << " ";
        cout << "| ";
        for (Rational& r : table[i]) {
            print(r);
            cout << " ";
        }
        cout << "\n";
    }
}

// Пошук заданого числа в хеш-таблиці
void find_number(vector<vector<Rational>>& table, vector<vector<int>> coefficient, int p, int a, int b)
{
    int num, den;
    cout << "\nВведіть чисельник: "; cin >> num;
    cout << "Введіть знаменник: "; cin >> den;

    if (num == 0 && den == 1) { cout << "\nInvalid" << endl; return; }

    Rational r(num, den);
    cout << "\nШукаємо число ";
    print(r);
    cout << " в хеш-таблиці.\n";
    cout << "Результат пошуку: " << (search(r, table, coefficient, p, a, b) ? "знайдено" : "не знайдено") << ".\n";

    if (search(r, table, coefficient, p, a, b))
    {
        int h1 = hash_function(r, table.size(), p, a, b);
        cout << "Перша хеш-функція: " << h1 << "\nДруга хеш функція: " << hash_function(r, coefficient[h1][0], p, coefficient[h1][1], coefficient[h1][2]) << endl;
    }
}

// Головна функція
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Масив раціональних чисел
    vector<Rational> arr = {
        Rational(1, 2),
        Rational(1, 2),
        Rational(3, 4),
        Rational(5, 6),
        Rational(7, 8),
        Rational(9, 10),
        Rational(2, 3),
        Rational(4, 5),
        Rational(6, 7),
        Rational(8, 9),
        Rational(10, 11)
    };

    // Вибираємо просте число p більше за максимальне значення чисельника і знаменника
    int p = 13;

    // Вибираємо випадкові числа a та b від 1 до p-1
    random_device rd; // генератор випадкових чисел
    mt19937 gen(rd()); // генератор випадкових чисел з рівномірним розподілом
    uniform_int_distribution<> dis(1, p - 1); // рівномірний розподіл від 1 до p-1

    int a = dis(gen);
    int b = dis(gen);

    // Розмір кошиків, коефіцієнти a та b внутрішньої хеш-функції
    vector<vector<int>> coefficient = { {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0} };

    // Створюємо хеш-таблицю з масиву чисел
    remove_duplicates(arr);
    vector<vector<Rational>> table = create_hash_table(arr, coefficient, p, a, b);
    rehash_function(table, coefficient, p);

    char input = '\0';
    // Цикл для взаємодії користувача і програми
    while (input != '2') {
        std::cout << "\n 0 - відобразити таблицю\n 1 - знайти число\n 2 - вийти \n";
        std::cin >> input;
        switch (input)
        {
        case(48):
            cout << "\na: " << a << " b: " << b << " p: " << p << "\n\n";
            print_hash_table(table, coefficient);
            break;
        case(49):
            find_number(table, coefficient, p, a, b);
            break;
        case(50):
            break;
        default:
            std::cout << "\n Invalid command \n";
            break;
        }
    }

    return 0;
}