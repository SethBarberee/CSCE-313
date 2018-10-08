#include <iostream>
#include <unistd.h> // needed for chdir
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <string.h>
using namespace std;

struct shell_token {
    int index;
    string token;
};

void execute(vector<string> args, bool do_fork){
    vector<char *> argv;
    for(auto const& a: args)
        argv.emplace_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);
    char **command_cmd = (char**)&args[0];
    if(do_fork){
        pid_t pid = fork();
        if(pid == 0){
            execvp(command_cmd[0], argv.data());
            perror("Exec failure");
            exit(1);
        }   else {
            wait(NULL);
        }
    }
    else {
        execvp(command_cmd[0], argv.data());
    }
};

// Handle any tokens that are found
int handle_token(vector<string>* commands, bool do_fork, vector<int>* background){
    vector<shell_token*> tokens;
    // Accumulate all the tokens

    for(unsigned long index = 0; index < commands->size(); index++){
        if(commands->at(index) == "cd" || commands->at(index) == "jobs" || commands->at(index) == ">" || commands->at(index) == "<" || commands->at(index) == "&" || commands->at(index) == ">>" || commands->at(index) == "<<"){
            if(commands->at(index) == "cd" || commands->at(index) == "jobs"){
                if(index != 0){
                    continue;
                }    
            }
            shell_token* token1 = new shell_token();
            token1->index = index;
            token1->token = commands->at(index);
            tokens.push_back(token1);
        }
    }

    while(tokens.size() > 0){
        shell_token* current_token = tokens.front();
        if(current_token->token == ">"){
            vector<string> left_command(commands->begin(), commands->begin()+current_token->index);
            string file_name = commands->at(current_token->index + 1);
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
            commands->erase(commands->begin(), commands->begin()+current_token->index+2);
        } else if(current_token->token == "<"){
            vector<string> left_command(commands->begin(), commands->begin()+current_token->index);
            string file_name = commands->at(current_token->index + 1);
            int fd = open(file_name.c_str(), O_RDONLY);
            vector<char *> argv;
            for(auto const& a: left_command)
                argv.emplace_back(const_cast<char*>(a.c_str()));
            argv.push_back(nullptr);
            char **command_cmd = (char**)&left_command[0];
            pid_t pid = fork();
            if(pid == 0){
                dup2(fd,0);
                execvp(command_cmd[0], argv.data());
                perror("Exec failure");
                exit(1);
            }
            else {
                wait(NULL);
            }
            commands->erase(commands->begin(), commands->begin()+current_token->index+2);
        } else if(current_token->token == ">>"){
            vector<string> left_command(commands->begin(), commands->begin()+current_token->index);
            string file_name = commands->at(current_token->index + 1);
            int fd = open(file_name.c_str(), O_CREAT|O_APPEND|O_WRONLY, S_IRUSR | S_IWUSR);
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
                close(fd);
            }
            commands->erase(commands->begin(), commands->begin()+current_token->index+2);
        } else if(current_token->token == "<<"){
            vector<string> left_command(commands->begin(), commands->begin()+current_token->index);
            string file_name = commands->at(current_token->index + 1);
            int fd = open(file_name.c_str(), O_RDONLY);
            vector<char *> argv;
            for(auto const& a: left_command)
                argv.emplace_back(const_cast<char*>(a.c_str()));
            argv.push_back(nullptr);
            char **command_cmd = (char**)&left_command[0];
            pid_t pid = fork();
            if(pid == 0){
                dup2(fd,0);
                execvp(command_cmd[0], argv.data());
                perror("Exec failure");
                exit(1);
            }
            else {
                wait(NULL);
                close(fd);
            }
            commands->erase(commands->begin(), commands->begin()+current_token->index+2);
        }
        else if(current_token->token == "cd"){
            string directory = commands->at(current_token->index + 1);
            chdir(directory.c_str());
            commands->erase(commands->begin(), commands->begin()+2);
        }
        else if(current_token->token == "&"){
            vector<char *> argv;
            vector<string> sub_command(commands->begin(), commands->begin()+current_token->index);
            for(auto const& a: sub_command)
                argv.emplace_back(const_cast<char*>(a.c_str()));
            argv.push_back(nullptr);
            char **command_cmd = (char**)&sub_command[0];
            pid_t pid = fork();
            if(pid == 0){
                execvp(command_cmd[0], argv.data());
                perror("Exec failure");
            }
            else {
                background->push_back(pid);
                waitpid(WNOHANG, NULL, 0);
                return 0;
            } 
            commands->erase(commands->begin(), commands->begin()+current_token->index);
        }
        else if(current_token->token == "jobs"){
            for(int i = 0; i < background->size(); i++){
                cout << background->at(i) << endl;
            }
            commands->erase(commands->begin(), commands->end());
        }
        tokens.erase(tokens.begin());
    }
    if(commands->size() > 0){
        execute(*commands, do_fork);
    }
    return 0;
};

