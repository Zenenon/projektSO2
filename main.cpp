#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>

constexpr const size_t N = 5;  // Number of philosophers (and forks)

enum class State { THINKING, HUNGRY, EATING };

// Get left and right neighbor indices
size_t inline left(size_t i) { return (i - 1 + N) % N; }
size_t inline right(size_t i) { return (i + 1) % N; }

// Philosophers' states
State state[N];

// Spinlock instead of mutex
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// Synchronization
SpinLock critical_region_lock;

// Fork availability (true = free, false = taken)
std::atomic<bool> forks[N] = { true, true, true, true, true };

// Random time generator
size_t random_time(size_t min, size_t max) {
    static thread_local std::mt19937 rnd(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(rnd);
}

// Print function with consistent formatting
void print_status(size_t i, const std::string& status) {
    std::cout << "[" << i << "] " << status << "\n";
}

// Try to eat if forks are available
void test(size_t i) {
    if (state[i] == State::HUNGRY && forks[i] && forks[right(i)]) {
        forks[i] = false;
        forks[right(i)] = false;
        state[i] = State::EATING;
        print_status(i, "\t\tis EATING");
    }
}

// Thinking phase
void think(size_t i) {
    size_t duration = random_time(400, 800);
    print_status(i, "is THINKING for " + std::to_string(duration) + "ms");
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

// Pick up forks (or wait)
void take_forks(size_t i) {
    critical_region_lock.lock();
    state[i] = State::HUNGRY;
    print_status(i, "\t\tis HUNGRY");
    test(i);
    critical_region_lock.unlock();

    while (state[i] != State::EATING) {
        std::this_thread::yield();
    }
}

// Eating phase
void eat(size_t i) {
    size_t duration = random_time(400, 800);
    print_status(i, "\t\t is eating for " + std::to_string(duration) + "ms");
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

// Put down forks
void put_forks(size_t i) {
    critical_region_lock.lock();
    state[i] = State::THINKING;
    forks[i] = true;
    forks[right(i)] = true;
    print_status(i, "\t\tis DONE eating and put down forks");
    test(left(i));
    test(right(i));
    critical_region_lock.unlock();
}

// Philosopher function
void philosopher(size_t i) {
    while (true) {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

// Start threads
int main() {
    std::cout << "Dining Philosophers Problem - Simulation Start\n\n";

    std::vector<std::thread> philosophers;
    for (size_t i = 0; i < N; i++) {
        philosophers.emplace_back(philosopher, i);
    }

    for (auto& p : philosophers) {
        p.join();
    }

    return 0;
}
