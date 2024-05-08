#!/bin/bash

echo "Compiling the cpm.cpp file."
g++ -o bin/chatted main.cpp -std=c++17

analize_dir="test_files_compressed"

for model in gpt-3.5 gpt-4 merge ; do

    humanDir="models/${model}/human_files/"
    chatDir="models/${model}/gpt_files/"

    echo "Model: ${model}"
    output_file="output_size_limit.txt"
    for sizeLimit in 200 1000 5000 8000 ;do
        echo "In sizeLimit=${sizeLimit}"
        ./bin/chatted -s $sizeLimit -f nsc -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
        ./bin/chatted -s $sizeLimit -f nsc -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
    done
done