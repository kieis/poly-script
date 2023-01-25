#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm> 
#include <cstdlib>
#include <set>

using namespace std;

#pragma warning(disable: 4996)

int myrandom(int i) { return std::rand() % i; }

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin)

#define MACRO_PREFIX "poly_"
#define MACRO_POSTFIX ""

#define MACRO_SWAP_START "[" MACRO_PREFIX "swap_lines" MACRO_POSTFIX "]"
#define MACRO_SWAP_END "[/" MACRO_PREFIX "swap_lines" MACRO_POSTFIX "]"

#define MACRO_ENUM_JUNK_START "[" MACRO_PREFIX "enum_junk" MACRO_POSTFIX "]"
#define MACRO_ENUM_JUNK_END "[/" MACRO_PREFIX "enum_junk" MACRO_POSTFIX "]"

#define COMMAND_CLEAR_JUNK "clear_enum_junks"
#define COMMAND_ENUM_JUNK_MAX "enum_junk_max"
#define COMMAND_ENABLE_BACKUP "enable_backup"
#define COMMAND_PAUSE_PROMP "pause_prompt"

struct ThreadInfo {
public:
    vector<string> fileName;
    vector<string> commands;
    bool bCommands[5] = { false, false, false, false, false };
    int junk_max = 8;
};
ThreadInfo threadInfo;
int global_thread_counter = -1;

std::vector<std::string> splitpath(const std::string& str, const std::set<char> delimiters) {
    std::vector<std::string> result;

    char const* pch = str.c_str();
    char const* start = pch;
    for (; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);
                result.push_back(str);
            }
            else
            {
                result.push_back("");
            }
            start = pch + 1;
        }
    }
    result.push_back(start);

    return result;
}

string get_exe_path() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

void clear_enum_junk(vector<string>& file, bool bPrint = true) {
    int count = 0;
loop:
    for (int i = 0; i < file.size(); i++) {
        if (strstr(file[i].c_str(), "eJUNK_GEN_")) {
            count++;
            file.erase(file.begin() + i);
            goto loop;
        }
    }

    if (bPrint) {
        cout << "cleared " << count << " enum junkeds." << endl;
    }
}

void apply_enum_junk(vector<string>& file, int junk_max) {
    vector<int> start_index,
        end_index;

    clear_enum_junk(file, false);

    for (int i = 0; i < file.size(); i++) {
        bool bFound = false;
        if (strstr(file[i].c_str(), MACRO_ENUM_JUNK_START)) {
            bFound = true;
        }
        if (bFound) {
            if ((i + 2) < file.size()) {
                if (strstr(file[i + 1].c_str(), MACRO_PREFIX)) {
                    start_index.push_back(i + 2);
                }
                else {
                    start_index.push_back(i);
                }
            }
        }
    }

    for (int i = 0; i < file.size(); i++) {
        bool bFound = false;
        if (strstr(file[i].c_str(), MACRO_ENUM_JUNK_END)) {
            bFound = true;
        }
        if (bFound) {
            if ((i - 1) > 0) {
                if (strstr(file[i - 1].c_str(), MACRO_PREFIX)) {
                    end_index.push_back(i - 1);
                }
                else {
                    end_index.push_back(i);
                }
            }
        }
    }

    if (start_index.size() > 0 && end_index.size() > 0 && start_index.size() == end_index.size()) {
        vector<string> items_junked;

        int stack = 0;

        for (int macroI = 0; macroI < start_index.size(); macroI++) {
            items_junked.clear();

            int rnd_junk = RandomInt(1, junk_max);

            for (int i = 0; i < rnd_junk; i++) {
                int rnd = RandomInt(1000000, 1999999);
                char buffer[256];
                sprintf(buffer, "	eJUNK_GEN_%d,", rnd);

                items_junked.push_back(buffer);
            }

            if ((macroI + 1) < start_index.size()) {
                stack += items_junked.size();

                start_index[macroI + 1] += stack;
                end_index[macroI + 1] += stack;
            }


            if (items_junked.size() > 0) {
                for (int i = 0; i < items_junked.size(); i++) {
                    int rnd1 = start_index[macroI] + 1;
                    int rnd2 = end_index[macroI];

                    int rnd_position = RandomInt(rnd1, rnd2);
                    file.insert(file.begin() + rnd_position, items_junked[i].c_str());
                }
            }
        }
    }

    cout << "enums junkeds: " << start_index.size() << " macros." << endl;
}

void apply_swap_lines(vector<string>& file) {
    vector<int> start_index,
        end_index;

    for (int i = 0; i < file.size(); i++) {
        if (strstr(file[i].c_str(), MACRO_SWAP_START)) {
            start_index.push_back(i);
        }
    }

    for (int i = 0; i < file.size(); i++) {
        if (strstr(file[i].c_str(), MACRO_SWAP_END)) {
            end_index.push_back(i);
        }
    }

    if (start_index.size() > 0 && end_index.size() > 0 && start_index.size() == end_index.size()) {
        vector<string> items_to_swap;

        for (int macroI = 0; macroI < start_index.size(); macroI++) {
            items_to_swap.clear();

            for (int i = start_index[macroI] + 1; i < end_index[macroI]; i++) {
                items_to_swap.push_back(file[i]);
            }

            if (items_to_swap.size() > 0) {
                std::random_shuffle(items_to_swap.begin(), items_to_swap.end(), myrandom);

                int i_swap = 0;
                for (int i = start_index[macroI] + 1; i < end_index[macroI]; i++) {
                    if (i_swap < items_to_swap.size()) {
                        file[i].replace(file[i].begin(), file[i].end(), items_to_swap[i_swap].c_str());
                        i_swap++;
                    }
                }
            }
        }
    }

    cout << "swapped: " << start_index.size() << " macros." << endl;
}

