#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <regex>
#include <unordered_map>
#include <vector>
using namespace std;

multimap<int, string> ConfigureDataMap(unordered_map<string, int> mymap)
{
    multimap<int, string> tmap;

    for (auto it = mymap.begin(); it != mymap.end(); it++)
    {
        if (it->second > 1)
            tmap.insert(make_pair(it->second, it->first));
    }

    return tmap;
}

int main()
{
    regex url("(?:(?:https?|ftp):\\/\\/)?[\\w/\\-?=%.]+\\.[\\w/\\-&?=%.]+");
    stringstream buffer;
    vector<string> urls;
    unordered_map<string, int> word_data;
    string word;
    string punctuation_marks = "[](),.:;!?\"";
    int sentences_amount = 0;

    ifstream fi("data.txt");
    buffer << fi.rdbuf();
    fi.close();

    while (!buffer.eof())
    {
        buffer >> word;

        if (regex_match(word, url))
            urls.push_back(word);

        char word_end = word[word.length() - 1];
        if (word_end == '.' || word_end == '?' || word_end == '!')
            sentences_amount++;

        transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (any_of(begin(word), end(word), ::isalpha))
        {
            for (char c : punctuation_marks)
                word.erase(remove(word.begin(), word.end(), c), word.end());
            if (word_data.find(word) != word_data.end())
                word_data[word]++;
            else
                word_data[word] = 1;
        }
    }
    buffer.clear();

    // Pasikartojanciu zodziu isvedimas
    ofstream fo_output("output.txt");
    multimap<int, string> word_data_configured = ConfigureDataMap(word_data);
    for (auto it = word_data_configured.rbegin(); it != word_data_configured.rend(); it++)
        fo_output << it->second << " - " << it->first << endl;
    fo_output.close();

    // URL adresu isvedimas
    ofstream fo_urls("urls.txt");
    for (auto url : urls)
        fo_urls << url << endl;
    fo_urls.close();

    return 0;
}