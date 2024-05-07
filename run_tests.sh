#!/bin/bash

echo "Compiling the cpm.cpp file."
g++ -o bin/chatted main.cpp -std=c++17


analize_dir="test"


for model in gpt-3.5 gpt-4 merge ; do
    
    humanDir="models/${model}/human_files/"
    chatDir="models/${model}/gpt_files/"

    echo "Output 1 for the ${model} model"
    output_file="output1.txt"
    for k in 2 3 4 5 6 7 8; do
        ./bin/chatted -k $k -f "nsc" -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
        ./bin/chatted -k $k -f "nsc" -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
    done

    echo "Output 2 for the ${model} model"
    output_file="output2.txt"
    for a in 0.1 0.3 0.5 0.7; do
        for filter in ns nc nsc nsce;do
            ./bin/chatted -a $a -f $filter -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
            ./bin/chatted -a $a -f $filter -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
        done
        ./bin/chatted -a $a -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
        ./bin/chatted -a $a -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
    done

    echo "Output 3 for the ${model} model"
    output_file="output3.txt"
    for a in 0.1 0.3 0.5 0.7; do
        for sizeLimit in 200 1000 5000 8000 ;do
            ./bin/chatted -a $a -s $sizeLimit -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
            ./bin/chatted -a $a -s $sizeLimit -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
        done
        ./bin/chatted -a $a -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_gpt_files/ -b gpt
        ./bin/chatted -a $a -o $output_file -g $chatDir -h $humanDir -t ./$analize_dir/test_human_files/ -b human
    done

done
