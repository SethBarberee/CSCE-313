std::string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  getcwd( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
};

std::string update_prompt( void ){
    std::string directory = GetCurrentWorkingDir();
    std::string prompt = directory + ">";
    return prompt;
};
