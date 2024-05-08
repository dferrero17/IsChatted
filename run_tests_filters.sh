#!/bin/bash

echo "Compiling the cpm.cpp file."
g++ -o bin/chatted main.cpp -std=c++17

analize_dir="test_files_compressed"

for model in gpt-3.5 gpt-4 merge ; do
    
    humanDir="models/${model}/human_files/"
    chatDir="models/${model}/gpt_files/"

    echo "Model: ${model}"
    output_file="output_filter.txt"
    for alpha in ns nc nsc nsce;do
        echo "In filter=${filter}"
        ./bin/chatted -f $filter -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
        ./bin/chatted -f $filter -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
    done
    echo "In filter = no filter"
    ./bin/chatted -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
    ./bin/chatted -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human

done
