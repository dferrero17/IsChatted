#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>
#include <vector>
#include <filesystem>

class Prompt {

    public:
        Prompt(
            std::string fileDir,
            std::unordered_set<char> filters,
            int k,
            int sizeLimit)
        :
        fileDir(fileDir),
        filters(filters),
        k(k),
        sizeLimit(sizeLimit) {};
    
    void populateTable(std::string file){

        lastKSymbols = "";

        std::ifstream inputFile(file);
        if (!inputFile) {
            std::cerr << "Failed to open file: " << file << std::endl;
            exit(1);
        }

        char symbol;
        while (inputFile.get(symbol)) {
            symbol = conversion(symbol);
            if (symbol == '\0'){
                continue;
            }
            alphabet.insert(symbol);
        }
        inputFile.close();
        
        //printAlphabet();

        inputFile.open(file);
        while(inputFile.get(symbol)){
            symbol = conversion(symbol);
            if (symbol == '\0'){
                continue;
            }
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
        std::unordered_set<char> filters;
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

    char conversion(char symbol) {
        if (filters.find('n') != filters.end() && std::isdigit(symbol)) {
            return '\0';
        }
        if (filters.find('s') != filters.end() && std::ispunct(symbol)) {
            return '\0';
        }
        if (filters.find('e') != filters.end() && std::isspace(symbol)) {
            return '\0';
        }
        if (filters.find('c') != filters.end() && std::islower(symbol)) {
            return std::toupper(symbol);
        }
        if ((!std::isalpha(symbol) && !std::ispunct(symbol) && !std::isspace(symbol)) || (symbol == '\n')) {
            return '\0';
        }   
        return symbol; 
    }
 
};

    class Encode {

        public:

            Encode(
                std::string chatDir,
                std::string humanDir,
                std::string inputDir,
                std::string stringFilters,
                std::string outputFile,
                bool isGPT,
                std::unordered_set<char> filters,
                int k,
                int sizeLimit,
                double alpha)
            : 
            chatDir(chatDir),
            humanDir(humanDir),
            inputDir(inputDir),
            stringFilters(stringFilters),
            outputFile(outputFile),
            isGPT(isGPT),
            filters(filters),
            k(k),
            alpha(alpha),
            sizeLimit(sizeLimit),
            gptPrompt(chatDir, filters, k, sizeLimit),
            humanPrompt(humanDir, filters, k, sizeLimit) {} ;


        void start(){
            
            if (chatDir.back() == '/'){
                for (const auto & entry : std::filesystem::directory_iterator(chatDir))
                    gptPrompt.populateTable(entry.path().string());
            }
            else {
                try
                {
                    gptPrompt.populateTable(chatDir);
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Not a directory nor a file. Directories should end with the '/' char." << std::endl;
                    std::cerr << e.what() << '\n';
                }
                
            }

            if (humanDir.back() == '/'){
                for (const auto & entry : std::filesystem::directory_iterator(humanDir))
                    humanPrompt.populateTable(entry.path().string());
            }
            else {
                try
                {
                    humanPrompt.populateTable(humanDir);
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Not a directory nor a file. Directories should end with the '/' char." << std::endl;
                    std::cerr << e.what() << '\n';
                }
                
            }

            //gptPrompt.printAlphabet();
            //humanPrompt.printAlphabet();
            if (inputDir.back() == '/'){
                for (const auto & entry : std::filesystem::directory_iterator(inputDir))
                    compareToReferences(entry.path().string());
            }
            else {
                try
                {
                    compareToReferences(inputDir);
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Not a directory nor a file. Directories should end with the '/' char." << std::endl;
                    std::cerr << e.what() << '\n';
                }
                
            };
                
        };

    private:
        std::string chatDir, humanDir, inputDir, stringFilters, outputFile;
        std::unordered_set<char> filters;
        int k, sizeLimit;
        double alpha;
        bool isGPT;

        double GPTBits = 0.0, humanBits = 0.0;
        int totalSymbols = 0; 
        std::string context = "";        //similiar to lastKSymbols
        Prompt gptPrompt;
        Prompt humanPrompt;

        void initialize(){
            context = "";
            totalSymbols = 0, GPTBits = 0.0, humanBits = 0.0;
        }

        void compareToReferences(std::string fileName){

            auto start = std::chrono::high_resolution_clock::now();

            initialize();
            
            //calculate encoding for both the tables
            std::ifstream file(fileName);
            if (!file) {
                std::cerr << "Failed to open file: " << fileName << std::endl;
                exit(1);
            }

            char symbol;
            while(file.get(symbol)){
                symbol = conversion(symbol);
                if (symbol == '\0'){
                    continue;
                }
                if (context.size() == k){
                    GPTBits += processSymbol(symbol, gptPrompt.getTable(), gptPrompt.getAlphabet());
                    humanBits += processSymbol(symbol, humanPrompt.getTable(), humanPrompt.getAlphabet());
                }
                //std::cout << "symbol: " << symbol << std::endl;
                updateContext(symbol);
                totalSymbols++;   
                
            }

            file.close();

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            
            bool inferedGPT = false;
            if (GPTBits < humanBits){
                inferedGPT = true;
            }

            if (outputFile != ""){
                writeToFile(inferedGPT, elapsed.count());
            }
            else {
                std::cout << "GPT bits: " << GPTBits << std::endl;
                std::cout << "Human bits: " << humanBits << std::endl;
                std::cout << "Total symbols: " << totalSymbols << std::endl;
            }
        }

        void writeToFile(bool inferedGPT, double runtime){

            std::stringstream ss;
            ss << k << "," << alpha << "," << sizeLimit << "," << stringFilters << "," //colocar filters
                << isGPT << "," << inferedGPT << "," << GPTBits/humanBits << ","
                << totalSymbols << ","
                << runtime << std::endl;
            
            // File writing
            std::ofstream outFile("./results/" + outputFile, std::ios::app);
            outFile << ss.str();
            outFile.close();
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

        char conversion(char symbol){

            if (filters.find('n') != filters.end() && std::isdigit(symbol)) {
                return '\0';
            }
            if (filters.find('s') != filters.end() && std::ispunct(symbol)) {
                return '\0';
            }
            if (filters.find('e') != filters.end() && std::isspace(symbol)) {
                return '\0';
            }
            if (filters.find('c') != filters.end() && std::islower(symbol)) {
                return std::toupper(symbol);
            }
            if ((!std::isalpha(symbol) && !std::ispunct(symbol) && !std::isspace(symbol)) || (symbol == '\n')) {
                return '\0';
            }  
            return symbol; 
        }
 
};


int main(int argc, char* argv[]) {

    std::string chatDir = "./models/gpt-3.5/gpt_files/"; 
    std::string humanDir = "./models/gpt-3.5/human_files/";
    std::string inputDir = "./test_files/test_gpt_files/";

    std::string outputFile = "", gptArg = "";
    std::string filters="";

    bool isGPT = false;

    int k = 3, sizeLimit = 5000;
    double alpha = 0.5;

    int opt;
    while ((opt = getopt(argc, argv, "g:h:t:f:k:a:s:o:b:")) != -1) {
        switch (opt) {
            case 'g':
                chatDir = optarg;
                break;
            case 'h':
                humanDir = optarg;
                break;
            case 't':
                inputDir = optarg;
                break;
            case 'f':
                filters = optarg;
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
            case 'o':
                outputFile = optarg;
                break;
            case 'b':
                gptArg = optarg;
                break;
            case '?':
                std::cerr << "Wrong arguments were used.\n" << std::endl;
                std::cerr << "Usage: " << argv[0] << "-k <k_value> -a <alpha> -s <size?limit> -f <filters> -o <result_file> -g <gpt_folder_directory> -h <human_folder_directory> -t <texts_to_analyze_directory>" << std::endl;
                std::cerr << "Available filters: " << std::endl;
                std::cerr << "n - remove numbers" << std::endl;
                std::cerr << "s - remove symbols" << std::endl;
                std::cerr << "e - remove spaces" << std::endl;
                std::cerr << "c - ignore case sensitiveness" << std::endl;
                std::cerr << "Several filters can be used at once." << std::endl;
                return 1;
        }
    }

    if (inputDir.empty() || k == 0 || alpha == 0.0 || humanDir.empty() || chatDir.empty()) {
        std::cerr << "Some options are required and should not be null.\n" << std::endl;
        std::cerr << "Usage: " << argv[0] << "-k <k_value> -a <alpha> -s <size?limit> -f <filters> -o <result_file> -g <gpt_folder_directory> -h <human_folder_directory> -t <texts_to_analyze_directory>" << std::endl;
        std::cerr << "<k_value> and <alpha> should not be null. Directories cannot be null either." << std::endl;
        return 1;
    }

    std::transform(gptArg.begin(), gptArg.end(), gptArg.begin(),
    [](unsigned char c){ return std::tolower(c); });
    
    if (gptArg != "gpt" && gptArg != "human"){
        std::cerr << "Valid arguments for -b flag are <gpt> or <human>." << std::endl;
        return 1;
    }

    if (gptArg == "gpt"){
        isGPT = true;
    }

    std::unordered_set<char> acceptedFilters = {'n', 's', 'e', 'c', ' '};  //numbers, symbols, space, case insensitiveness
    std::unordered_set<char> acceptedFilters_ = {};

    for (char c : filters) {
        if ((acceptedFilters.find(std::tolower(c)) == acceptedFilters.end()) && filters!="") {
            std::cerr << "Filter not found." << std::endl;
            std::cerr << "Available filters: " << std::endl;
            std::cerr << "n - remove numbers" << std::endl;
            std::cerr << "s - remove symbols" << std::endl;
            std::cerr << "e - remove spaces" << std::endl;
            std::cerr << "c - ignore case sensitiveness" << std::endl;
            std::cerr << "Several filters can be used at once." << std::endl;
            return 1;
        }

        acceptedFilters_.insert(std::tolower(c));
    }

    Encode main(chatDir, humanDir, inputDir, filters, outputFile, isGPT, acceptedFilters_, k, sizeLimit, alpha);
    main.start();

    return 0;
}