#!/bin/bash

# Sprawdzenie, czy podano 2 parametry.
if [ $# != 2 ];
then
  echo -n "Program potrzebuje dwóch parametrów: "
  echo "programu do przetestowania, ścieżki do folderu z testami."
  exit 1;
fi

# Ścieżka do testowanego programu.
BIN=$1
# Ścieżka do folderu z testami.
DIR=$2
# Liczba poprawnych testów.
ACCEPTED=0
# Liczba wszystkich testów.
TESTS=0



for f in $(find $DIR -name '*.in'); do
  TESTS=$((TESTS + 1))
  echo "Test ${f#$DIR/}"
  # Tworzy plik tymczasowy związany z testem.
  TMP_FILE="$(mktemp)"
  valgrind --error-exitcode=15 --leak-check=full \
  --show-leak-kinds=all --errors-for-leak-kinds=all -q $BIN <$f &>/dev/null \
  1> $TMP_FILE.out 2> $TMP_FILE.err
  EXIT_CODE=$?
  # Porównanie wyników programu z *.out i *.err poprzez diff.
  diff -q ${f%.in}.out $TMP_FILE.out &>/dev/null
  OUT=$?
  diff -q ${f%.in}.err $TMP_FILE.err &>/dev/null
  ERR=$?
  # Wypisanie komunikatów (wyników testu).
  if [ $EXIT_CODE -eq 0 ] && [ $ERR -eq 0 ] && [ $OUT -eq 0 ]
  then
    echo " -> test zaliczony (OK)"
    ACCEPTED=$((ACCEPTED + 1))
  else
    echo " -> test niezaliczony (WRONG)"
  fi
  if [ $EXIT_CODE -eq 0 ]
  then
    echo " -> kod wyjścia: 0"
  elif [ $EXIT_CODE -eq 15 ]
  then
    echo " -> błąd zarządzania pamięcią (kod wyjścia: 15)"
  else
    echo " -> niezidentyfikowany błąd (kod wyjścia: $EXIT_CODE)"
  fi
  if [ $OUT -ne 0 ]
  then
    echo " -> błędna odpowiedź"
  fi
  if [ $ERR -ne 0 ]
  then
    echo " -> błędne wyjście diagnostyczne"
  fi
  echo
  # Usunięcie plików tymczasowych testu.
  rm $TMP_FILE*
done

if [ $ACCEPTED -eq $TESTS ]
then
  exit 0;
else
  exit 1;
fi
