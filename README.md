# TAI_Lab2 - GUIDE

## How to run the isChatted program

Before running the program, the user must have the following requirements.

1. Create/Be on a virutal environment;

2. Installed the requirements inside `requirements.txt`

    ```bash
    pip install requirements.txt
    ```

3. Have the models inside the `./models/` folder. You can download the models from https://uapt33090-my.sharepoint.com/:u:/g/personal/tiagogcarvalho_ua_pt/EbwiCwn5H0BCmRkFVvFKRnkBBNkepXtmktnRC0SlspvHbg?e=P9HSap


Upon concluding the installation phase, you may run the program following these steps:

1. Compile the file

    ```bash
    g++ -o bin/chatted main.cpp -std=c++17
    ```

2. Execute file with arguments, *assumining you have the chry.txt file inside example folder*:

    ```bash
    ./bin/chatted -g ./models/gpt-4/gpt_files/ -h ./models/gpt-4/human_files/ -t ./input_files/input_file.txt -f nsec -k 10 -a 0.5 -s 100 -o ./results_file.txt -b Human
    ```

    where `-g ./models/gpt-4/gpt_files/ -h ./models/gpt-4/human_files/ -t ./input_files/input_file.txt -f nsec -k 10 -a 0.5 -s 100 -o ./results_file.txt -b Human` are the arguments that need to be passed.

    Alternatively, you can ommit the `g`, which, by default, is equal to `./models/gpt-4/gpt_files/`; `h`, by default `./models/gpt-4/human_files/`; `t`, by default `./test_files/test_gpt_files/`; `k`, by default `3`; `a`, by default `0.5`; `s`, by default `5000`.

## Additional experiment

We also created a interface that helps the user on running the program.

To run the interface the user must conclude the installation phase detailed on the top and run the following comand:

```bash
python app.py
```
