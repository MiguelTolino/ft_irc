
#include "HandleCmds.hpp"

// HandleCmds::HandleCmds()
// :_users(), _channels(),_listToSend(),_sender(),_cmd()
// { }

HandleCmds::HandleCmds()
:_sender(),_cmd()
{ 
    _users = new ListUsers();
    _channels = new ListChannels();
    _firstTimeFlag = 0;
}

HandleCmds::~HandleCmds()
{ 
    delete _users;
}


User* HandleCmds::newUser(const int fd)
{
    return _users->createUser(fd);
}

bool HandleCmds::removeUser(const int fd)
{
    return _users->removeUser(fd);
}

void HandleCmds::sendPRIVMSG( const std::string& nick)
{
    std::cout << "inside sendPRIVMSG" << std::endl;
    ResultCmd result;
    result.addUser(_users->getUser(nick)->getFd());
    //send(_users->getUser(nick)->getFd(), msg, sizeof(msg), 0);
    
    //std::string msg = _sender + PRIVMSG + _cmd->getParams();
    
    // que en sender se cargue el getFullName nick@user@server que debe devolver el usuario


}

std::list<ResultCmd> HandleCmds::mierdaDeFuncionDeMiguelQueNoSabeProgramarNiEscuchar()
{
    std::list<ResultCmd> results;
    return results;
}

/*The executeCmd method is the main entry point for handling commands. 
It first checks if the sender of the command is logged in, and if not, 
it returns an error message. Then, it checks the command name and calls 
the appropriate method to handle the command. For example, if the command
is "NICK", it calls the setNick method of the _users object to set the
user's nickname.*/

