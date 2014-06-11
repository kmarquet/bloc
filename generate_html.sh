#!/bin/sh
for file in `ls results/PERFS*.tex`
do
    latex2html $file
done

for file in `ls results/MEMORY*.tex`
do
    latex2html $file
done
