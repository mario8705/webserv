#include "config.hpp"
config::config(std::vector<std::string> content) : _config(content)
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

//std::string config::getStringConfig() {return(_stringconfig);}
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

    size_t is_title = line.find('{');
    if (is_title != std::string::npos)
    {
        std::size_t first_non_space = line.find_first_not_of(" /t/n");
        std::string title_line = line.substr(first_non_space, line.length() - 1 - first_non_space);
        conf.push_back("Title : " + title_line);
        conf.push_back("Open bracket : {");
        conf.push_back("End of line \n");
        return(conf);
    }
    tmp = split_with_space(line);
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
    std::vector<server> servers;

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
        size_t isnl = final[i].find("Title : ");
        //if (isnl != std::string::npos)
            //std::cout << "Title SPOTTED || ";
        std::cout << final[i] << std::endl;
    }
    return(final);
}

void split_serv(std::vector<std::string> args)
{
    std::vector<std::vector<std::string> > final;
    size_t i = 0;
    size_t j = 0;

    std::cout << args.size();
    while (i < args.size())
    {
        //size_t title = args[i].find("Title : ");
        //if (title != std::string::npos)
        //    j++;
        final[j].push_back(args[i]);
        i++;
    }
    /*std::vector<server> servers;
    for (size_t k = 0; k < final.size(); k++)
    {
    }*/


}

int main()
{
    std::vector<std::string> infile;
    std::vector<std::vector<std::string> > final;
    infile = getconfigfile();
    //split_serv(infile);
    //config conf(infile);
}