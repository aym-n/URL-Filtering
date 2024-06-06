#include<iostream>
#include<string>
#include<fstream>
#include<bitset>

using namespace std;

/*
Polynomial Rolling Hashing Function 
is a hash function that uses only multiplications and additions

Hash(s) = ( s[0] + s[1]*p + s[2]*p^2 + ... ) % m
*/
unsigned long polynomial_hash(string s, long long p, long long m) {
    unsigned long hash = 0;
    long long p_pow = 1;
    for (char c : s) {
        hash = (hash + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash;
}

/*
DJB2
this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. 
another version of this algorithm (now favored by bernstein) uses xor: 

hash(i) = hash(i - 1) * 33 ^ str[i]; 

the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained. 
*/

unsigned long dbj2(string s){
    unsigned long hash = 5381;
    for (char c : s) {
        hash = ((hash << 5) + hash) + c;
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

unsigned long sdbm(string s){
    unsigned long hash = 0;
    for (char c : s) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

class BloomFilter {
    private:
        long long p = 31;
        long long m = 10e9 + 1;
        long long size;
        string filename;
    public:
        bitset<10^9 + 1> bits(0);
        BloomFilter(string filename){
            this->size = 0;
            cout << bits.to_string() << endl;
            cout << "--- Initializing BloomFilter ---"<< endl;
            getchar();
            ifstream file(filename);
            if(file.is_open()){
                string line;
                while(getline(file, line)){
                    cout << "Added " << line << endl;
                    add(line);
                    size++;
                }
                file.close();
            }

            cout << "--- BloomFilter Initialized ---" << endl;
            cout << "Total Rows Added: " << size << endl;
            cout << "Total Size " << sizeof(bits) << endl;
        }

        void add(string s){
            long long hash = polynomial_hash(s, p, m);
            long long hash2 = dbj2(s);
            long long hash3 = sdbm(s);
            bits.set(hash);
            bits.set(hash2);
            bits.set(hash3);
        }

        bool contains(string s){
            long long hash = polynomial_hash(s, p, m);
            long long hash2 = dbj2(s);
            long long hash3 = sdbm(s);

            return bits.test(hash) && bits.test(hash2) && bits.test(hash3);
        }

        void test(string filename){
            int positives = 0;
            int negatives = 0;
            ifstream file(filename);
            if(file.is_open()){
                string line;
                while(getline(file, line)){
                    cout << "Checking " << line << " : " << contains(line) << endl;
                    positives += contains(line);
                    negatives += !contains(line);
                }

                cout << "Total Positives: " << positives << endl;
                cout << "Total Negatives: " << negatives << endl;
                file.close();
            }
        }
};

int main(){
    BloomFilter bf("malicious.csv");
    bf.test("malicious.csv");
    getchar();
    bf.test("benign.csv");

    cout << bf.bits.all() << endl;
}

