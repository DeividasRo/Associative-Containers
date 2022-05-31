#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <regex>
#include <unordered_map>
#include <vector>
#include <iomanip>
using namespace std;

// sukonfiguruoja duomenis, paverciant unordered_map i multimap ir atrenkant tik zodzius pasikartojancius daugiau nei 1 karta
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
    vector<string> urls;
    unordered_map<string, int> word_data;
    unordered_map<string, vector<int>> word_positions;

    string word, line;
    string punctuation_marks = "[](),.:;!?\"";
    int line_count = 0;

    ifstream fi("data.txt");

    while (fi)
    {
        while (getline(fi, line))
        {
            istringstream iss(line);
            line_count++;
            while (iss >> word)
            {
                // tikrina ar zodis yra URL adresas
                if (regex_match(word, url))
                {
                    urls.push_back(word);
                    continue;
                }

                // pavercia zodzio raides mazosiomis
                transform(word.begin(), word.end(), word.begin(), ::tolower);

                if (any_of(begin(word), end(word), ::isalpha))
                {
                    // panaikina skyrybos zenklus zodyje
                    for (char c : punctuation_marks)
                        word.erase(remove(word.begin(), word.end(), c), word.end());

                    // tikrina ar zodis jau buvo irasytas i word_data
                    if (word_data.find(word) != word_data.end())
                        word_data[word]++;
                    else
                        word_data[word] = 1;

                    // tikrina ar tam tikra eilute jau yra uzfiksuota tikrinamam zodziui
                    if (find(word_positions[word].begin(), word_positions[word].end(), line_count) == word_positions[word].end())
                        word_positions[word].push_back(line_count);
                }
            }
        }
    }
    fi.close();

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

    // Cross-reference lenteles isvedimas (zodis X eilutes skaicius)
    ofstream fo_cr("crossref_table.txt");
    fo_cr << setw(22) << "|";
    for (int i = 1; i < line_count; i++)
    {
        if (i < 10)
            fo_cr << 0;
        fo_cr << i << "|";
    }
    string s(19 + line_count * 3, '-');
    fo_cr << endl;
    fo_cr << s << endl;
    for (auto it = word_data_configured.rbegin(); it != word_data_configured.rend(); it++)
    {
        fo_cr << setw(21) << right << it->second << "|";
        int j = 0;
        for (int i = 1; i < line_count; i++)
        {
            if (word_positions[it->second][j] == i)
            {
                fo_cr << "XX|";
                j++;
            }
            else
                fo_cr << "  |";
        }
        fo_cr << endl;
    }
    fo_cr.close();

    return 0;
}