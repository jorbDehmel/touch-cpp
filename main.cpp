// Jordan Dehmel, 2023

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cassert>

#include "tags.hpp"
#include "getOS.hpp"

using namespace std;

/////////////////////////////////////////

struct Info
{
    string name;
    string email;
    string website;
    long year;
    string yearSuffix;
    string license;
    string other;
    bool doDefault;
};

/////////////////////////////////////////

string formatFilename(string toFormat)
{
    string out = toFormat;
    transform(out.begin(), out.end(), out.begin(), ::toupper);
    replace(out.begin(), out.end(), '.', '_');

    return out;
}

// returns true on success
bool loadData(const string dataFilePath, Info &info)
{
    ifstream file(dataFilePath);
    if (!file.is_open())
    {
        cout << "During loading of " << dataFilePath << ":\n";
        cout << tags::red_bold
             << "Error: Failed to load data file. Defaults will be used.\n"
             << tags::reset;

        return false;
    }

    getline(file, info.name);
    getline(file, info.email);
    getline(file, info.website);

    string yearTemp;
    getline(file, yearTemp);
    info.year = stoi(yearTemp);

    getline(file, info.yearSuffix);
    getline(file, info.license);
    getline(file, info.other);

    string defaultTemp;
    getline(file, defaultTemp);
    info.doDefault = (defaultTemp == "true");

    file.close();

    return true;
}

// returns true on success
void saveData(const string dataFilePath, const Info &info)
{
    ofstream file(dataFilePath, ios::out);
    if (!file.is_open())
    {
        cout << "During loading of " << dataFilePath << ":\n";
        cout << tags::red_bold
             << "Error: Failed to load data file for output.\n"
             << tags::reset;

        return;
    }

    file << info.name << '\n'
         << info.email << '\n'
         << info.website << '\n'
         << to_string(info.year) << '\n'
         << info.yearSuffix << '\n'
         << info.license << '\n'
         << info.other << '\n'
         << (info.doDefault
                 ? "true"
                 : "false");

    file.close();

    return;
}

/////////////////////////////////////////

int main(const int argc, const char *argv[])
{
    string dataFilePath;
    if (OS == Linux)
    {
        // I cannot for the life of me get this to work without system calls
        system("echo $HOME > temp.txt");
        ifstream fin("temp.txt");
        assert(fin.is_open());
        fin >> dataFilePath;
        fin.close();

        dataFilePath += "/touch-cpp_data.txt";
    }
    else
    {
        dataFilePath = "$HOME/touch-cpp_data.txt";
    }

    // Prepare
    Info info{"James Smith", "example@example.com", "github.com/example", 1970 + (time(NULL) / 31557600),
              " - present", "MIT licence via mit-license.org held by author", "", true};

    string filename = "";

    loadData(dataFilePath, info);

    /////////////////////////////////////////

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (i + 1 >= argc && argv[i][1] != 'h')
            {
                cout << tags::red_bold
                     << "Error: Too few arguments for option "
                     << argv[i] << '\n'
                     << tags::reset;

                return 1;
            }

            char flag = argv[i][1];
            i++;
            switch (flag)
            {
            // Flags
            case 'a':
                // author
                info.name = argv[i];

                break;
            case 'e':
                // email
                info.email = argv[i];
                break;
            case 'w':
                // website
                info.website = argv[i];
                break;
            case 'y':
                // year
                info.year = stoi(argv[i]);
                break;
            case 's':
                // year suffix
                info.yearSuffix = argv[i];
                break;
            case 'l':
                // license
                info.license = argv[i];
                break;
            case 'o':
                // other
                info.other = argv[i];
                break;
            case 'd':
                // doDefault
                info.doDefault = (argv[i][0] == 't');
                break;

            // Terminal cases
            case 'h':
                // help
                cout << tags::yellow_bold
                     << "    | Meaning     | Usage\n"
                     << "-------------------------------------------------\n"
                     << " -a | Author      | -a \"James Smith\"\n"
                     << " -d | Do default  | -d true\n"
                     << " -e | Email       | -e jsmith@foobar.com\n"
                     << " -h | Help        | -h\n"
                     << " -l | Lisence     | -l \"MIT via mit-license.org\"\n"
                     << " -o | Other       | -o \"Have a nice day!\"\n"
                     << " -s | Year suffix | -s \" - present\"\n"
                     << " -w | Website     | -w www.jsmith.com/\n"
                     << " -y | Year        | -y 1912\n"
                     << tags::reset;

                return 0;
            default:
                cout << tags::red_bold
                     << "Error: Unrecognized flag \"-"
                     << flag << "\"\n"
                     << tags::reset;

                return 3;
            }
        }
        else
        {
            filename = argv[i];
        }
    }

    saveData(dataFilePath, info);

    /////////////////////////////////////////

    if (filename.size() == 0)
    {
        cout << tags::red_bold
             << "Error: No filepath provided.\n"
             << tags::reset;
        return 2;
    }

    FILE *realfile;
    realfile = fopen(filename.c_str(), "wx");

    if (realfile == NULL)
    {
        // This is a success because the corrosponding `touch` case is.
        return 0;
    }

    stringstream file;

    file << "/*\n"
         << info.name << '\n'
         << info.email << '\n'
         << info.website << '\n'
         << info.year << info.yearSuffix << '\n'
         << info.license << "\n";
    if (info.other != "")
    {
        file << info.other << '\n';
    }
    file << "*/\n\n";

    // Suffix-based specialization
    if (info.doDefault)
    {
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".cpp")
        {
            file << "#include <iostream>\n"
                    "using namespace std;\n\n"
                    "int main(const int argc, const char *argv[])\n"
                    "{\n"
                    "    cout << \"Hello, world!\" << endl;\n"
                    "    return 0;\n"
                    "}\n";
        }
        else
        {
            string formatted = formatFilename(filename);
            file << "#ifndef " + formatted + "\n"
                 << "#define " + formatted + "\n\n"
                 << "class FooBar\n"
                 << "{\n"
                 << "public:\n"
                 << "    void fizzBuzz();\n"
                 << "};\n\n"
                 << "#endif\n";
        }
    }

    /////////////////////////////////////////

    fputs(file.str().c_str(), realfile);
    fclose(realfile);

    return 0;
}
