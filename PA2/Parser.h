#include <iostream>

std::string parse_command(std::string command, int index){
// TODO implement parser recursively
    int new_index = index;
    std::string to_parse = command;
    std::string parsed = "";
    std::string left_half = "";
    std::string right_half = "";
    std::cout << "Command entered: " << to_parse << std::endl;
    // Iterate from right to left so the recursion will execute the left-most command first
    for(int i = to_parse.length(); i >= new_index; i--){
        char to_parse_char = to_parse[i];
        switch(to_parse_char){
            case '&':
                        std::cout << "TODO implement forking" << std::endl;
                        break;
            case '|':
                        left_half = to_parse.substr(0,i-1);
                        right_half = to_parse.substr(i+2);
                        std::cout << "Checking " << left_half << std::endl;
                        // Parse the left half
                        parse_command(left_half, 0);
                        // Parse the right half
                        parse_command(right_half,0);
                        break;
            case '>':
            case '<':
                        std::cout << "TODO implement redirection" << std::endl;
                        left_half = to_parse.substr(0,i-1);
                        right_half = to_parse.substr(i+2);
                        parse_command(left_half, 0);
                        parse_command(right_half,0);
                        break;
            case 'd':
                        if(to_parse[i-1] == 'c'){
                            std::cout << "TODO implement changing directory" << std::endl;
                        }
                        parsed = to_parse_char + parsed;
                        break;
            case '-':
                        if(to_parse[i-1] == ' '){
                            std::cout << "Argument detected" << std::endl;
                        }
                        parsed = to_parse_char + parsed;
                        break;
            default:
                        parsed = to_parse_char + parsed;
                        break;
        }
    }
    std::cout << "TODO execute: " << parsed << std::endl;
    return parsed; 
};
