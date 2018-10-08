#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include "Parser.h"
#include "Prompt.h"
using namespace std;



int main(int argc, char ** argv){
    int opt = 0;
    int no_prompt = 0;
    string prompt = "";
    vector<int> background;
    opt = getopt( argc, argv, "t::h?");
    while( opt != -1 ) {
        switch( opt ) {
            case 't':
                no_prompt = 1;
                break;
                 
            case 'h':   /* fall-through is intentional */
            case '?':
                break;
                 
            default:
                /* You won't actually get here. */
                break;
        }
         
        opt = getopt( argc, argv, "t::h?" );
    }
    while(true){
        string command;
        if(!no_prompt){
            prompt = update_prompt() + " ";
        }
        else {
            prompt = "";
        }
        cout << prompt;
        // show the prompt
        // get the command entered
        getline(cin,command);
        if(command == "exit"){
            cout << "Exiting..." << endl;
            return 0;
        }
        else {
            parse_command(command, background);

        }
        for(int i = 0; i < background.size(); i++){
            pid_t pid = fork();
            int status = 0;
            if(pid == 0){
                status = waitpid(background.at(i), NULL, WNOHANG);
            } else {
                wait(NULL);
                if(status > 0){
                    background.erase(background.begin() + i);
                }
            }
        }
    }
}
