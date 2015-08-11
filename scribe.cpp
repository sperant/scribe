 /*
 * Scribe -- Personal Assistant
 * Usage:
 * scribe add My Todo Note
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <map>

class ScribeFile
{
  std::string file_loc, arc_loc;
  public:
    ScribeFile(std::map <std::string, std::string> config) {
      file_loc = config["SCRIBEFILE"];
      arc_loc = config["ARCHIVEFILE"];
    }

    void add(std::string todo_text, std::string myfile_loc, std::string extra = "") {
      int n = current_n(myfile_loc) + 1;

      std::ofstream myfile;
      myfile.open(myfile_loc, std::ios::app);
      myfile << std::to_string(n) << ": " << todo_text + extra << std::endl;
      myfile.close();
    }
    void add(std::string todo_text) {
      add(todo_text, file_loc);
    }

    void ls(std::string myfile_loc) {
      std::string line;
      std::ifstream file;

      file.open(myfile_loc);
      while (std::getline(file, line)) {
        std::cout << line << std::endl;
      }
      file.close();
    }
    void ls() { ls(file_loc); }
    void ls (std::string myfile_loc, std::string tag) {
      std::string line;
      std::ifstream file;
      file.open(myfile_loc);
      while(std::getline(file, line)) {
        std::size_t found = line.find(tag);
        if (found != std::string::npos)
          std::cout << line << std::endl;
      }
    }

    std::string rm(int index) { return rm(index, file_loc); }
    std::string rm(int index, std::string myfile_loc) {
      std::string line, todo_text;
      int n, cn;
      int t = 0;
      cn = current_n(myfile_loc);

      std::string *entries = new std::string[cn+1];

      std::fstream file;
      file.open(myfile_loc, std::ios::in);
      while (std::getline(file, line)) {
        n = find_n(line);
        std::size_t found = line.find(":");
        if (found != std::string::npos) {
          entries[n] = line.substr(found+2);
        }
      }
      file.close(); 

      file.open(myfile_loc, std::ios::out);
      for(int i = 0; i <= cn; i++) {
        if (i == index) {
          todo_text = entries[i];
          continue;
        }
        
        file << t << ": " + entries[i] << std::endl;
        t++;
      }

      file.close();

      return todo_text;
    }

    void done(int index) {
      std::string todo_text = rm(index);
      std::ostringstream oss;
      oss << std::time(nullptr);
      add(todo_text, arc_loc, "|" + oss.str());
    }
    void done(std::string tag) {
      std::ifstream file;
      std::string line;
      int i = 0;
      file.open(file_loc);
      while(std::getline(file, line)) {
        std::size_t found = line.find(tag);
        if (found != std::string::npos) {
          done(find_n(line) - i);
          i++;
        }
      }
      file.close();
    }

    void clear(int setting = 0) {
      int cn = current_n();
      for (int i = 0; i <= cn; i++) {
        if (setting == 0) done(0);
        if (setting == 1) rm(0);
      }
    }

    void undo(int index) {
      std::string todo_text = rm(index, arc_loc);
      std::size_t found = todo_text.find_last_of("|");
      add(todo_text.substr(0,found));
    }

    int current_n(std::string myfile_loc) {
      std::string line, last_line;
      std::ifstream file;

      file.open(myfile_loc, std::ios::in);
      while (std::getline(file, line)) {
        last_line = line;
      }
      file.close();

      return find_n(last_line);
    }
    int current_n() { return current_n(file_loc); }

    int find_n(std::string line) {
      std::size_t found = line.find(":");
      if (found != std::string::npos)
        return atoi(line.substr(0,found).c_str());
      
      return -1;
    }
};

std::map<std::string, std::string> get_config() {
  // Read Config File
  // TODO: Trim whitespace
  std::map <std::string, std::string> config;

  char scribe_dir[255];
  strcpy(scribe_dir,std::getenv("HOME"));
  strcat(scribe_dir,"/.scribe");

  std::string line;
  if (FILE *sdfile = fopen(scribe_dir, "r")) {
    fclose(sdfile);

    std::ifstream config_file;
    config_file.open(scribe_dir); 
    while (std::getline(config_file, line)) {
      std::size_t found = line.find("=");
      if (found != std::string::npos) {
        config[line.substr(0,found)] = line.substr(found+1);
      }
    }

    config_file.close();
  } else {
    // Generate Scribe Config File and populate config if not already created.
    std::ofstream config_file;
    config_file.open(scribe_dir);
    config_file << "SCRIBEFILE=" << std::getenv("HOME") << "/scribe.txt" << std::endl;
    config_file << "ARCHIVEFILE=" << std::getenv("HOME") << "/scribe.arc.txt" <<std::endl; 
    config_file.close();
    std::cout << "Generated your scribe configuration file for you at ~/.scribe" << std::endl;
  }

  return config;
}

void usage_error() {
  std::cout << "Usage Error. For help, type: scribe help" << std::endl;
}

void command(std::string command, std::string flag, std::string parameter) {
  std::map<std::string, std::string> config = get_config();
  ScribeFile sf (config);

  if (command.empty()) {
    std::cout << "Scribe -- Your lovable personal assistant. v0.1.0" << std::endl;
  }
  else if (command.compare("help") == 0) {
    std::cout << "Scribe -- Your lovable personal assistant.\n" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\tscribe add TODO\t\t - Creates TODO as a new todo item." << std::endl;
    std::cout << "\tscribe ls [--arc]\t - Lists all todos." << std::endl;
    std::cout << "\tscribe ls @TAG [--arc]\t - Lists all todos with TAG." << std::endl;
    std::cout << "\tscribe rm [--arc] ID\t\t - Permanently deletes todo with ID." << std::endl;
    std::cout << "\tscribe done ID\t\t - Marks as completed todo with ID and archives." << std::endl;
    std::cout << "\tscribe done @TAG\t - Marks as completed all todos with TAG and archives." << std::endl;
    std::cout << "\tscribe clear [--rm]\t - Marks as completed all todos." << std::endl;
    std::cout << "\tscribe undo ID\t - Moves todo with ID in archive file to todo file." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "\t--arc\t - Perform on Archive File." << std::endl;
    std::cout << "\t--rm\t - Permanently delete, skip archiving." << std::endl;
    std::cout << "\nNeed more help? Want to Contribute? Go to: http://github.com/sperant/scribe" << std::endl;
  }
  else if (command.compare("add") == 0) {
    if (parameter.empty()) {
      usage_error();
      return;
    }
    sf.add(parameter);
  }
  else if (command.compare("ls") == 0) {
    if (parameter.empty() && flag.empty()) {
      sf.ls(config["SCRIBEFILE"]);
    }
    else if (parameter.empty() && flag.compare("arc") == 0) {
      sf.ls(config["ARCHIVEFILE"]);
    }
    else if (parameter.substr(0,1).compare("@") == 0) {
      if (flag.compare("arc") == 0) {
        sf.ls(config["ARCHIVEFILE"], parameter);
      } else {
        sf.ls(config["SCRIBEFILE"], parameter);
      }
    }
    else {
      usage_error();
    }
  }
  else if (command.compare("rm") == 0) {
    if (parameter.empty()) {
      usage_error();
      return;
    }
    else if (flag.empty()) {
      sf.rm(atoi(parameter.c_str()));
    }
    else if (flag.compare("arc") == 0) {
      sf.rm(atoi(parameter.c_str()), config["ARCHIVEFILE"]);
    }
    else {
      usage_error();
    }
  }
  else if (command.compare("done") == 0) {
    if (parameter.empty()) {
      usage_error();
      return;
    }

    int index;
    std::istringstream iss(parameter);
    iss >> index;
    if (iss.eof() == false) {
      sf.done(parameter);
    }
    else {
      sf.done(atoi(parameter.c_str()));
    }
  }
  else if (command.compare("clear") == 0) {
    if (flag.empty()) {
      sf.clear(0);
    }
    else if (flag.compare("rm") == 0) {
      sf.clear(1);
    }
    else {
      usage_error();
    }
  }
  else if (command.compare("undo") == 0) {
    if (parameter.empty()) {
      usage_error();
      return;
    }

    sf.undo(atoi(parameter.c_str()));
  }
  else {
    usage_error();
  }
}

int main(int argc, const char * argv[]) {
  // Parse and Deliver Commands
  
  std::string arg;
  std::string mycommand, flag, parameter;
  bool command_found = false;
  // .../ add 2 
  for (int i = 1; i < argc; i++) {
    arg = argv[i];
    std::size_t found = arg.find("--");
    if (found != std::string::npos) {
      flag = arg.substr(found+2);
      continue;
    }
    if (!command_found) {
      mycommand = arg;
      command_found = true;
    } else {
      if (parameter.empty()) {
        parameter = arg;
      } else {
        parameter = parameter + " " + arg;
      }
    }
  }

  command(mycommand, flag, parameter);
  return 0;
}