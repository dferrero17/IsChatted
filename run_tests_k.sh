#!/bin/bash

echo "Compiling the cpm.cpp file."
g++ -o bin/chatted main.cpp -std=c++17

analize_dir="test_files_compressed"

humanDir="models/gpt-3.5/human_files/"
chatDir="models/gpt-3.5/gpt_files/"

echo "Model: gpt-3.5"
output_file="output_k.txt"
for k in 2 3 4 5 6 7; do
    echo "In k=${k}"
    ./bin/chatted -k $k -a 0.1 -f nsc -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
    ./bin/chatted -k $k -a 0.1 -f nsc -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
done
