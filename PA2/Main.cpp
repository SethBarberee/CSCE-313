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
            parse_command(command,0);
        }
    }
}
