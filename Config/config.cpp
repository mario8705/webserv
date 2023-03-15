
#include <iostream>
#include <fstream>
#include <string>
int getconfig(void)
{
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());
    std::string content;

    if (file.good())
    {
        std::string line;
        while (std::getline(file, line)) {
            content += line;
            content += '\n';
        }
        file.close();
        std::cout  << filename << " : \n" << content << std::endl;
    }
    else
    {
        std::cerr << "Error  cannot open config file\n";
        return 1;
    }
    return 0;
}

int main()
{
    getconfig();
    return(0);
}