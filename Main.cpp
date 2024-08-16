#include <future>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

bool make_thread = true; 
unsigned int thread_count; 
unsigned int active_thread_count = 1; 

void merge(int* arr, int l, int m, int r) {
    int nl = m - l + 1;
    int nr = r - m;

    vector<int> left(nl);
    vector<int> right(nr);

    for (int i = 0; i < nl; i++)
        left[i] = arr[l + i];
    for (int j = 0; j < nr; j++)
        right[j] = arr[m + 1 + j];

    int i = 0, j = 0;
    int k = l;

    while (i < nl && j < nr) {
        if (left[i] <= right[j]) {
            arr[k] = left[i++];
        } else {
            arr[k] = right[j++];
        }
        k++;
    }

    while (i < nl) {
        arr[k++] = left[i++];
    }

    while (j < nr) {
        arr[k++] = right[j++];   
    }
}

void mergeSort(int* arr, int left, int right) {
    if (left >= right)
        return;

    int middle = left + (right - left) / 2;

    if (make_thread && (active_thread_count < thread_count) && (right - left > 10000)) {
        active_thread_count++;
        future<void> f = async(launch::async, [&]() {
            mergeSort(arr, left, middle);
            active_thread_count--;
        });
        mergeSort(arr, middle + 1, right);
        merge(arr, left, middle, right);
        f.get();  
    } else {
        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

int main() {
    thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0)
        thread_count = 2;
    cout << thread_count << " concurrent threads are supported.\n";

    srand(0);
    long arr_size = 10000000;
    int* array = new int[arr_size];
    if (array == nullptr) return 0;

    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 500000;
    }

    cout << "Running single-threaded merge sort..." << endl;
    make_thread = false;
    active_thread_count = 1;  // Сброс счетчика потоков
    auto start = chrono::high_resolution_clock::now();

    mergeSort(array, 0, arr_size - 1);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Single-threaded merge sort execution time: " << elapsed.count() << " seconds\n";

    for (long i = 0; i < arr_size - 1; i++) {
        if (array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }

    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 500000;
    }

    start = chrono::high_resolution_clock::now();

    cout << "Running multithreaded merge sort..." << endl;
    make_thread = true;
    mergeSort(array, 0, arr_size - 1);

    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Multithreaded merge sort execution time: " << elapsed.count() << " seconds\n";

    delete[] array;

    return 0;
}
