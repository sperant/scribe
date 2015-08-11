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
    ScribeFile(std::string scribe_file) {
      file_loc = scribe_file;
      std::size_t found = file_loc.find_last_of(".txt");
      if (found != std::string::npos)
        arc_loc = file_loc.substr(0,found-3) + ".arc.txt";
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

    void ls(int i) {
      if (i == 0) ls(file_loc);
      if (i == 1) ls(arc_loc);
    }

    void ls (int i, std::string tag) {
      if (i == 2) {
        std::string line;
        std::ifstream file;
        file.open(file_loc);
        while(std::getline(file, line)) {
          std::size_t found = line.find("@");
          if (found != std::string::npos)
            std::cout << line << std::endl;
        }
      }
    }

    std::string rm(int index) {
      std::string line, todo_text;
      int n, cn;
      int t = 0;
      cn = current_n();

      std::string *entries = new std::string[cn+1];

      std::fstream file;
      file.open(file_loc, std::ios::in);
      while (std::getline(file, line)) {
        n = find_n(line);
        std::size_t found = line.find(":");
        if (found != std::string::npos)
          entries[n] = line.substr(found+2);
      }
      file.close();

      file.open(file_loc, std::ios::out);
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


int main(int argc, const char * argv[]) {
  // Read Scribe Config
  // todo: handle extra whitespace
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
    config_file << "SCRIBEFILE=" << std::getenv("HOME") << "/scribe.txt";
    config_file.close();
    std::cout << "Generated your scribefile for you at ~/.scribe" << std::endl;
  }
      

  if (argc <= 1) {
    std::cout << "Scribe -- Your lovable personal assistant." << std::endl;
    return 0;
  }

  std::string arg = argv[1];

  ScribeFile sf (config["SCRIBEFILE"]);

  if (arg.compare("add") == 0) {
    if (argc <= 2) {
      std::cout << "Usage is incorrect here. More arguments needed." << std::endl;
      return 0;
    }

    std::string todo_text = argv[2];

    if (argc > 3 )
      for (int i = 3; i < argc; i++)
        todo_text = todo_text + " " + argv[i];

    sf.add(todo_text);
  }
  else if (arg.compare("ls") == 0) {
    if (argc <= 2) {
      sf.ls(0);
      return 0;
    }

    std::string arg2 = argv[2];
    if (arg2.compare("--arc") == 0) {
      sf.ls(1);
    }
    else if (arg2.substr(0,1).compare("@") == 0) {
      sf.ls(2, arg2.substr(1));
    }
    else {
      std::cout << "undefined" << std::endl;
    }
  }
  else if (arg.compare("rm") == 0) {
    if (argc <= 2) {
      std::cout << "Usage incorrect. Not enough arguments." << std::endl;
      return 0;
    }

    sf.rm(atoi(argv[2]));
  }
  else if (arg.compare("done") == 0) {
    if (argc <= 2) {
      std::cout << "Usage incorrect. Not enough arguments." << std::endl;
      return 0;
    }

    int index;
    std::istringstream iss(argv[2]);
    iss >> index;
    if (iss.eof() == false) {
      sf.done(argv[2]);
    }
    else {
      sf.done(atoi(argv[2]));
    }
  }
  else if (arg.compare("clear") == 0) {
    if (argc <= 2) {
      sf.clear();
      return 0;
    }

    std::string arg2 = argv[2];
    if (arg2.compare("--rm")) {
      sf.clear(1);
    }
    else {
      std::cout << "undefined" << std::endl;
    }
  }
  else if (arg.compare("undo") == 0) {

  }
  else {
    std::cout << "Unknown command. Try: scribe help" << std::endl;
    return 0;
  }

  return 0;
}