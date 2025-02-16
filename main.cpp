#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
using namespace std;
class StopWordRemover {
private:
    unordered_set<string> stopWords;
    bool caseSensitive;
    vector<string> processedTokens;

    // Helper functions

    // Convert string to lowercase
    string toLower(const string& str) const {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

    // Check if a word is a stop word
    bool isStopWord(const string& word) const {
        if (caseSensitive) {
            return stopWords.find(word) != stopWords.end();
        } else {
            return stopWords.find(toLower(word)) != stopWords.end();
        }
    }

    // Tokenize text into words
    vector<string> tokenize(const string& text) const {
        vector<string> tokens;
        stringstream ss(text);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }
    bool check_token_valid(const string& token, string& entry, int pos) {
        if (pos < 0 || pos + token.size() > entry.size()) {
            return false;
        }

        // Check left boundary
        bool leftValid = (pos == 0 || isspace(entry[pos - 1]));

        // Check right boundary
        bool rightValid = (pos + token.size() == entry.size() || isspace(entry[pos + token.size()]));

        return leftValid && rightValid;
    }
public:
    StopWordRemover(bool caseSensitive = false) : caseSensitive(caseSensitive) {
        initializeDefaultStopWords();
    }

    // Initialize default stop words
    void initializeDefaultStopWords() {
        stopWords = {
            "the", "is", "at", "which", "on", "in", "a", "an", "and", "info",
            "error", "warning", "debug", "trace", "fatal", "timestamp", "date",
            "time", "level", "message"
        };
    }

    // Load custom stop words from a file
    bool loadCustomStopWords(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        string word;
        while (file >> word) {
            if (!caseSensitive) {
                word = toLower(word);
            }
            stopWords.insert(word);
        }
        return true;
    }

    // Remove special characters
    string removeSpecialCharacters(const string& text) {
        string finalText;
        bool spaceFlag = false;

        for (char ch : text) {
            if (isalnum(ch)) {
                finalText += ch;
                spaceFlag = false;
            } else if (isspace(ch)) {
                if (!spaceFlag) {
                    finalText += ' ';
                    spaceFlag = true;
                }
            }else {
                finalText += " ";
            }
        }
        return finalText;
    }
    // Process text and append tokens to processedTokens
    vector<string> processText(const string& text) {
        string cleanedText = removeSpecialCharacters(text);
        vector<string> tokens = tokenize(cleanedText);
        vector<string> filteredTokens;
        for (const string& token : tokens) {
            if (!isStopWord(token)) {
                filteredTokens.push_back(token);
                processedTokens.push_back(token);
            }
        }
        return filteredTokens;
    }

    // Clear processedTokens
    void clearProcessedTokens() {
        processedTokens.clear();
    }

    // Detect dynamic stop words based on frequency threshold
    void detectDynamicStopWords(double frequencyThreshold) {
        if (processedTokens.empty()) {
            return;
        }

        unordered_map<string, int> wordFrequency;
        for (const string& token : processedTokens) {
            string key = caseSensitive ? token : toLower(token);
            wordFrequency[key]++;
        }

        for (const auto& [word, count] : wordFrequency) {
            double frequency = static_cast<double>(count) / processedTokens.size();
            if (frequency >= frequencyThreshold) {
                stopWords.insert(word);
            }
        }
    }

    // Process a single log entry
    string processLogEntry(const string& logEntry) {
        string cleaned = removeSpecialCharacters(logEntry);
        vector<string> tokens = tokenize(cleaned);
        string result = logEntry;
        for (int i = 0; i < result.size(); i++)
            if (!isalnum(result[i]) && !isspace(result[i]))
                result[i]= ' ';
        int pos = 0;
        for (const string& token : tokens) {
            pos = result.find(token, pos);
            if (isStopWord(token) && check_token_valid(token, result, pos))
                result.replace(pos, token.length(), string(token.length(), ' '));
            pos += token.length();
        }
        return result;
    }
};

// Example usage
int main() {
    string file_path = "file.txt";
    StopWordRemover remover(false);
    remover.initializeDefaultStopWords();
    // if (!remover.loadCustomStopWords(file_path))
    //     cout << "Failed to load stop words from file " << file_path << endl;
    string logEntry = "My name is Sameh, I'am true king.";
    cout<< "original stop words:  "<< logEntry << endl;
    cout << "processed stop words: ";
    cout<<remover.processLogEntry(logEntry) << endl;
    return 0;
}