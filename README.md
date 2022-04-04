# language_interpreter

statyczne/słabe/stałe/wymagane

Język w założeniu posiada pętlę while, konstrukcję if else, switch i funkcje. Interpreter pisany w C++ otrzyma std::istream, z którego będzie odczytywał nowy kod i std::ostream na który wypisze wyniki.

Obsługa błędów polegać będzie na przerwaniu wykonywania kodu i wypisaniu na ostream komunikatu z linią, opisem błędu i kontekstem. Wszystkie obecnie wykonywane bloki zostaną zamknięte bez zapisywania (np. jeśli błąd wystąpił w ciele funkcji, funkcja zostanie uznana za nieistniejącą). Język nie przewiduje łapania błędów ani wyjątków.

Dokładna struktura realizacji jest na chwilę obecną nieokreślona i ukształtuje się z czasem.

Testy będą polegały na podawaniu interpreterowi fragmentów kodu i porównywaniu wyników z oczekiwanymi. Przetestowany zostanie w ten sposób każdy przewidziany błąd i każda zaimplementowana funkcjonalność. Ponadto, język zostanie wykorzystany zgodnie z przeznaczeniem: napisanych będzie kilka programów i jakakolwiek wykryta niezgodność z założeniami trafi do puli testów.

typy podstawowe (nie ma klas, więc jedyne):

* bool
* char (8b)
* uchar
* int (32b)
* uint
* float (32b)
* str

Planowane jest umożliwienie konwersji z każdego typu na każdy.

kod obrazujący użycie:

```
int GCD(@int a, int b){
    @int buf=b, buf2; // b jest stałe, podane przez wartość (w razie zdjęcia niemutowalności)

    print(buf2); // 0

    while(buf){
        buf2=buf;
        buf=a%buf2;
        a=buf2; // a jest mutowalne
    }
    return a;
}

int a=16, b=9;
{
    print(GCD(a, b)); // 8
    b = GCD(a, b); // błąd, przypisanie do stałej
    @int b = GCD(a, b); //nowa zmienna przykrywa starą
} //                      ale nie poza blokiem
print(b); // 9

{a=8;} // to nie jest nowa zmienna
print(a); // 8

switch(a){
    0 -> {print("asd");}
    1 -> print("dsa");
    2 -> print("asd"); print("dsa"); // błąd, powinien być blok lub jedna instrukcja
    "16" -> print("this will be printed");
    else -> print("123");
}

@str line; // ""
if(a==str=getline()){
  print("correct!");
}else if(str=="super_secret$tring")
  print("how did you know?");
else{
  print("nope");
}

int GCD(){return 5;}
int GCD(){return 8;}

print(GCD()); // 8
print(GCD(5, 8)); // 4

@str asd="bsd";
asd[0]--; // "asd"
```
