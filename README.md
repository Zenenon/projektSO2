**Wiktor Idzik - 272938**
**Sprawozdanie – Problem jedzących filozofów**

**Opis problemu**
Problem jedzących filozofów to klasyczny problem synchronizacji, w którym grupa filozofów siedzi wokół stołu, między którymi leżą widelce.
Każdy filozof cyklicznie myśli, jest głodny i spożywa posiłek. Aby zjeść, filozof musi podnieść dwa widelce – jeden po swojej lewej i jeden po prawej stronie.
Problem polega na zapewnieniu, że filozofowie mogą jeść bez ryzyka zakleszczenia (deadlock) lub zagłodzenia (starvation).

**Wątki i ich rola**
W programie każdy filozof jest reprezentowany przez osobny wątek:

- **Wątek filozofa** – odpowiada za cykliczne przechodzenie przez trzy stany:
  - **THINKING (myślenie)** – filozof myśli przez losowy czas.
  - **HUNGRY (głodny)** – filozof próbuje podnieść dwa widelce.
  - **EATING (jedzenie)** – filozof spożywa posiłek przez losowy czas, po czym odkłada widelce.

Każdy z wątków na bieżąco raportuje swój stan w konsoli.

**Sekcje krytyczne i ich obsługa**
W programie występują trzy główne sekcje krytyczne:
    Podnoszenie widełków – filozof może podnieść widelec tylko wtedy, gdy nie jest on używany przez innego filozofa (mutex).
    Odkładanie widełków – po zakończeniu jedzenia oba mutexy są zwalniane.
    Wypisywanie do konsoli – wszystkie wypisy są chronione dodatkowym mutexem, aby uniknąć przeplatania tekstu z różnych wątków.
    
**Rozwiązanie synchronizacji**
Program nie korzysta z gotowych bibliotek synchronizacyjnych (std::mutex lub pthread_mutex_t). Zamiast tego:
    Zaimplementowano własną klasę Mutex, działającą jak SpinLock – aktywnie oczekuje na dostęp do zasobu przy użyciu atomowych instrukcji (__sync_lock_test_and_set, __sync_lock_release).
    Każdy widelec to osobna instancja Mutex, przechowywana w tablicy forks.
Aby uniknąć zakleszczenia, filozofowie podnoszą widelce w innej kolejności w zależności od swojego numeru:
    Filozofowie o parzystym numerze podnoszą najpierw swój widelec, potem sąsiedni.
    Filozofowie o nieparzystym numerze – w odwrotnej kolejności.
Dzięki temu żaden cykl oczekiwań nie może się zamknąć — co eliminuje możliwość zakleszczenia.

**Raportowanie stanu**
Wypisywanie stanu każdego filozofa odbywa się za pomocą funkcji log_state, która używa specjalnego print_mutex, aby tylko jeden wątek na raz miał dostęp do konsoli. Dzięki temu informacje nie nakładają się na siebie i są czytelne.
