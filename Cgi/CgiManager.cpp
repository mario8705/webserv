//
// Created by tgriffit on 4/3/23.
//

#include "CgiManager.hpp"
#include <cstdlib>
#include <vector>
#include <sstream>
#include <fcntl.h>

CgiManager::CgiManager(const std::string &pathToCgi, const std::map<std::string, std::string> &envMap)
    : m_path(pathToCgi)
{
    m_envList = SerializeEnvMap(envMap);
    m_mosi = -1;
    m_miso = -1;
}

CgiManager::CgiManager(const CgiManager &copy)
{
}

CgiManager::~CgiManager()
{
}

CgiManager &CgiManager::operator=(const CgiManager &toAssign) {
    return *this;
}

/**
 * Once executed you have to close the pipes yourself (the dtor doesn't do it)
 * @return
 */
pid_t CgiManager::SpawnSubProcess()
{
    std::vector<std::string> args;
    pid_t pid;
    int miso[2];
    int mosi[2];

    args.push_back(m_path);
    if (pipe(miso) < 0)
    {
        throw CgiManager::CgiException("pipe failed");
    }
    if (pipe(mosi) < 0)
    {
        close(miso[0]);
        close(miso[1]);
        throw CgiManager::CgiException("pipe failed");
    }
    if ((pid = fork()) < 0)
    {
        close(miso[0]);
        close(miso[1]);
        close(mosi[0]);
        close(mosi[1]);
        throw CgiManager::CgiException("fork failed");
    }
    if (0 == pid)
    {
        close(miso[0]);
        close(mosi[1]);
        if (dup2(miso[1], 1) < 0)
            exit(1);
        if (dup2(mosi[0], 0) < 0)
            exit(1);
        close(miso[1]);
        close(mosi[0]);
        SpawnSubProcess(m_path.c_str(), args, m_envList);
        std::cerr << "CGI failed to execute (executable not found)" << std::endl;
        exit(1);
    }
    close(miso[1]);
    close(mosi[0]);
    m_miso = miso[0];
    m_mosi = mosi[1];
    return pid;
}

std::vector<std::string> CgiManager::SerializeEnvMap(const tEnvMap &envMap)
{
    std::vector<std::string> env;
    tEnvMap::const_iterator it;

    for (it = envMap.begin(); it != envMap.end() ; ++it)
    {
        std::stringstream ss;

        ss << it->first << "=" << it->second;
        env.push_back(ss.str());
    }
    return env;
}

int CgiManager::SpawnSubProcess(const std::string &path, const std::vector<std::string> &args,
                                  const std::vector<std::string> &envs) {
    char const *argv[args.size() + 1];
    char const *envp[envs.size() + 1];
    size_t i;

    for (i = 0; i <= args.size(); ++i)
    {
        if (i < args.size())
            argv[i] = args[i].c_str();
        else
            argv[i] = NULL;
    }
    for (i = 0; i <= envs.size(); ++i)
    {
        if (i < envs.size())
            envp[i] = envs[i].c_str();
        else
            envp[i] = NULL;
    }
    return execve(path.c_str(),
                  const_cast<char * const *>(argv),
                  const_cast<char * const *>(envp));
}

int CgiManager::GetMISO() const {
    return m_miso;
}

int CgiManager::GetMOSI() const {
    return m_mosi;
}

CgiManager::CgiException::CgiException(const char *msg) throw()
    : m_message(msg)
{
}

CgiManager::CgiException::~CgiException() throw()
{
}

const char *CgiManager::CgiException::what() const throw()
{
    return m_message;
}
