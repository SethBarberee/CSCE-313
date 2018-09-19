#include <iostream>

std::string parse_command(std::string command, int index){
// TODO implement parser recursively
    int new_index = index;
    std::string to_parse = command;
    std::string parsed = "";
    std::string sub_parse = "";
    std::cout << "Command entered: " << to_parse << std::endl;
    for(int i = new_index; i < to_parse.length(); i++){
        char to_parse_char = to_parse[i];
        switch(to_parse_char){
            case '&':
                        std::cout << "TODO implement forking" << std::endl;
                        break;
            case '|':
                        // TODO Parse the other side
                        sub_parse = to_parse.substr(i+1);
                        std::cout << "Checking " << sub_parse << std::endl;
                        parsed = parsed +  parse_command(sub_parse, i+1);
                        break;
            case '>':
            case '<':
                        std::cout << "TODO implement redirection" << std::endl;
                        break;
            case 'c':
                        if(to_parse[i+1] == 'd'){
                            std::cout << "TODO implement changing directory" << std::endl;
                        }
                        parsed = parsed + to_parse_char;
                        break;
            default:
                        parsed = parsed + to_parse_char;
                        break;
        }
    }
    std::cout << "TODO execute: " << parsed << std::endl;
    return parsed; 
};
