#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Нет входных данных"
  exit 1
fi

file=$1

if [ ! -f "$file" ]; then
  echo "Файл не найден!"
  exit 1
fi

count=$(wc -l < "$file")

if [ $count -eq 0 ]; then
  echo "Файл пуст!"
  exit 1
fi

sum=$(paste -sd+ "$file" | bc)
average=$(echo "scale=2; $sum / $count" | bc)

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"
