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

    string templateFile;
    string templateHeader;
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

    getline(file, info.templateFile);
    getline(file, info.templateHeader);

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
                 : "false")
         << '\n'
         << info.templateFile << '\n'
         << info.templateHeader;

    file.close();

    return;
}

/////////////////////////////////////////

int main(const int argc, const char *argv[])
{
    string homeDir;
    string dataFilePath;
    if (OS == Linux)
    {
        // I cannot for the life of me get this to work without system calls
        system("echo $HOME > temp.txt");
        ifstream fin("temp.txt");
        assert(fin.is_open());
        fin >> homeDir;
        fin.close();
        system("rm -f temp.txt");

        dataFilePath = homeDir + "/touch-cpp_data.txt";
    }
    else
    {
        homeDir = "$HOME/";
        dataFilePath = "$HOME/touch-cpp_data.txt";
    }

    // Prepare
    Info info{"James Smith", "example@example.com", "github.com/example", 1970 + (time(NULL) / 31557600),
              " - present", "MIT licence via mit-license.org held by author", "", true, "", ""};

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
            case 'c':
                // Template filepath
                info.templateFile = argv[i];
                break;
            case 't':
                // Template header filepath
                info.templateHeader = argv[i];
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
                     << " -c | .c template | -c defaultToAdd.cpp\n"
                     << " -d | Do default  | -d true\n"
                     << " -e | Email       | -e jsmith@foobar.com\n"
                     << " -h | Help        | -h\n"
                     << " -l | Lisence     | -l \"MIT via mit-license.org\"\n"
                     << " -o | Other       | -o \"Have a nice day!\"\n"
                     << " -s | Year suffix | -s \" - present\"\n"
                     << " -t | .h template | -t defaultToAdd.hpp\n"
                     << " -w | Website     | -w www.jsmith.com/\n"
                     << " -y | Year        | -y 1912\n"
                     << "-------------------------------------------------\n"
                     << "    These options all modify the default, rather\n"
                     << "than being single-use. To use a template, create\n"
                     << "a *.cpp file in your $HOME or /home/$USER dir.\n"
                     << "This will have all the code that follows the\n"
                     << "universal header.\n"
                     << "-------------------------------------------------\n"
                     << "Current .cpp template: " << info.templateFile << '\n'
                     << "Current .h/.hpp template: " << info.templateHeader << '\n'
                     << "-------------------------------------------------\n"
                     << "(Jordan 'Jorb' Dehmel, 2023, jdehmel@outlook.com)\n"
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

    if ((filename.size() > 2 && filename.substr(filename.size() - 2) == ".h") ||
        (filename.size() > 4 && (filename.substr(filename.size() - 4) == ".cpp" ||
                                 filename.substr(filename.size() - 4) == ".hpp")))
    {
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
    }

    // Suffix-based specialization
    if (info.doDefault)
    {
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".cpp")
        {
            if (info.templateFile == "")
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
                // load template here
                ifstream templateFile(homeDir + '/' + info.templateFile);
                if (!templateFile.is_open())
                {
                    cout << tags::red_bold
                         << "Could not open specified template file \""
                         << homeDir << '/' << info.templateFile << "\".\n"
                         << "Ensure this file exists.\n"
                         << tags::reset;
                }

                string line;
                while (getline(templateFile, line))
                {
                    file << line << '\n';
                }

                templateFile.close();
            }
        }
        else if ((filename.size() > 4 && filename.substr(filename.size() - 4) == ".hpp") || (filename.size() > 2 && filename.substr(filename.size() - 2) == ".h"))
        {
            string formatted = formatFilename(filename);
            file << "#ifndef " + formatted + "\n"
                 << "#define " + formatted + "\n\n";

            if (info.templateHeader == "")
            {
                string formatted = formatFilename(filename);
                file << "class FooBar\n"
                     << "{\n"
                     << "public:\n"
                     << "    void fizzBuzz();\n"
                     << "};\n";
            }
            else
            {
                // load template here
                ifstream templateFile(homeDir + '/' + info.templateHeader);
                if (!templateFile.is_open())
                {
                    cout << tags::red_bold
                         << "Could not open specified template file \""
                         << homeDir << '/' << info.templateHeader << "\".\n"
                         << "Ensure this file exists.\n"
                         << tags::reset;
                }

                string line;
                while (getline(templateFile, line))
                {
                    file << line << '\n';
                }

                templateFile.close();
            }

            file << "\n#endif\n";
        }
    }

    /////////////////////////////////////////

    fputs(file.str().c_str(), realfile);
    fclose(realfile);

    return 0;
}