void handle_two_pipes(vector<shell_token*>* pipes, vector<string>* commands, int num_pipes, vector<int>* background){
    int pipe1[2];
    int pipe_past;
    int num_commands = num_pipes + 1;
    int index = 0;
    pid_t child;
    vector<string> part1;
    vector<string> part2;
    vector<string> part3;
    vector<string> part4;
    vector<string> current;
    shell_token* current_token;
    shell_token* next_token;
    shell_token* next_token2;
    current_token = pipes->at(0);
    if(num_pipes == 2) {
        next_token = pipes->at(1);
        part3.assign(commands->begin() + next_token->index + 1, commands->end());
        part2.assign(commands->begin() + current_token->index + 1, commands->begin() + next_token->index);
    }
    else if(num_pipes == 3){
        next_token = pipes->at(1);
        next_token2 = pipes->at(2);
        part2.assign(commands->begin() + current_token->index + 1, commands->begin() + next_token->index);
        part3.assign(commands->begin() + next_token->index + 1, commands->begin() + next_token2->index);
        part4.assign(commands->begin() + next_token2->index + 1, commands->end());
    }

    part1.assign(commands->begin(), commands->begin() + current_token->index);


    while(index < num_commands){
        pipe(pipe1);
        if(index == 0){
            current = part1;
        }
        else if(index == 1){
            current = part2;
        }
        else if(index == 2){
            current = part3;
        }
        else if(index == 3){
            current = part4;
        }
        if((child = fork()) == 0){
            dup2(pipe_past, 0);
            if((index + 1) != num_commands){
                    dup2(pipe1[1],1);
            }
            close(pipe1[0]);
            execute(current, 0);
        } else {
            wait(NULL);
            close(pipe1[1]);
            pipe_past = pipe1[0];
            index++;
        }
    }
}


void handle_pipes(vector<shell_token*>* pipes, vector<string>* commands, int num_pipes, vector<int>* background){
    if(num_pipes > 1){
          handle_two_pipes(pipes, commands, num_pipes, background);
    }
    else if(num_pipes == 1){
            shell_token* current_token = pipes->at(0);
            int fds[2];
            pid_t pid1;
            pid_t pid2;
            vector<string> args;
            vector<string> half;
            args.assign(commands->begin(), commands->begin() + current_token->index);
            half.assign(commands->begin() + current_token->index + 1, commands->end());
            pipe(fds);
            if((pid2 = fork()) == 0){
                dup2(fds[0], 0);
                close(fds[1]);
                handle_token(&half,0,background);
            } else if((pid1 = fork()) == 0){
                dup2(fds[1], 1);
                close(fds[0]);
                handle_token(&args,0,background);
            }
            else {
                waitpid(pid1,NULL,0);
                close(fds[0]);
                close(fds[1]);
                num_pipes--;
            }
            waitpid(pid2, NULL, 0);
            close(fds[0]);
            close(fds[1]);
            commands->erase(commands->begin(), commands->begin() + current_token->index  + 1);
    } 
    else {
        handle_token(commands, 1, background);
    }
}

void parse_command(std::string command, vector<int> background){
    vector<string> commands;
    vector<shell_token*> pipes;
    int num_pipes = 0;
    bool is_string = false;
    string long_string;
    string command_string;
    string current = command;
    while (current.size() > 0){
        char current_char = current.front();
        if(current_char == '"' || (current_char == *"'")){
            is_string = !is_string;
        }
        if(is_string && (current_char != '"' && current_char != *"'")){
            long_string = long_string + current_char;
            current.erase(current.begin());
        }
        else {
            if(current_char == *" "){
                if(command_string.size() > 0){
                    commands.push_back(command_string);
                    command_string.clear();
                    current.erase(current.begin());
                }
            }
            else {
                if(current_char != '"' && current_char != *"'"){
                    command_string = command_string + current_char;
                    current.erase(current.begin());
                }
                else {
                    current.erase(current.begin());
                }
            }
        }   
    }
    if(command_string.size() > 0){
        commands.push_back(command_string);
    }
    if(long_string.size() > 0){
        commands.push_back(long_string);
    }
    // Accumulate all the pipe

    for(unsigned long index = 0; index < commands.size(); index++){
        if(commands[index] == "|"){
            shell_token* token1 = new shell_token();
            token1->index = index;
            token1->token = commands[index];
            pipes.push_back(token1);
            num_pipes++;
        }
    }
    handle_pipes(&pipes, &commands, num_pipes, &background);
};
