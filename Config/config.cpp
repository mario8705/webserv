#include "config.hpp"
config::config(std::string content) : _stringconfig(content)
{
    std::cout << "config on\n";
}

server::server(std::string content) : _stringconfig(content)
{
    std::cout << "server creation\n";
}

config::~config() {
    std::cout << "config destructor\n";
}

server::~server() {
    //std::cout << "server destructor\n";
}

std::string config::getStringConfig() {return(_stringconfig);}
std::string server::getStringConfig() {return(_stringconfig);}

std::vector<std::string>	split_with_space(std::string line)
{
    std::vector<std::string>	words;

    if (line.size() == 0)
        return (words);
    int k = 0;
    int j = -1;
    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == ' ' || line[i] == '\t')
        {
            if (i != 0)
                k++;
            while (line[i] == ' ' || line[i] == '\t')
                i++;
            i--;
        }
        else
        {
            if (j != k)
            {
                j = k;
                words.push_back("");
            }
            words[k].push_back(line[i]);
        }
    }
    return (words);
}

std::vector<std::string> tokenization(std::string line, std::vector<std::string> &conf)
{
    std::vector<std::string> clean;
    std::vector<std::string> tmp;
    std::vector<std::string> tmp2;



    size_t is_title = line.find('{');
    if (is_title != std::string::npos)
    {
        conf.push_back("Title : " + line);
        conf.push_back("End of line \n");
        return(conf);
    }
    tmp = split_with_space(line);
    size_t j = 0;
    for (size_t i = 0; i < tmp.size(); i++)
    {
         if (tmp[i] == "{")
            conf.push_back("Open bracket : " + tmp[i]);
        else if (tmp[i] == "}")
            conf.push_back("Close bracket : " + tmp[i]);
        else
            conf.push_back("Text : " + tmp[i]);
    }
    conf.push_back("End of line \n");
    return(conf);
}

std::vector<std::string> getconfigfile(void)
{
    std::string filename = "../config.conf";
    std::ifstream file(filename.c_str());
    std::string content;
    std::vector<std::string> final;
    std::vector<std::string> test;

    if (file.good())
    {
        int it;
        std::string line;
        while (std::getline(file, line, '\n')) {
            if (line.size() != 0)
                tokenization(line, final);
        }

        file.close();
    }
    else
        exit(1);
    for (size_t i = 0; i < final.size(); i++)
    {
        size_t isnl = final[i].find('\n');
        if (isnl != std::string::npos)
            std::cout << "END OF LINE SPOTTED || ";
        std::cout << final[i] << std::endl;
    }
    return(final);
}

int main()
{
    std::vector<std::string> infile;
    infile = getconfigfile();
    //config conf(infile);
}