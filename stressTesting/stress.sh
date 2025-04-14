#!/bin/bash

loop=${1:-1000}
comp=${2:-1}

if [ "$comp" -eq 1 ]; then
  echo "Compiling..."
  g++ gen.cpp -o gen
  g++ solution.cpp -o solution 
  g++ brute.cpp -o brute 
fi

for ((i=1; i<=loop; i++)); do
  echo "Test $i"
  ./gen > in.txt
  ./solution < in.txt > out1.txt
  ./brute < in.txt > out2.txt

  if ! diff -w out1.txt out2.txt > /dev/null; then
    echo "Mismatch found!"
    echo "=== Input ==="
    cat in.txt
    echo
    echo "=== Output ==="
    cat out1.txt
    echo
    echo "=== Expected ==="
    cat out2.txt
    exit 1
  fi
done

echo "All tests passed!"
rm -f in.txt out1.txt out2.txt


