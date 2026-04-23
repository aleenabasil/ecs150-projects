//wish--ALeena
//build shell: read input(getline()), parse, reapeat until done
//interactive mode and batch mode(one file)
//fork(), execv(), dup2()
//handle whitespace, split arguemnts, & and .
//implement exit, cd, path
//have path list(/bin) search to find exectuables
//child process and call execv()
//use access
//output rediretion (>)--> overwrite/create file --> stdout and stderr
//parallel commands (&) --> start all process, wait for all to finish
//handle errors --> prin error message


#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <vector>


using namespace std;

//error message
void printError(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}


int main(int argc, char* argv[]) {
    //shell path
    vector<string> paths;
    paths.push_back("/bin");

    //check # of arguemnts
    if(argc > 2){
        printError();
        exit(1);
    }

    //input will be either interactive or batch
    istream* input = &cin;
    ifstream batchFile;

    //one arguement == batch
    if(argc == 2){
        batchFile.open(argv[1]);

        if(!batchFile.is_open()){
            printError();
            exit(1);
        }

        input = &batchFile;
    }

    string line;

    //reads line, parse, runs commands, and continues
    while(true){
        if(!getline(*input, line)){
            break;
        }

        //remove whitespace
        size_t start = line.find_first_not_of(" \t\n\r");

        if(start == string::npos){
            continue;
        }


        size_t end = line.find_last_not_of(" \t\n\r");
        line = line.substr(start, end - start +1);

        if(line.empty()){
            continue;
        }

        //add space to heko parse commands
        string result;
        for(char c : line){
            if(c == '>' || c == '&'){
                result += ' ';
                result += c;
                result += ' ';
            }
            else{
                result += c;
            }
        }

        //seperate parallel commands
        //stringstream --> https://www.systutorials.com/docs/linux/man/3-std::stringstream/
        vector<string> comm;
        string current;
        string read;
        stringstream ss(result);

        while(ss >> read){
            if(read == "&"){
                if(!current.empty()){
                    comm.push_back(current);
                    current.clear();
                }
            }
            else{
                if(!current.empty()){
                   current += " ";
                }
                current += read;
            }
        }

        if(!current.empty()){
            comm.push_back(current);
        }


        //children process id
        vector<pid_t> children;

        //each command from the line
        for(const string& opp : comm){
            vector<string> args;
            bool redirect = false;
            string outFile;

            vector<string> parts;
            string part;
            stringstream cs(opp);

            while(cs >> part){
                parts.push_back(part);
            }

            if(parts.empty()){
                continue;
            }

            //redirection, if there is more than one it is an error
            int index = -1;
            for(int i = 0; i < (int)parts.size(); i++){
                if(parts[i] == ">"){
                    if(index != -1){
                        index = -2;
                        break;
                    }
                    index = i;
                }
            }


            //no muliple operators
            if(index == -2){
                printError();
                break;
            }

            //redirection fromat
            if(index != -1){
                if(index == 0 || index != (int)parts.size() -2){
                    printError();
                    break;
                }
                redirect = true;
                outFile = parts.back();

                args.assign(parts.begin(), parts.begin() + index);

            }
            else{
                args = parts;
            }

            if(args.empty()){
                printError();
                break;
            }

            //exit
            if(args[0] == "exit"){
                if(redirect || args.size() != 1){
                    printError();
                }
                else{
                    for(pid_t pid : children){
                        waitpid(pid, nullptr, 0);
                    }
                    exit(0);
                }
            }
            //cd
            else if(args[0] == "cd"){
                if(redirect || args.size() != 2 || chdir(args[1].c_str()) != 0){
                    printError();
                }
            }
            //path
            else if(args[0] == "path"){
                if(redirect){
                    printError();
                }
                else{
                    paths.assign(args.begin() + 1, args.end());
                }
            }
            else{
                if(paths.empty()){
                    printError();
                    continue;
                }
                //create new child
                pid_t pid = fork();

                if(pid < 0){
                    printError();
                    continue;
                }

                //child process
                if(pid == 0){
                    if(redirect){
                        //open,create,turnacate
                        //linux --> https://man7.org/linux/man-pages/man2/open.2.html
                        int fd = open(outFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

                        if(fd < 0){
                            printError();
                            exit(1);
                        }

                        //stdout and stderr to file
                        if(dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0){
                            printError();
                            close(fd);
                            exit(1);
                        }
                        close(fd);
                    }
                    vector<char*> exec;
                    for(const string& arg : args){
                        exec.push_back(const_cast<char*>(arg.c_str()));

                    }

                    exec.push_back(nullptr);
                    for(const string& dir : paths){
                        string fullPath = dir + "/" + args[0];
                        //linux--> https://man7.org/linux/man-pages/man2/access.2.html
                        if(access(fullPath.c_str(), X_OK) == 0){
                            execv(fullPath.c_str(), exec.data());
                        }
                    }

                    printError();
                    exit(1);
                }
                //parnet process
                else{
                    children.push_back(pid);
                }
            }

        }

        //after all commands, wait for children to finish
        for(pid_t pid: children){
            waitpid(pid, nullptr, 0);
        }

    }




    return 0;
}



