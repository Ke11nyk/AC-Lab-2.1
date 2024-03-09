#include <iostream>
#include <vector>
#include <random>
#include <Windows.h>

using namespace std;

// ��������� ��� ��������� ������������� �����
struct Rational {
    int num; // ���������
    int den; // ���������
    Rational() { num = 0; den = 1; }
    Rational(int num, int den) : num(num), den(den) {}
};

// ������� ��� ���������� ���������� �������� ������� ���� �����
int gcd(int num, int den) {
    if (den == 0) return num;
    return gcd(den, num % den);
}

// ������� ��� ���������� ������������� �����
void reduce(Rational& r) {
    int g = gcd(r.num, r.den);
    r.num /= g;
    r.den /= g;
}

// ������� ��� ��������� ���� ������������ �����
bool equal(Rational& r1, Rational& r2) {
    return r1.num == r2.num && r1.den == r2.den;
}

// ������� ��� ������ ������������� �����
void print(Rational& r) {
    cout << r.num << "/" << r.den;
}

// ������� ��� ��������� ������� � ������� ������������ �����
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

// ���-������� ��� ������������� �����
int hash_function(Rational& r, int m, int p, int a, int b) {
    if (m == 0 || p == 0) return 0;

    return ((r.num * a + r.den * b) % p) % m; // ���������� ���-��� �� �������� ((r.num * a + r.den * b) mod p) mod m
}

// ������� ��� ��������� ���-������� � ������ ������������ �����
vector<vector<Rational>> create_hash_table(vector<Rational>& arr, vector<vector<int>>& coefficient, int p, int a, int b) {
    vector<vector<Rational>> table(arr.size());
    for (Rational& r : arr) {
        int h = hash_function(r, table.size(), p, a, b);
        coefficient[h][0] = 1;
        table[h].push_back(r);
    }
    return table;
}

// ��������� ��������� ��� �����
void rehash_function(vector<vector<Rational>>& table, vector<vector<int>>& coefficient, int p) {
    for (int i = 0; i < table.size(); i++)
        if (table[i].size() > 1)
        {
            int size = table[i].size() * table[i].size();
            bool collision = true;
            Rational l;
            vector<Rational> fixBuffer(size, l);

            coefficient[i][0] = size;

            // ���� ��� ���������� ����� � �������
            while (collision)
            {
                vector<Rational> buffer(size, l);

                random_device rd; // ��������� ���������� �����
                mt19937 gen(rd()); // ��������� ���������� ����� � ��������� ���������
                uniform_int_distribution<> dis(1, p - 1); // ��������� ������� �� 1 �� p-1

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

// ������� ��� ������ ������������� ����� � ���-�������
bool search(Rational& r, vector<vector<Rational>> table, vector<vector<int>> coefficient, int p, int a, int b) {
    reduce(r);
    int h1 = hash_function(r, table.size(), p, a, b);
    int h2 = hash_function(r, coefficient[h1][0], p, coefficient[h1][1], coefficient[h1][2]);
    if (equal(table[h1][h2], r)) return true;

    return false;
}

// ������� ��� ������ ���-������� � �������
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

// ����� �������� ����� � ���-�������
void find_number(vector<vector<Rational>>& table, vector<vector<int>> coefficient, int p, int a, int b)
{
    int num, den;
    cout << "\n������ ���������: "; cin >> num;
    cout << "������ ���������: "; cin >> den;

    if (num == 0 && den == 1) { cout << "\nInvalid" << endl; return; }

    Rational r(num, den);
    cout << "\n������ ����� ";
    print(r);
    cout << " � ���-�������.\n";
    cout << "��������� ������: " << (search(r, table, coefficient, p, a, b) ? "��������" : "�� ��������") << ".\n";

    if (search(r, table, coefficient, p, a, b))
    {
        int h1 = hash_function(r, table.size(), p, a, b);
        cout << "����� ���-�������: " << h1 << "\n����� ��� �������: " << hash_function(r, coefficient[h1][0], p, coefficient[h1][1], coefficient[h1][2]) << endl;
    }
}

// ������� �������
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // ����� ������������ �����
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

    // �������� ������ ����� p ����� �� ����������� �������� ���������� � ����������
    int p = 13;

    // �������� �������� ����� a �� b �� 1 �� p-1
    random_device rd; // ��������� ���������� �����
    mt19937 gen(rd()); // ��������� ���������� ����� � ��������� ���������
    uniform_int_distribution<> dis(1, p - 1); // ��������� ������� �� 1 �� p-1

    int a = dis(gen);
    int b = dis(gen);

    // ����� ������, ����������� a �� b ��������� ���-�������
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

    // ��������� ���-������� � ������ �����
    remove_duplicates(arr);
    vector<vector<Rational>> table = create_hash_table(arr, coefficient, p, a, b);
    rehash_function(table, coefficient, p);

    char input = '\0';
    // ���� ��� �����䳿 ����������� � ��������
    while (input != '2') {
        std::cout << "\n 0 - ���������� �������\n 1 - ������ �����\n 2 - ����� \n";
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