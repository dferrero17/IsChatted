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

The argument rules follow the following table:

- **Size limit (s)**: The size limit, as mentioned above, determines the maximum allowed number count that a *hashmap* can have. When counting the symbols and incrementing them in the respecting table, when encountering a counter that is the same number as the size limit, then all the elements of that hash table become halved. This parameter is useful in situations where the reference texts are too extensive and surpass the program's native data number storing capacity..

- **Filters (f)**: The filters, as also mentioned above, filter both the texts to be analyzed and the reference texts to decrease some computational effort for symbols that are not relevant to provide a good description for a model. These filters include:

    - **n**: Filter all the characters that are numbers.

    - **s**: Filter all the characters that are symbols.

    - **e**: Filter all the characters that are spaces.

    - **c**: All letters are converted to their uppercase, and therefore, all texts become text insensitive.

    Several filters can be applied at once, for example, indicating the filter as "*ns*" means all numbers and symbols will be filtered, but spaces will not, and the letters maintain their case sensitiveness.

- **Smoothing Parameter (α)**: Aids in smoothing the probability distributions, ensuring that symbols not yet observed are still given a chance of occurrence, thus preventing zero-probability issues.

- **Context size (k)**: Dictates the size of the sequence of symbols considered as a context, influencing the model’s sensitivity to patterns within the data.

- **File/directory to be analyzed location (t)**: Location of the file(s) to be analyzed and classified based on the number of bits for each model. Can either be a directory or a file. If it corresponds to a directory, then all the files in that directory will be analyzed. 

- **File/directory as a ChatGPT reference location (g)**: Location of the file(s) to be used as a reference for the ChatGPT model. Can either be a directory or a file. If it corresponds to a directory, then all the files in that directory will be analyzed.

- **File/directory as a Human reference location (h)**: Location of the file(s) to be used as a reference for the human/non-AI model. Can either be a directory or a file. If it corresponds to a directory, then all the files in that directory will be analyzed.

- **True label of the file/directory to be analyzed (b)**: Restrictively GPT/Human, referees to the original input source, i.e. to the text to be analyzed.

- **Path to the results file (o)**: Specify the name of the file where the results should be displayed. If this parameter is not passed as an argument, the results will be displayed solely on the user's terminal. Each line in the file indicates respectively the $k$, $alpha$, size limit, filters used, the true label of the file/directory to be analyzed, if the resulting model is more similar to ChatGPT or not, the reason between the number of bits using the ChatGPT model and the number of bits using the human model and the time, in seconds, it took to analyze that file.

## Additional experiment

We also created a interface that helps the user on running the program.

To run the interface the user must conclude the installation phase detailed on the top and run the following comand:

```bash
python app.py
```
