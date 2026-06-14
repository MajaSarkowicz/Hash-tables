#include <iostream>
#include <exception>
#include "../Cuckoo.hpp"
#include "../OpenAddressing.hpp"
#include "../AVLChain.hpp"
#include "benchmarks.hpp"

using namespace std;


int main(){
    // Glowne menu wyboru struktury danych do testowania
    int choice = -1;
    while(choice != 0){
        cout << "Wybierz strukture:" << endl 
        << "1. AVL" << endl
        << "2. OpenAddressing" << endl
        << "3. Cuckoo" << endl
        << "0. Wyjdz" << endl
        << ">";
        cin >> choice;
        switch (choice)
        {
        case 1:
            cout << "Starting benchmark for ChainAVL<int, int>..." << endl;
            run_avl<ChainAVL<int, int>>(4, 100);
            cout << "Benchmark completed successfully." << endl;
            break;
        case 2:
            cout << "Starting benchmark for OpenAddressing<int, int>..." << endl;
            run_oa<OpenAddressing<int, int>>(4, 100);
            cout << "Benchmark completed successfully." << endl;
            break;
        case 3:
            cout << "Starting benchmark for CuckooHashTable<int, int>..." << endl;
            run_cuckoo<CuckooHashTable<int, int>>(4, 100);
            cout << "Benchmark completed successfully." << endl;
            break;
        }
    }
}