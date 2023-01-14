// Jordan Dehmel, 2023

#include <cassert>
#include <iostream>
#include <regex>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <sstream>

#include "tags.hpp"
#include "getOS.hpp"

using namespace std;

string formatFilename(string toFormat)
{
    string out = toFormat;
    transform(out.begin(), out.end(), out.begin(), ::toupper);
    out = regex_replace(out, regex("\\."), "_");

    return out;
}

bool loadData(const string dataFilePath, string &name, string &email, string &website, int &year, string &yearSuffix,
              string &license, string &other, bool &doDefault)
{
}

int main(const int argc, const char *argv[])
{
    string dataFilePath;
    if (OS == Linux)
    {
        dataFilePath = "~/.touch-cpp_data.txt";
    }
    else
    {
        dataFilePath = "$HOME/.touch-cpp_data.txt";
    }

    // Prepare
    string name = "Joe Smith";
    string email = "example@example.com";
    string website = "github.com/example";

    int year = 1970 + (time(NULL) / (365.25 * 24 * 60 * 60));
    string yearSuffix = " - present";
    string license = "MIT licence via mit-license.org held by author";
    string other = "";

    bool doDefault = true;

    string filename = "";

    loadData(dataFilePath, name, email, website, year, yearSuffix, license, other, doDefault);

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
                name = argv[i];

                break;
            case 'e':
                // email
                email = argv[i];
                break;
            case 'w':
                // website
                website = argv[i];
                break;
            case 'y':
                // year
                year = stoi(argv[i]);
                break;
            case 's':
                // year suffix
                yearSuffix = argv[i];
                break;
            case 'l':
                // license
                license = argv[i];
                break;
            case 'o':
                // other
                other = argv[i];
                break;
            case 'd':
                // doDefault
                doDefault = (argv[i][0] == 't');
                break;

            // Terminal cases
            case 'h':
                // help
                cout << tags::yellow_bold
                     << "    | Meaning     | Usage\n"
                     << "-------------------------------------------------\n"
                     << " -a | Author      | -a \"Joe Smith\"\n"
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
         << name << '\n'
         << email << '\n'
         << website << '\n'
         << year << yearSuffix << '\n'
         << license << "\n";
    if (other != "")
    {
        file << other << '\n';
    }
    file << "*/\n\n";

    // Suffix-based specialization
    if (doDefault)
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
