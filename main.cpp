#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <iterator>

using namespace std;

const string colors[] = {"red??", "green", "orang", "black", "pinkk", "yello", "brown", "coral"};

int RandomNumber ()
{
    return rand() % (colors->size());
}

unsigned char ToByte (bool b[8])
{
    unsigned char c = 0;
    for (int i=0; i < 8; ++i)
        if (b[i])
            c |= 1 << i;
    return c;
}

bool FileExists (const string& name)
{
    struct stat buffer{};
    return (stat (name.c_str(), &buffer) == 0);
}

void Decode (const string& file, const string& result)
{
    string content;

    getline(ifstream(file.c_str()), content, '\0');
    content.erase(std::remove(content.begin(), content.end(), ':'), content.end());
    content.erase(std::remove(content.begin(), content.end(), ';'), content.end());

    cout << "Finished reading file!" << endl;

    bool n = true, tmp[8];
    vector<char> fileBytes(content.size()/(4+5));
    char pos = 0;
    size_t c = 0;
    for (size_t i = 0; i < content.size(); )
    {
        if (n) {
            i += 5;
            n = false;
        } else
        {
            string b = "";
            for (int j = 0; j < 4; ++j) b += content[i+j];
            if(b == "SUS!") tmp[pos] = true; else tmp[pos] = false;
            pos++;
            if(pos > 7)
            {
                fileBytes[c] = ToByte(tmp);
                c++;
                for (int k = 0; k < 8; k++) {
                    tmp[k] = false;
                }
                pos = 0;
            }
            i+=4;
            n = true;
        }
    }

    cout << "Finished decoding!" << endl;

    string fileBuilder;
    for (size_t i = 0; i<c; i++) fileBuilder+=(fileBytes[i]);

    //ofstream(result, ios::binary).write(fileBytes, c);
    std::ofstream writeFile;
    writeFile.open(result, std::ios::out | std::ios::binary);
    if (!fileBytes.empty())
        writeFile.write(reinterpret_cast<char*>(&fileBytes[0]),
                        fileBytes.size() * sizeof(fileBytes[0]));

    cout << "Saved file!" << endl;
    return;
}

void Encode (const string& file, const string& result)
{
    ifstream input(file, ios::binary);

    vector<char> bytes(
            (istreambuf_iterator<char>(input)),
            (istreambuf_iterator<char>()));

    input.close();

    cout << "Finished reading file!" << endl;

    string outputString = "";

    for (uint32_t i = 0; i < bytes.size(); ++i)
    {
        bitset<8> bitset(bytes[i]);
        for (int j = 0; j < 8; j++)
        {
            string sus = "SAFE";
            if (bitset[j]) sus = "SUS!";
            outputString += colors[RandomNumber()] + ":" + sus + ";";
        }
    }

    cout << "Finished encoding! (" << bytes.size() << ")" << endl;

    ofstream out(result);
    out << outputString;

    cout << "Saved file!" << endl;
    return;
}





int main (int argc, char *argv[])
{
    string prog = argv[0];
    vector<string> args;

    if (argc > 3)
    {
        if (argc > 4)
        {
            cout << "Too many arguments!" << endl;
            return 1;
        }

        args.assign(argv + 1, argv + argc);
    } else
    {
        cout << "Not enough arguments, usage: \"encode/decode [sourcefile] [destination]\"" << endl;
        return 1;
    }

    if (!FileExists(args[1]))
    {
        cout << "The source file doesn't exist!" << endl;
        return 1;
    }
    if (FileExists(args[2]))
    {
        cout << "The destination file exists already!" << endl;
        return 1;
    }

    if(args[0] == "encode")
    {
        srand(time(NULL) * getpid());
        Encode(args[1], args[2]);
    } else if(args[0] == "decode")
    {
        Decode(args[1], args[2]);
    } else
    {
        cout << "Unknown operation: " << args[1] << endl;
        return 1;
    }

    return 0;
}