std::list<ResultCmd> HandleCmds::executeCmd(Command* cmd)
{    
    _cmd = cmd;
    std::string msg = _cmd->getMsg();        
    User* sender = _users->getUser(_cmd->getSender());

    // if (!sender->isLogged() && msg.find("USER") ==  std::string::npos && msg.find("NICK") ==  std::string::npos)
    //     return mierdaDeFuncionDeMiguelQueNoSabeProgramarNiEscuchar();

    std::list<ResultCmd> results;
    std::string comando = _cmd->getCommand();
    std::cout << "*****HandleCmds:mesaje: " << _cmd->getMsg() << std::endl;
    std::cout << "HandleCmds:comando: " << comando << std::endl;
    if (!_cmd->getCommand().compare("NICK"))
    {
        if (_cmd->paramsSize() == 0)
        {
            ResultCmd result;
            result.addUser(_cmd->getSender());
            results.push_back(result);
            return results;
        }
        else
        {
            ResultCmd result(_users->setNick(cmd->getNextParam(), cmd->getSender()));
            if (result.getResultCode() == 1)
            {   
                result.setMsg(ServerMsgs::getWelcomeMsg(sender->getNick(), sender->getUser()));
                result.addUser(cmd->getSender());
                results.push_back(result);
            } 
            return results;
        }
    }
    if (!_cmd->getCommand().compare("USER"))
    {
        //TODO unir parármetro en un solo nombre
        if (_cmd->paramsSize() == 0)
        {
            ResultCmd result;
            // añadir error para ERR_NEEDMOREPARAMS
            result.addUser(_cmd->getSender());
            results.push_back(result);
            return results;
        }
        else
        {            
            ResultCmd result(_users->setUser(cmd->getNextParam(), cmd->getSender()));
            if (result.getResultCode() == 1)
            {
                result.setMsg(ServerMsgs::getWelcomeMsg(sender->getNick(), sender->getUser()));
                result.addUser(cmd->getSender());
                results.push_back(result);
            }            
            return results;
        }
    }
    if (!_cmd->getCommand().compare("PRIVMSG"))
    {
        std::list<ResultCmd> results;
        if (_cmd->paramsSize() != 2)
        {
            ResultCmd result;
            // añadir msg error result.setError(ERR_NEEDMOREPARAMS);
            result.addUser(_cmd->getSender());
            results.push_back(result);
            return results;
        }
        else
        {           
            _cmd->generateTargets();
            std::queue<std::string> nicks = _cmd->getTargets();
            std::string msg = _cmd->getNextParam();
            while(!nicks.empty())
            {
                ResultCmd result;
                std::string nick = nicks.front();
                if (nick[0] == '#' || nick[0] == '!')
                {
                    Channel* channel = _channels->getChannel(nick);
                    if (channel != NULL)
                    {
                        result.setUsers(channel->getUsers());
                        result.addToMsg(sender->getFullName() + "PRIVMSG " + nick + " " + msg);
                        results.push_back(result);
                    }
                } else {
                    User* user = _users->getUser(nick);
                    if (user != NULL)
                    {   
                        result.addUser(user->getFd());
                        result.addToMsg(sender->getFullName() + "PRIVMSG " + user->getNick() + " " + msg);
                        results.push_back(result);
                    }
                    else
                    {
                        //TODO: Generar error
                    }
                }
                nicks.pop();                
            }
            return results;
        }
    }
    if (!_cmd->getCommand().compare("JOIN"))
    {
        if (_cmd->paramsSize() != 1)
        {
            ResultCmd result;
            // añadir msg error result.setError(ERR_NEEDMOREPARAMS);
            result.addUser(_cmd->getSender());
            results.push_back(result);
            return results;
        }
        else
        {           
            _cmd->generateTargets();
            std::queue<std::string> channels = _cmd->getTargets();
            while(!channels.empty())
            {                
                std::string channel = channels.front();
                std::cout << "HandleCmds:canal guardado:" << channel << std::endl;
                ResultCmd result;
                std::list<ResultCmd> msgsOfChannel = _channels->joinChannel(channel,sender);
                results.insert(results.end(),msgsOfChannel.begin(), msgsOfChannel.end());                                     
                channels.pop();
            }
            return results;
        }
    }
    if (!_cmd->getCommand().compare("CAP"))
    {
        ResultCmd result(0);
        result.addUser(_cmd->getSender());
        /*https://ircv3.net/specs/extensions/capability-negotiation.html*/
        std::cout << "HandleCmds:CAP command detected" << std::endl;
        if (_cmd->paramsSize() == 0)
        {
            
            result.setMsg("Invalid C");
            results.push_back(result);
            std::cout << "HandleCmds:ERROR: Invalid CAP command" << std::endl;
            return results;
            /* Respuesta si falla */
            //:example.org 410 jw FOO :Invalid CAP command
            //si no tiene subcomando tambien hay que devolver esta respuesta
            
        }
        else
        {
            std::string param = _cmd->getNextParam();
            if (!param.compare("LS"))
            {
                std::cout << "HandleCmds:LS command detected after CAP" << std::endl;
                result.setMsg(CAPASUPPORTANSWER);
                results.push_back(result);
                return results;
            }
            else if (!param.compare("END"))
            {
                std::cout << "HandleCmds:END command detected afert CAP" << std::endl;
                return results;
                /*The END subcommand signals to the server that capability
                negotiation is complete and requests that the server continue
                with client registration. If the client is already registered,
                this command MUST be ignored by the server.*/
            }
            else if (!param.compare("REQ"))
            {
                //REQ blindly request a set of capabilitys to the server
                //atm we will ignore them.
                int _firstTimeFlag = 0;
                std::cout << "HandleCmds:REQ command detected after CAP" << std::endl;
                if (_firstTimeFlag != 0)
                    //NAK server answers NAK when the requested capa is rejected
                    result.setMsg("CAP * NAK :"); //missing requested capas
                _firstTimeFlag = 1;

                results.push_back(result);
                return results;
            }
            else if (!param.compare("LIST"))
            {
                //LIST client request list of server capab
                std::cout << "HandleCmds:LIST command detected after CAP" << std::endl;
                //no capa enabled atm, empty answer
                result.setMsg("CAP * LIST :");
                results.push_back(result);
                return results;
            }
        }
    }
    if (!_cmd->getCommand().compare("PING"))
    {
        //ping should receive a ip, if this ip is not the same as the host, it should be send to the 
        //requested IP, but since we are not implementing a multi server irc we will answer always.
        ResultCmd result(0);
        result.addUser(_cmd->getSender());
        std::string pongReply = "PONG " + _cmd->getNextParam(); // + _hostname en IRCServer //host name ex: c2r16s2.42.madrid.com 
        result.setMsg(pongReply);
        results.push_back(result);
        return results;

        //*** los servidores IRC que voy probando mandan un PING al cliente una vez terminan de negociar capabilitys ***///
        //a veces mandan un ping con un numero delante ej: "007: PING" a lo que el cliente responde PONG 7A304BDE
        //mas ejemplos
        //2147483647: PING <-server
        //PONG 2458166833  <-cliente
    }
    if (!_cmd->getCommand().compare("PONG"))
    {
        ResultCmd result(0);
        result.addUser(_cmd->getSender());
        //no se si hace falta
        std::string pingReply = "PING " + _cmd->getNextParam();
        result.setMsg(pingReply);
        results.push_back(result);
        return results;
    }
    if (!_cmd->getCommand().compare("QUIT"))
    {
        //disconnect and delete user
    }
    if (!_cmd->getCommand().compare(PRIVMSG))
    {
        
    }
    else
    {
        std::cout << "HandleCmds:idk = " << _cmd->getCommand() << std::endl;
        ResultCmd result(0);
        result.addUser(_cmd->getSender());
        result.setMsg("IDK what u said :( " + _cmd->getMsg());
        results.push_back(result);
        return results;
    }

    delete _cmd;
    _cmd = NULL;
    return results;

}

ListUsers* HandleCmds::getUsers()
{
    return _users;
}

// void HandleCmds::sendFt(std::string msg2send, int fd)
// {
//     ResultCmd result()
//     User* sender = _users->getUser(_cmd->getSender());
//     std::cout << "Message *" << msg2send << "* to fd " << sender->getFd() << std::endl;
//     const char *answer = msg2send.c_str();
//     if (fd == 0) //send to him self
//         send(sender->getFd(), answer, std::strlen(answer), 0);
//     else
//         send(fd, answer, std::strlen(answer), 0);
// }
