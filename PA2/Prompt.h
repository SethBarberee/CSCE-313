// stolen from http://www.codebind.com/cpp-tutorial/c-get-current-directory-linuxwindows

#include <time.h>

// Gets the current working dir
std::string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  getcwd( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
};

// Gets Date and Time
std::string GetDate( void ){
    char buf[FILENAME_MAX];
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    std::string date_time(buf);
    return date_time;
}

// Return username
// BUG: If I change users in terminal, it doesn't like that
std::string GetUser( void ){
    char buf[FILENAME_MAX];
    getlogin_r(buf, FILENAME_MAX);
    std::string user(buf);
    return user;
}

std::string update_prompt( void ){
    // TODO make this more modular by config
    std::string directory = GetCurrentWorkingDir();
    std::string date = GetDate();
    std::string username = GetUser();
    std::string prompt = username + " | " + date + " " + directory + ">";
    return prompt;
};
