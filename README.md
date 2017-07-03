# Wizualizacja-sygnalow
C++ usage GUI in Win Api (External text data from sensors).  Visualization of automation signals.

Projekt napisano w okresie: 2017_05

Zadanie programistyczne z wizualizacją sygnałów automatyki.
(C++, z wykorzystaniem GUI w WinAPI, zewnętrzne dane tekstowe z czujników)

Zaimplementować program zawierający GUI w środowisku WinAPI, który wczytuje, przetwarza i wizualizuje dane aktualnego położenia kątowego (roll, pitch, yaw) robota mobilnego.
Program (w GUI) ma rysować prosty kompas i wyświetlać aktualny kierunek robota, zakładając, że na początku ruchu robot był skierowany na północ. 
W GUI nalezy dodać przyciski odpowiedzialne za wyświetlenie tych sygnałów na wykresie.

Wymagania dotyczące implementacji (funkcjonalności GUI) oraz wczytywania danych:
-przyciski odpowiedzialne za zmianę podziałki czasowej (przy rysowaniu wykresu oraz animacji),
-przyciski odpowiedzialne za zmianę skali amplitudy (przy rysowaniu wykresu),
-suwak lub 2 przyciski służące do zmiany okna uśredniania sygnału (jeżeli jest to wymagane),
-element wczytujący odpowiednie kolumny z pliku tekstowego,
-odrzucenie pierwszysch n próbek z danymi.

Opis pliku z danymi tekstowymi (pliki z danymi są dostępne w archiwum data.zip):
Liczby zawarte w pliku są zapisem danych wyjściowych z sensorów. Każdy sensor podaje dane z częstotliwością 25Hz 
(25 linii w pliku zawiera dane wyjściowe opisujące 1 sekundę ruchu sensora). Dane są grupowane w trójki (obejmują wskazania dotyczące każdej z 3 osi). W kolejnych kolumnach zapisane są od lewej odpowiednio:
- położenie kątowe (roll, pitch, yaw) w stopniach,
- przyspieszenie (a_x, a_y, a_z) w G (1G = 9,81m/s^2),
- wskazania magnetometru (m_x, m_y, m_z) - natężenie pola,
- wskazania żyroskopu (omega_x, omega_y, omega_z) w stopniach na sekundę.

Opis:
roll - obrót prawoskrętny względem osi X, pitch - względem osi Y, yaw - względem osi Z.
a_x - przyspieszenie sensora wzdłuż osi X sensora,
m_x - natężenie pola magnetycznego na osi X sensora,
omega_x - prędkość obrotowa względem osi X (obrót prawoskrętny względem osi X)

Poczatkowe próbki położenia kątowego są błędne. Algorytm wyznaczający położenie kątowe potrzebuje czasu, aby znaleźć położenie początkowe.
Należy umożliwić, aby rysowanie danych na wykresie zaczynało się od próbki wybranej przez użytkownika (odrzucenie pierwszych n próbek).
