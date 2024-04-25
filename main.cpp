#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>
#include <vector>

class Encode {

    public:

        Encode(
            std::string chatFile,
            std::string humanFile, 
            std::string inputFile,
            int k,
            int sizeLimit,
            double alpha)
        : 
        chatFile(chatFile),
        humanFile(humanFile),
        inputFile(inputFile),
        k(k),
        alpha(alpha),
        gptPrompt(chatFile, k, sizeLimit),
        humanPrompt(humanFile, k, sizeLimit) {} ;

    void openFile(std::string file){
        std::ifstream File(file, std::ios::binary);
        if (!File) {
            std::cerr << "Failed to open file: " << file << std::endl;
            exit(1);
        }
    }

    void start(){

        //just check first if it is possible to open all the files
        openFile(chatFile);
        openFile(humanFile);
        openFile(inputFile);
        
        // populate both tables
        gptPrompt.populateTable();
        humanPrompt.populateTable();

        compareToReferences();

    };

    private:
        std::string chatFile, humanFile, inputFile;
        int k, sizeLimit;
        double alpha;

        int GPTBits, humanBits, totalSymbols = 0; 
        std::string context;        //similiar to lastKSymbols
        Prompt gptPrompt;
        Prompt humanPrompt;

        void compareToReferences(){
            //calculate encoding for both the tables
            std::ifstream file(inputFile, std::ios::binary);
            if (!file) {
                std::cerr << "Failed to open file: " << inputFile << std::endl;
                exit(1);
            }

            char symbol;
            file.open(inputFile);
            while(file.get(symbol)){

                if (context.size() == k){
                    GPTBits += processSymbol(symbol, gptPrompt.getTable(), gptPrompt.getAlphabet());
                    humanBits += processSymbol(symbol, humanPrompt.getTable(), humanPrompt.getAlphabet());
                }

                updateContext(symbol);
                totalSymbols++;   
            }

            file.close();
        }

        void updateContext(char symbol){

            if (context.size() == k) { 
                context.erase(0, 1); 
            }

            context += symbol;
        }


        double calculateProb(std::unordered_map<char, int>& alphabetCounters, char symbol, std::unordered_set<char> alphabet){

            double sumCounters = alpha*alphabet.size();
            for (const auto& pair : alphabetCounters) {
                sumCounters += pair.second;
            }

            double probability = (alphabetCounters[symbol] + alpha) / sumCounters;
            
            return probability;
        }

        int processSymbol(char symbol, std::unordered_map<std::string, std::unordered_map<char, int>> table, std::unordered_set<char> alphabet){
            //try to find the context in the table
            auto it = table.find(context);
            if (it == table.end()) {                      // key not in table
                return -log2(1/alphabet.size());          //just a normal char? TODO: ask professor
            }
            else{
                std::unordered_map<char, int>& alphabetCounters = it->second; 
                return -log2(calculateProb(alphabetCounters, symbol, alphabet));
            }
            return 0;
        }

};

class Prompt {

    public:
        Prompt(
            std::string file,
            int k,
            int sizeLimit)
        :
        file(file),
        k(k),
        sizeLimit(sizeLimit) {};
    
    void populateTable(){

        std::ifstream inputFile(file, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Failed to open file: " << file << std::endl;
            exit(1);
        }

        char symbol;
        while (inputFile.get(symbol)) {
            alphabet.insert(symbol);;
        }
        
        char symbol;
        inputFile.open(file);
        while(inputFile.get(symbol)){
            processSymbol(symbol);
        }
        inputFile.close();
    
    }

    std::unordered_map<std::string, std::unordered_map<char, int>> getTable(){
        return promptTable;
    };

    
    std::unordered_set<char> getAlphabet(){
        return alphabet;
    }


    private:
        std::string file;
        int k, sizeLimit;

        std::string lastKSymbols = "";
        std::unordered_set<char> alphabet;
        std::unordered_map<std::string, std::unordered_map<char, int>> promptTable;

    void processSymbol(char currentSymbol){
        
        if (lastKSymbols.size() == k) { 
            updateCountersTable(currentSymbol);
        }

        updateLastKSymbols(currentSymbol);

    }

    void updateCountersTable(char symbol){

        auto it = promptTable.find(lastKSymbols);
        if (it == promptTable.end()) {                      // key not in table
            
            std::unordered_map<char, int> charCounter;
            for (char c : alphabet) {
                charCounter[c] = 0;
            }
            charCounter[symbol] = 1;
            
            promptTable.insert({lastKSymbols, charCounter});

        } else {                                            // key in table
            std::unordered_map<char, int>& charCounter = it->second; 
            charCounter[symbol]++;

            if (charCounter[symbol] == sizeLimit){
                resizeTable(promptTable);
            }
            
        }
    }
    
    void updateLastKSymbols(char symbol) {
        if (lastKSymbols.size() == k) { 
            lastKSymbols.erase(0, 1); 
        }

        lastKSymbols += symbol;
    }

    void resizeTable(std::unordered_map<std::string, std::unordered_map<char, int>>& promptTable) {
        // if size has reached its limit, halve the values of int
        for (auto& entry : promptTable) {
            std::unordered_map<char, int>& charCounter = entry.second;
            for (auto& pair : charCounter) {
                pair.second /= 2;
            }
        }
    }
 
};


int main(int argc, char* argv[]) {

    std::string chatFile, 
                humanFile,
                inputFile = "";

    int k = 0, sizeLimit = 5000;
    double alpha = 0.0;

    int opt;
    while ((opt = getopt(argc, argv, "rc:rh:t:k:a:s:")) != -1) {
        switch (opt) {
            case 'rc':
                chatFile = optarg;
                break;
            case 'rh':
                humanFile = optarg;
                break;
            case 't':
                inputFile = optarg;
                break;
            case 'k':
                k = std::stoi(optarg);
                break;
            case 'a':
                alpha = std::stod(optarg);
                break;
            case 's':
                sizeLimit = std::stoi(optarg);
                break;
            case '?':
                std::cerr << "Wrong arguments were used.\n" << std::endl;
                std::cerr << "Usage: " << argv[0] << " -rc <chatFilePath> -rh <humanFilePath> -t <inputFileAnalyze> -k <anchorSize> -a <alpha>" << std::endl;
                std::cerr << "Additional arguments: -l <sizeLimit>" << std::endl;
                return 1;
        }
    }

    if (chatFile.empty() || humanFile.empty() || inputFile.empty() || k == 0 || alpha == 0.0) {
        std::cerr << "All options are required and should not be null.\n" << std::endl;
        std::cerr << "Usage: " << argv[0] << " -rc <chatFilePath> -rh <humanFilePath> -t <inputFileAnalyze> -k <anchorSize> -a <alpha>" << std::endl;
        std::cerr << "Additional arguments: -l <sizeLimit>" << std::endl;
        return 1;
    }

    Encode main(chatFile, humanFile, inputFile, k, alpha, sizeLimit);
    main.start();

    return 0;
}