std::vector<std::string> read_file_as_vector(const std::string& filename) {
    std::ifstream source;
    source.open(filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(source, line)) {
        lines.push_back(line);
    }
    return lines;
}

void write_file_as_vector(const std::string& filename, std::vector<std::string> data) {
    std::ofstream outFile(filename.c_str());
    for (const auto& e : data) {
        outFile << e << "\n";
    }
}

void print_init() {
    cout << "============= Available Macros =============" << endl;
    cout << endl;

    cout << ">> " << MACRO_SWAP_START << " >> start definition." << endl;
    cout << ">> " << MACRO_SWAP_END << " >> end definition." << endl;

    cout << ">> " << MACRO_ENUM_JUNK_START << " >> start definition." << endl;
    cout << ">> " << MACRO_ENUM_JUNK_END << " >> end definition." << endl;
    cout << endl;

    cout << "================= Commands =================" << endl;
    cout << endl;

    cout << ">> " << COMMAND_CLEAR_JUNK << " >> will remove all enum randomic junkes in the file." << endl;
    cout << ">> " << COMMAND_ENUM_JUNK_MAX << " >> max junks that can be random generated." << endl;
    cout << ">> " << COMMAND_ENABLE_BACKUP << " >> will create a backupfile." << endl;
    cout << ">> " << COMMAND_PAUSE_PROMP << " >> will pause cmd." << endl;
    cout << endl;

    cout << "================== Output ==================" << endl;
    cout << endl;
}

void generateThread(int i) {
    vector<string> outputFile = read_file_as_vector(threadInfo.fileName[i].c_str());
    cout << "File >> [" << threadInfo.fileName[i].c_str() << "] : " << endl;

    //backup
    if (threadInfo.bCommands[2]) {
        std::set<char> delims{ '\\' };
        std::vector<std::string> path = splitpath(threadInfo.fileName[i], delims);

        string backupFile = get_exe_path() + string("\\backup\\") + path.back();//MYMACRO +

        write_file_as_vector(backupFile, outputFile);
    }

    if (!threadInfo.bCommands[0]) {
        int randomJunk = RandomInt(1, threadInfo.junk_max);
        apply_enum_junk(outputFile, randomJunk);
    }
    else {
        clear_enum_junk(outputFile);
    }
    apply_swap_lines(outputFile);

    write_file_as_vector(threadInfo.fileName[i].c_str(), outputFile);

    cout << endl;
}

int main(int argc, char* argv[]) {
    SetConsoleTitleA("PolyOap 1.0");
    std::srand(unsigned(std::time(0)));

    print_init();

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strstr(argv[i], "-")) {
                threadInfo.commands.push_back(argv[i]);
            }
            else {
                threadInfo.fileName.push_back(argv[i]);
            }
        }

        if (!threadInfo.commands.empty()) {
            for (int i = 0; i < threadInfo.commands.size(); i++) {
                if (!threadInfo.bCommands[0]) {
                    if (strstr(threadInfo.commands[i].c_str(), COMMAND_CLEAR_JUNK)) {
                        threadInfo.bCommands[0] = true;
                    }
                }
                if (!threadInfo.bCommands[1]) {
                    if (strstr(threadInfo.commands[i].c_str(), COMMAND_ENUM_JUNK_MAX)) {
                        size_t pos = threadInfo.commands[i].find("=");
                        threadInfo.commands[i].erase(threadInfo.commands[i].begin(), threadInfo.commands[i].begin() + pos + 1);

                        threadInfo.junk_max = stoi(threadInfo.commands[i]);
                        threadInfo.bCommands[1] = true;
                    }
                }
                if (!threadInfo.bCommands[2]) {
                    if (strstr(threadInfo.commands[i].c_str(), COMMAND_ENABLE_BACKUP)) {
                        threadInfo.bCommands[2] = true;
                    }
                }
                if (!threadInfo.bCommands[3]) {
                    if (strstr(threadInfo.commands[i].c_str(), COMMAND_PAUSE_PROMP)) {
                        threadInfo.bCommands[3] = true;
                    }
                }
            }
        }

        if (!threadInfo.fileName.empty()) {
            cout << "Max Junks >> [" << threadInfo.junk_max << "]" << endl;
            cout << endl;

            for (int i = 0; i < threadInfo.fileName.size(); i++) {
                generateThread(i);
            }
        }
    }
    else {
        cout << "Error: didn't found file." << endl;
    }

    if (threadInfo.bCommands[3]) {
        getchar();
    }

    return 1;
}