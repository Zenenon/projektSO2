#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <iomanip>

using namespace std;

class Mutex {
private:
    volatile bool locked;
public:
    Mutex() : locked(false) {}
    void lock() {
        while (__sync_lock_test_and_set(&locked, 1)) {
            // Busy wait until unlocked
        }
    }
    void unlock() {
        __sync_lock_release(&locked);
    }
};

int num_philosophers;
Mutex* forks;
Mutex print_mutex;

void log_state(int id, const string& state, int duration = 0) {
    print_mutex.lock();
    if (state == "thinks" || state == "eats") {
        cout << setw(id * 8) << "" << id + 1 << " " << state << " " << duration << "ms\n";
    } else {
        cout << setw(id * 8) << "" << id + 1 << " " << state << "\n";
    }
    print_mutex.unlock();
}

void* philosopher(void* arg) {
    int id = *(int*)arg;
    delete (int*)arg;

    while (true) {
        int think_time = (rand() % 300) + 200;
        log_state(id, "thinks", think_time);
        usleep(think_time * 1000);

        log_state(id, "is hungry");
        if (id % 2 == 0) {
            forks[id].lock();
            forks[(id + 1) % num_philosophers].lock();
        } else {
            forks[(id + 1) % num_philosophers].lock();
            forks[id].lock();
        }

        int eat_time = (rand() % 600) + 200;
        log_state(id, "eats", eat_time);
        usleep(eat_time * 1000);

        forks[id].unlock();
        forks[(id + 1) % num_philosophers].unlock();
    }
}

int main() {
    cout << "Enter the number of philosophers: ";
    cin >> num_philosophers;

    if (num_philosophers < 2) {
        cerr << "There must be at least 2 philosophers." << endl;
        return 1;
    }

    pthread_t threads[num_philosophers];
    forks = new Mutex[num_philosophers];

    for (int i = 0; i < num_philosophers; i++) {
        int* id = new int(i);
        pthread_create(&threads[i], nullptr, philosopher, id);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], nullptr);
    }

    delete[] forks;
    return 0;
}
