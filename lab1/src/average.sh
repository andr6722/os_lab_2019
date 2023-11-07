#!/bin/bash


sum=0
count=0


for arg in "$@"; do
    sum=$((sum + arg))
    count=$((count + 1))
done

echo "Количество чисел: $count"
echo "Среднее арифметическое: $((sum / count))"
