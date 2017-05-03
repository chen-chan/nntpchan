#include "nntp_handler.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>

namespace nntpchan
{
  NNTPServerHandler::NNTPServerHandler(const std::string & storage) :
    LineReader(1024),
    m_auth(nullptr),
    m_store(storage),
    m_authed(false),
    m_state(eStateReadCommand)
  {
  }

  NNTPServerHandler::~NNTPServerHandler()
  {
    if(m_auth) delete m_auth;
  }

  void NNTPServerHandler::HandleLine(const std::string &line)
  {
    if(m_state == eStateReadCommand) {
      std::deque<std::string> command;
      std::istringstream s;
      s.str(line);
      for (std::string part; std::getline(s, part, ' '); ) {
          if(part.size()) command.push_back(std::string(part));
      }
      if(command.size())
        HandleCommand(command);
      else
        QueueLine("501 Syntax error");
    }
  }

  void NNTPServerHandler::OnData(const char * data, ssize_t l)
  {
    Data(data, l);
  }

  void NNTPServerHandler::HandleCommand(const std::deque<std::string> & command)
  {
    auto cmd = command[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::size_t cmdlen = command.size();
    std::cerr << "handle command [" << cmd << "] " << (int) cmdlen << std::endl;
    if (cmd == "QUIT") {
      Quit();
      return;
    } else if (cmd == "MODE" ) {
      if(cmdlen == 2) {
        // set mode
        SwitchMode(command[1]);
      } else if(cmdlen) {
        // too many arguments
        QueueLine("500 too many arguments");
      } else {
        // get mode
        QueueLine("500 wrong arguments");
      }
    } else {
      // unknown command
      QueueLine("500 Unknown Command");
    }
  }

  void NNTPServerHandler::SwitchMode(const std::string & mode)
  {
    std::string m = mode;
    std::transform(m.begin(), m.end(), m.begin(), ::toupper);
    if (m == "READER") {
      m_mode = m;
      if(PostingAllowed()) {
        QueueLine("200 Posting is permitted yo");
      } else {
        QueueLine("201 Posting is not permitted yo");
      }
    } else if (m == "STREAM") {
      m_mode = m;
      if (PostingAllowed()) {
        QueueLine("203 Streaming enabled");
      } else {
        QueueLine("483 Streaming Denied");
      }
    } else {
      // unknown mode
      QueueLine("500 Unknown mode");
    }
  }

  void NNTPServerHandler::Quit()
  {
    std::cerr << "quitting" << std::endl;
    m_state = eStateQuit;
    QueueLine("205 quitting");
  }

  bool NNTPServerHandler::ShouldClose()
  {
    return m_state == eStateQuit;
  }

  bool NNTPServerHandler::PostingAllowed()
  {
    return m_authed || m_auth == nullptr;
  }

  void NNTPServerHandler::Greet()
  {
    if(PostingAllowed())
      QueueLine("200 Posting allowed");
    else
      QueueLine("201 Posting not allowed");
  }

  void NNTPServerHandler::SetAuth(NNTPCredentialDB *creds)
  {
    if(m_auth) delete m_auth;
    m_auth = creds;
  }
}
