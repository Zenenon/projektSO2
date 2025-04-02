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

**Sekcje krytyczne i ich rozwiązanie**
Sekcje krytyczne w programie występują w miejscach, gdzie kilka wątków może jednocześnie próbować uzyskać dostęp do wspólnych zasobów (widelców).

- **Sekcja krytyczna 1:** Podnoszenie widelców – tylko jeden filozof może jednocześnie podnieść dany widelec.  
- **Sekcja krytyczna 2:** Odkładanie widelców – konieczne jest sprawdzenie, czy sąsiedni filozofowie mogą rozpocząć jedzenie po zwolnieniu widelców.  
- **Sekcja krytyczna 3:** Raportowanie stanu filozofów – dostęp do konsoli jest również chroniony, aby uniknąć zakłóceń w wyświetlanych komunikatach.

**Rozwiązanie:**  
- Zastosowano **SpinLock**, który został zaimplementowany ręcznie.
- SpinLock blokuje zasób poprzez aktywne oczekiwanie na jego dostępność, co spełnia wymaganie projektu dotyczące ręcznej implementacji synchronizacji.  
- Aby uniknąć zakleszczenia, zastosowano **hierarchię zasobów**, która wymusza, aby filozofowie zawsze podnosili widelce w określonej kolejności – od widelca o niższym numerze do wyższego.
- Ostatni filozof zmienia kolejność, by zachować spójność i uniknąć kolizji.

**Podsumowanie**
Program efektywnie rozwiązuje problem ucztujących filozofów, spełniając kluczowe wymagania:
- Implementacja wątków bez użycia gotowych modułów synchronizacji.
- Prawidłowa obsługa sekcji krytycznych przy użyciu SpinLock.
- Brak zakleszczeń dzięki hierarchii zasobów.
- Filozofowie raportują swoje stany, co ułatwia śledzenie przebiegu symulacji.
