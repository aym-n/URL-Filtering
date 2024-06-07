#include <bitset>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/*
Polynomial Rolling Hashing Function 
is a hash function that uses only multiplications and additions

Hash(s) = ( s[0] + s[1]*p + s[2]*p^2 + ... ) % m
*/
unsigned long long polynomial_hash(const string &s, long long p, long long m)
{
    unsigned long long hash = 0;
    long long p_pow = 1;
    for (char c : s)
    {
        hash = (hash + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash;
}

/*
DJB2
This algorithm (k=33) was first reported by Dan Bernstein many years ago in comp.lang.c. 
Another version of this algorithm (now favored by Bernstein) uses XOR: 

hash(i) = hash(i - 1) * 33 ^ str[i]; 

The magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained. 
*/

unsigned long long djb2(const string &s)
{
    unsigned long long hash = 5381;
    for (char c : s)
    {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

/*
SDBM
This algorithm was created for sdbm (a public-domain reimplementation of ndbm)
database library.
It was found to do well in scrambling bits, causing better distribution of the keys
and fewer splits.
It also happens to be a good general hashing function with good distribution.
The actual function (pseudo code) is:
    for i in i..len(str):
        hash(i) = hash(i - 1) * 65599 + str[i];
*/

unsigned long long sdbm(const string &s)
{
    unsigned long long hash = 0;
    for (char c : s)
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

class BloomFilter
{
private:
    long long p = 31;
    long long m = 1e9 + 9;  // Using 1e9 + 9 for practical purposes
    long long size;
    string filename;
    bitset<1000001> bits;  // Correctly sized bitset

public:
    BloomFilter(string filename) : size(0), filename(filename), bits(0)
    {
        cout << "--- Initializing BloomFilter ---" << endl;
        ifstream file(filename);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                cout << "Added " << line << endl;
                add(line);
                size++;
            }
            file.close();
        }
        else
        {
            cerr << "Unable to open file: " << filename << endl;
        }

        cout << "--- BloomFilter Initialized ---" << endl;
        cout << "Total Rows Added: " << size << endl;
        cout << "Total Size: " << bits.size() << " bits" << endl;
    }

    void add(const string &s)
    {
        auto hash = polynomial_hash(s, p, m);
        auto hash2 = djb2(s) % m;
        auto hash3 = sdbm(s) % m;
        bits.set(hash % bits.size());
        bits.set(hash2 % bits.size());
        bits.set(hash3 % bits.size());
    }

    bool contains(const string &s)
    {
        auto hash = polynomial_hash(s, p, m);
        auto hash2 = djb2(s) % m;
        auto hash3 = sdbm(s) % m;

        return bits.test(hash % bits.size()) && bits.test(hash2 % bits.size()) && bits.test(hash3 % bits.size());
    }

    void test(const string &filename)
    {
        int positives = 0;
        int negatives = 0;
        ifstream file(filename);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                cout << "Checking " << line << " : " << contains(line) << endl;
                positives += contains(line);
                negatives += !contains(line);
            }
            file.close();
        }
        else
        {
            cerr << "Unable to open file: " << filename << endl;
        }

        cout << "Total Positives: " << positives << endl;
        cout << "Total Negatives: " << negatives << endl;
    }

    bitset<1000001> get_bits()
    {
      return bits;
    }
};

int main()
{
    BloomFilter bf("malicious.csv");
    bf.test("malicious.csv");
    getchar();
    bf.test("benign.csv");

    cout << "All bits set: " << bf.get_bits().all() << endl;
    return 0;
}
