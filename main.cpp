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
#include <filesystem>

class Prompt {

    public:
        Prompt(
            std::string fileDir,
            int k,
            int sizeLimit)
        :
        fileDir(fileDir),
        k(k),
        sizeLimit(sizeLimit) {};
    
    void populateTable(std::string file){

        lastKSymbols = "";

        std::ifstream inputFile(file, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Failed to open file: " << file << std::endl;
            exit(1);
        }

        char symbol;
        while (inputFile.get(symbol)) {
            alphabet.insert(symbol);;
        }
        inputFile.close();
        
        //printAlphabet();

        inputFile.open(file);
        while(inputFile.get(symbol)){
            processSymbol(symbol);
        }
        inputFile.close();
    
    }

    void printAlphabet() const {
        std::cout << "Alphabet: ";
        for (char c : alphabet) {
            std::cout << c << "";
        }
        std::cout << std::endl;
    }

    void printPromptTable() const {
        std::cout << "Prompt Table:" << std::endl;
        for (const auto& outer_pair : promptTable) {
            std::cout << "Key: " << outer_pair.first << std::endl;
            std::cout << "Values: " << std::endl;
            for (const auto& inner_pair : outer_pair.second) {
                std::cout << "  " << inner_pair.first << ": " << inner_pair.second << "; ";
            }
            std::cout << std::endl;
        }
    }

    std::unordered_map<std::string, std::unordered_map<char, double>> getTable(){
        return promptTable;
    };
   
    std::unordered_set<char> getAlphabet(){
        return alphabet;
    }


    private:
        std::string fileDir;
        int k, sizeLimit;

        std::string lastKSymbols = "";
        std::unordered_set<char> alphabet;
        std::unordered_map<std::string, std::unordered_map<char, double>> promptTable;

    void processSymbol(char currentSymbol){

        if (lastKSymbols.size() == k) { 
            updateCountersTable(currentSymbol);
        }
        updateLastKSymbols(currentSymbol);
    }

    void updateCountersTable(char symbol){

        auto it = promptTable.find(lastKSymbols);
        if (it == promptTable.end()) {                      // key not in table
            
            std::unordered_map<char, double> charCounter;
            for (char c : alphabet) {
                charCounter[c] = 0;
            }
            charCounter[symbol] = 1;
            promptTable.insert({lastKSymbols, charCounter});

        } else {                                            // key in table
            std::unordered_map<char, double>& charCounter = it->second; 

            if (charCounter[symbol] == sizeLimit){
                resizeTable();
            }

            charCounter[symbol]++;
            promptTable[lastKSymbols] = charCounter;

        }
    }
    
    void updateLastKSymbols(char symbol) {
        if (lastKSymbols.size() == k) { 
            lastKSymbols.erase(0, 1); 
        }
        lastKSymbols += symbol;
    }

    void resizeTable() {
        // if size has reached its limit, halve the values of int
        for (auto& entry : promptTable) {
            std::unordered_map<char, double>& charCounter = entry.second;
            for (auto& pair : charCounter) {
                pair.second /= 2;
            }
        }
    }
 
};

    class Encode {

        public:

            Encode(
                std::string chatDir,
                std::string humanDir, 
                std::string inputFile,
                int k,
                int sizeLimit,
                double alpha)
            : 
            chatDir(chatDir),
            humanDir(humanDir),
            inputFile(inputFile),
            k(k),
            alpha(alpha),
            sizeLimit(sizeLimit),
            gptPrompt(chatDir, k, sizeLimit),
            humanPrompt(humanDir, k, sizeLimit) {} ;

        void openFile(std::string file){
            std::ifstream File(file, std::ios::binary);
            if (!File) {
                std::cerr << "Failed to open file: " << file << std::endl;
                exit(1);
            }
        }

        void start(){

            //just check first if it is possible to open all the files
            openFile(inputFile);
            
            // populate both tables
            for (const auto & entry : std::filesystem::directory_iterator(chatDir))
                gptPrompt.populateTable(entry.path().string());

            for (const auto & entry : std::filesystem::directory_iterator(humanDir))
                humanPrompt.populateTable(entry.path().string());

            compareToReferences();

        };

    private:
        std::string chatDir, humanDir, inputFile;
        int k, sizeLimit;
        double alpha;

        double GPTBits = 0.0, humanBits = 0.0;
        int totalSymbols = 0; 
        std::string context = "";        //similiar to lastKSymbols
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
            while(file.get(symbol)){
                if (context.size() == k){
                    GPTBits += processSymbol(symbol, gptPrompt.getTable(), gptPrompt.getAlphabet());
                    humanBits += processSymbol(symbol, humanPrompt.getTable(), humanPrompt.getAlphabet());
                }

                updateContext(symbol);
                totalSymbols++;   
            }

            std::cout << "GPT bits: " << GPTBits << std::endl;
            std::cout << "Human bits: " << humanBits << std::endl;
            std::cout << "Total symbols: " << totalSymbols << std::endl;

            file.close();
        }

        void updateContext(char symbol){

            if (context.size() == k) { 
                context.erase(0, 1); 
            }

            context += symbol;
        }

        double calculateProb(std::unordered_map<char, double>& alphabetCounters, char symbol, std::unordered_set<char> alphabet){

            double sumCounters = alpha*alphabet.size();
            for (const auto& pair : alphabetCounters) {
                sumCounters += pair.second;
            }

            double probability = (alphabetCounters[symbol] + alpha) / sumCounters;

            return probability;
        }

        double processSymbol(char symbol, std::unordered_map<std::string, std::unordered_map<char, double>> table, std::unordered_set<char> alphabet){
            //try to find the context in the table
            auto it = table.find(context);
            if (it == table.end()) {                      // key not in table
                return -log2(1.0/alphabet.size());
            }
            else{
                std::unordered_map<char, double>& alphabetCounters = it->second; 
                return -log2(calculateProb(alphabetCounters, symbol, alphabet));
            }
            return 0;
        }

};


int main(int argc, char* argv[]) {

    std::string chatDir = "./gpt_files/"; 
    std::string humanDir = "./human_files/";

    std::string inputFile = "";

    int k = 0, sizeLimit = 5;
    double alpha = 0.0;

    int opt;
    while ((opt = getopt(argc, argv, "rc:rh:t:k:a:s:")) != -1) {
        switch (opt) {
            case 'r':
                chatDir = optarg;
                break;
            case 'c':
                humanDir = optarg;
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
                std::cerr << "Usage: " << argv[0] << " -t <inputFileAnalyze> -k <anchorSize> -a <alpha>" << std::endl;
                std::cerr << "Additional arguments: -rc <chatFilePath> -rh <humanFilePath> -s <sizeLimit>" << std::endl;
                return 1;
        }
    }

    if (inputFile.empty() || k == 0 || alpha == 0.0) {
        std::cerr << "All options are required and should not be null.\n" << std::endl;
        std::cerr << "Usage: " << argv[0] << " -t <inputFileAnalyze> -k <anchorSize> -a <alpha>" << std::endl;
        std::cerr << "Additional arguments: -rc <chatFilePath> -rh <humanFilePath> -s <sizeLimit>" << std::endl;
        return 1;
    }

    Encode main(chatDir, humanDir, inputFile, k, sizeLimit, alpha);
    main.start();

    return 0;
}