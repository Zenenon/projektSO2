#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <atomic>
using namespace std;

int myrand(int min, int max) {
    static mt19937 rnd(time(nullptr));
    return uniform_int_distribution<>(min, max)(rnd);
}

class SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(memory_order_acquire)) {
            this_thread::yield();
        }
    }
    void unlock() {
        flag.clear(memory_order_release);
    }
};

void philosopher(int ph, SpinLock& ma, SpinLock& mb, SpinLock& mo) {
    for (;;) {
        int duration = myrand(200, 800);
        {
            mo.lock();
            cout << ph << " thinks " << duration << "ms\n";
            mo.unlock();
        }
        this_thread::sleep_for(chrono::milliseconds(duration));

        {
            mo.lock();
            cout << "\t\t" << ph << " is hungry\n";
            mo.unlock();
        }

        ma.lock();
        mb.lock();
        duration = myrand(200, 800);
        {
            mo.lock();
            cout << "\t\t\t\t" << ph << " eats " << duration << "ms\n";
            mo.unlock();
        }
        this_thread::sleep_for(chrono::milliseconds(duration));
        ma.unlock();
        mb.unlock();
    }
}

int main() {
    cout << "Dining Philosophers C++11 with Resource hierarchy\n";

    int n;
    cout << "Enter number of philosophers: ";
    cin >> n;

    vector<SpinLock> forks(n);
    SpinLock mo;
    vector<thread> philosophers;

    for (int i = 0; i < n; ++i) {
        philosophers.emplace_back([&, i] { philosopher(i + 1, forks[i], forks[(i + 1) % n], mo); });
    }

    for (auto& t : philosophers) {
        t.join();
    }
}
