#include <iostream>
#include <unistd.h> // needed for chdir
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <vector>
using namespace std;


// TODO look at Perror
// TODO look at execl
void execute(vector<string> args){
    vector<char *> argv;
    for(auto const& a: args)
        argv.emplace_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);
    char **command_cmd = (char**)&args[0];
    pid_t pid = fork();
    if(pid == 0){
        execvp(command_cmd[0], argv.data());
        perror("Exec failure");
        exit(1);
    }
    else {
        wait(NULL);
    }
}

int parse_command(std::string command){
    vector<string> commands;
    std::string to_parse = command;
    std::stringstream ss(to_parse);
    string current;
    while (ss >> current){
        commands.push_back(current);

        if(ss.peek() == ' ')
            ss.ignore();
    }
    // TODO Maybe create a list of nodes that store the index of where a special token is??
    // Test case: ls cd
    // Current behavior: tries to cd
    // Desired behavior: ls with argument cd
    // Workaround: testing if cd is the first command or not
    for(unsigned long index = 0; index < commands.size(); index++){
        if(commands[index] == "|"){
            // TODO Split into two vectors and execute both
            while(commands.size() != 0){
                int fds[2];
                pipe(fds);
                vector<string> left_command(commands.begin(), commands.begin()+index);
                vector<string> right_command(commands.begin()+index+1, commands.end());
                if(fork() == 0){
                    dup2(fds[1],1);
                    close(fds[0]);
                    // TODO exec left command
                    vector<char *> argv;
                    for(auto const& a: left_command)
                        argv.emplace_back(const_cast<char*>(a.c_str()));
                    argv.push_back(nullptr);
                    char **command_cmd = (char**)&left_command[0];
                    execvp(command_cmd[0], argv.data());
                    // TODO delete first element or find a way to modulize it
                    perror("Exec failure");
                    exit(1);
                } else {
                    wait(0);
                    close(fds[1]);
                    dup2(fds[0], 0);
                }
            }
            return 1;
        }
        else if(commands[index] == ">"){
            vector<string> left_command(commands.begin(), commands.begin()+index);
            string file_name = commands[index + 1];
            int fd = open(file_name.c_str(), O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR);
            vector<char *> argv;
            for(auto const& a: left_command)
                argv.emplace_back(const_cast<char*>(a.c_str()));
            argv.push_back(nullptr);
            char **command_cmd = (char**)&left_command[0];
            pid_t pid = fork();
            if(pid == 0){
                dup2(fd,1);
                execvp(command_cmd[0], argv.data());
                perror("Exec failure");
                exit(1);
            }
            else {
                wait(NULL);
            }
            // Done, so return successfully
            return 0;
        }
        else if(commands[index] == "<"){
            // TODO set up redirection and execute
            vector<string> left_command(commands.begin(), commands.begin()+index);
            string file_name = commands[index + 1];
            // TODO open file as use as input instead similar to above
            return 1;
        }
        else if(commands[index] == "&"){
            cout << "Fork detected" << endl;
            vector<string> left_command(commands.begin(), commands.begin()+index);
            execute(left_command);
            return 1;
        }
        else if(commands[index] == "cd"){
            cout << "Change directory" << endl;
            // test if CD is first argument, if it isn't, don't worry about it
            if(index > 0){
                break;
            }
            // TODO use chdir call
            return 1;
        }
    }
    execute(commands);
    return 0;
};


