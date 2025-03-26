#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>

constexpr const size_t N = 5;  // Liczba filozofów i widelców

enum class State { THINKING, HUNGRY, EATING };

// Funkcje zwracające indeksy sąsiadów filozofa
size_t inline left(size_t i) { return (i - 1 + N) % N; }
size_t inline right(size_t i) { return (i + 1) % N; }

// Stan każdego filozofa
State state[N];

// Spinlock do synchronizacji (zamiast mutexa)
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // Unikamy aktywnego blokowania CPU
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// Spinlock do synchronizacji dostępu do sekcji krytycznej
SpinLock critical_region_lock;

// Flagi widelców (true = wolny, false = zajęty)
std::atomic<bool> forks[N] = { true, true, true, true, true };

// Generator losowych czasów
size_t random_time(size_t min, size_t max) {
    static thread_local std::mt19937 rnd(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(rnd);
}

// Sprawdza, czy filozof może jeść
void test(size_t i) {
    if (state[i] == State::HUNGRY && forks[i] && forks[right(i)]) {
        // Jeśli oba widelce są wolne, filozof zaczyna jeść
        forks[i] = false;
        forks[right(i)] = false;
        state[i] = State::EATING;
    }
}

// Filozof myśli przez losowy czas
void think(size_t i) {
    size_t duration = random_time(400, 800);
    std::cout << i << " myśli przez " << duration << "ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

// Filozof próbuje podnieść widelce
void take_forks(size_t i) {
    critical_region_lock.lock();  // Sekcja krytyczna
    state[i] = State::HUNGRY;
    std::cout << i << " jest głodny.\n";
    test(i);  // Sprawdza, czy może jeść
    critical_region_lock.unlock();

    // Jeśli nie zdobył widelców, czeka aktywnie (unikamy semaforów)
    while (state[i] != State::EATING) {
        std::this_thread::yield();
    }
}

// Filozof je przez losowy czas
void eat(size_t i) {
    size_t duration = random_time(400, 800);
    std::cout << i << " je przez " << duration << "ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

// Filozof odkłada widelce
void put_forks(size_t i) {
    critical_region_lock.lock();  // Sekcja krytyczna
    state[i] = State::THINKING;
    forks[i] = true;
    forks[right(i)] = true;
    std::cout << i << " odkłada widelce.\n";

    // Sprawdza, czy sąsiedzi mogą jeść
    test(left(i));
    test(right(i));
    critical_region_lock.unlock();
}

// Funkcja filozofa
void philosopher(size_t i) {
    while (true) {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

// Uruchamianie wątków filozofów
int main() {
    std::vector<std::thread> philosophers;
    for (size_t i = 0; i < N; i++) {
        philosophers.emplace_back(philosopher, i);
    }

    // Dołączamy wątki (nigdy się nie kończą)
    for (auto& p : philosophers) {
        p.join();
    }

    return 0;
}
