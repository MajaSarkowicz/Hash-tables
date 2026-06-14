#include <random>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <climits>

using namespace std;

// Rozmiary struktur do testowania wydajnosci
const int MEASUREMENT_POINTS[8] = {5000, 10000, 15000, 20000, 25000, 30000, 35000, 40000};
const int KEY_RANGE = 100000;
const int VAL_RANGE = 100000;

template <typename T> struct testItem {
    T structure;
    vector<int> key_cache;
};

template <typename T> vector<T> create_reiteration_copies(const testItem<T>& item, int reiterations){
    // Tworzenie wielu kopii struktury do wielokrotnych pomiarow
    vector<T> reiter_copies;
    for(int i = 0; i < reiterations; i++){
        reiter_copies.push_back(item.structure);
    }
    return reiter_copies;
}

template <typename T> testItem<T> populate_structure(int seed, int size, double cap_multiplier){
    // Tworzenie struktury z losowymi danymi
    mt19937 rng_key((((seed*3)%4)+9)*7);
    mt19937 rng_val(seed*2+15);
    uniform_int_distribution<int> dist_key(-KEY_RANGE, KEY_RANGE);
    uniform_int_distribution<int> dist_val(-VAL_RANGE, VAL_RANGE);
    int key;
    int val;
    testItem<T> testitem;
    testitem.structure = T(size*cap_multiplier);
    for(int i = 0; i<size; i++){
        key = dist_key(rng_key);
        val = dist_val(rng_val);
        testitem.structure.insert(key, val);
        testitem.key_cache.push_back(key);
    }
    cout << "    completed insertion of " << size << " items." << endl;
    return testitem;
}

template <typename T> vector<testItem<T>> generate_test_structures(int batch, double cap_multiplier){
    // Generowanie zestawu struktur o roznych rozmiarach do testowania
    vector<testItem<T>> structures;
    for(int i = 0; i<size(MEASUREMENT_POINTS); i++){
        structures.push_back(populate_structure<T>(batch, MEASUREMENT_POINTS[i], cap_multiplier));
    }
    return structures;
}

template <typename T> double test_insert(testItem<T> structure, int reiterations, int value, int scenario){
    auto reiter_copies = create_reiteration_copies<T>(structure, reiterations);
    vector<int> keys;
    random_device seed;
    mt19937 rng(seed());
    if(scenario == 2){
        //keys don't appear in structure
        std::uniform_int_distribution<int> dist(KEY_RANGE+1, 2*KEY_RANGE+1);
        for(int i = 0; i < reiterations; i++){
            keys.push_back(dist(rng));
        }
    }
    else if(scenario == 1){
        //random keys in orignal key range
        std::uniform_int_distribution<int> dist(-KEY_RANGE, KEY_RANGE);
        for(int i = 0; i < reiterations; i++){
            keys.push_back(dist(rng));
        }
    }
    else{
        //keys chosen from the actual structure 
        std::uniform_int_distribution<int> dist(0, structure.key_cache.size() - 1);
        for(int i = 0; i < reiterations; i++){
            keys.push_back(structure.key_cache.at(dist(rng)));
        }
    }
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < reiterations; i++) {
        reiter_copies.at(i).insert(keys.at(i), value);
    }
    auto end = chrono::steady_clock::now();
    return chrono::duration<double>(end - start).count() / reiterations;   
}

template <typename T> double test_remove(testItem<T> structure, int reiterations){
    auto reiter_copies = create_reiteration_copies<T>(structure, reiterations);
    vector<int> keys;
    random_device seed;
    mt19937 rng(seed());
    std::uniform_int_distribution<int> dist(0, structure.key_cache.size() - 1);
    for(int i = 0; i < reiterations; i++){
        keys.push_back(structure.key_cache.at(dist(rng)));
    }
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < reiterations; i++) {
        reiter_copies.at(i).remove(keys.at(i));
    }
    auto end = chrono::steady_clock::now();
    return chrono::duration<double>(end - start).count() / reiterations; 
}

template <typename T> void run_avl(int seeds, int reiterations){
    vector<vector<testItem<T>>> structures_optimistic;
    vector<vector<testItem<T>>> structures_average;
    vector<vector<testItem<T>>> structures_pessimistic;
    for(int i = 0; i < seeds; i++){
        cout << "Seed " << i << ":" << endl;
        structures_optimistic.push_back(generate_test_structures<T>(i, 10));
        structures_average.push_back(generate_test_structures<T>(i, 2.5));
        structures_pessimistic.push_back(generate_test_structures<T>(i, 1.1));
        cout << "Seed " << i << " generation complete." << endl;
    }
    for(int i = 0; i<size(MEASUREMENT_POINTS); i++){
        fstream size_file("avl_size" + to_string(MEASUREMENT_POINTS[i]) + ".csv", ios::out);
        size_file << "seed, insert_optimistic, insert_avg, insert_pessimistic, remove_optimistic, remove_avg, remove_pessimistic\n";
        for(int j = 0; j < seeds; j++){
            cout << "  seed " << j << ": ";
            double insert_opt = test_insert<T>(structures_optimistic[j][i], reiterations, 4522, 1);
            double insert_avg = test_insert<T>(structures_average[j][i], reiterations, 43533, 1);
            double insert_pes = test_insert<T>(structures_pessimistic[j][i], reiterations, 1234, 1);
            double remove_opt = test_remove<T>(structures_optimistic[j][i], reiterations);
            double remove_avg = test_remove<T>(structures_average[j][i], reiterations);
            double remove_pes = test_remove<T>(structures_pessimistic[j][i], reiterations);
            cout << "done" << endl;
            size_file << j << "," << insert_opt << "," << insert_avg << "," << insert_pes << "," << remove_opt << "," << remove_avg << "," << remove_pes << "\n";
        }
        size_file.close();
    }
}

template <typename T> void run_oa(int seeds, int reiterations){
    vector<vector<testItem<T>>> structures_optimistic;
    vector<vector<testItem<T>>> structures_average;
    vector<vector<testItem<T>>> structures_pessimistic;
    for(int i = 0; i < seeds; i++){
        cout << "Seed " << i << ":" << endl;
        structures_optimistic.push_back(generate_test_structures<T>(i, 20));
        structures_average.push_back(generate_test_structures<T>(i, 2.5));
        structures_pessimistic.push_back(generate_test_structures<T>(i, 1.1));
        cout << "Seed " << i << " generation complete." << endl;
    }
    for(int i = 0; i<size(MEASUREMENT_POINTS); i++){
        fstream size_file("oa_size" + to_string(MEASUREMENT_POINTS[i]) + ".csv", ios::out);
        size_file << "seed, insert_optimistic, insert_avg, insert_pessimistic, remove_optimistic, remove_avg, remove_pessimistic\n";
        for(int j = 0; j < seeds; j++){
            cout << "  seed " << j << ": ";
            double insert_opt = test_insert<T>(structures_optimistic[j][i], reiterations, 4522, 1);
            double insert_avg = test_insert<T>(structures_average[j][i], reiterations, 43533, 1);
            double insert_pes = test_insert<T>(structures_pessimistic[j][i], reiterations, 1234, 1);
            double remove_opt = test_remove<T>(structures_optimistic[j][i], reiterations);
            double remove_avg = test_remove<T>(structures_average[j][i], reiterations);
            double remove_pes = test_remove<T>(structures_pessimistic[j][i], reiterations);
            cout << "done" << endl;
            size_file << j << "," << insert_opt << "," << insert_avg << "," << insert_pes << "," << remove_opt << "," << remove_avg << "," << remove_pes << "\n";
        }
        size_file.close();
    }
}

template <typename T> void run_cuckoo(int seeds, int reiterations){
    vector<vector<testItem<T>>> structures_optimistic;
    vector<vector<testItem<T>>> structures_average;
    vector<vector<testItem<T>>> structures_pessimistic;
    for(int i = 0; i < seeds; i++){
        cout << "Seed " << i << ":" << endl;
        structures_optimistic.push_back(generate_test_structures<T>(i, 10));
        structures_average.push_back(generate_test_structures<T>(i, 2.5));
        structures_pessimistic.push_back(generate_test_structures<T>(i, 1.1));
        cout << "Seed " << i << " generation complete." << endl;
    }
    for(int i = 0; i<size(MEASUREMENT_POINTS); i++){
        fstream size_file("cuckoo_size" + to_string(MEASUREMENT_POINTS[i]) + ".csv", ios::out);
        size_file << "seed, insert_optimistic, insert_avg, insert_pessimistic, remove_optimistic, remove_avg, remove_pessimistic\n";
        for(int j = 0; j < seeds; j++){
            cout << "  seed " << j << ": ";
            double insert_opt = test_insert<T>(structures_optimistic[j][i], reiterations, 4522, 0);
            double insert_avg = test_insert<T>(structures_average[j][i], reiterations, 43533, 1);
            double insert_pes = test_insert<T>(structures_pessimistic[j][i], reiterations, 1234, 2);
            double remove_opt = test_remove<T>(structures_optimistic[j][i], reiterations);
            double remove_avg = test_remove<T>(structures_average[j][i], reiterations);
            double remove_pes = test_remove<T>(structures_pessimistic[j][i], reiterations);
            cout << "done" << endl;
            size_file << j << "," << insert_opt << "," << insert_avg << "," << insert_pes << "," << remove_opt << "," << remove_avg << "," << remove_pes << "\n";
        }
        size_file.close();
    }
}