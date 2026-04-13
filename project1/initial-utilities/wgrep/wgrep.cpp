//Wgrep -- ALeena Basil
#include<iostream>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>


using namespace std;

//helper to read file descriptor and print matching lines
void wgrep_helper(int file_desc, char *argv[]){
    ssize_t res = 0;
    char temp_buff[1];
    string tempRead = "";
    const char *search = argv[1];

    //read the file one character at a time
    while((res = read(file_desc, temp_buff, 1)) > 0){
        //add curr characyer to the string 
        tempRead = tempRead + temp_buff[0];
        if(temp_buff[0] == '\n'){
            if(strstr(tempRead.c_str(), search)){
                //if match found, then print entire line
                write(STDOUT_FILENO, tempRead.c_str(), tempRead.length());
            }
            tempRead = "";
        }
        
    }
}

int main(int argc, char *argv[]){
    int file_desc = 0;

    //check if there is no file to search
    if(argc < 2){
        cout << "wgrep: searchterm [file ...]" << endl;
        return 1;
    }

    //check if only one search file os provided, use the standard input in helper
    if(argc == 2){
        wgrep_helper(STDIN_FILENO, argv);
        return 0;
    }

    //go through each file
    for(int i = 2; i < argc; i++){
        file_desc = open(argv[i], O_RDONLY);
        //check fi the file failed to open
        if(file_desc < 0){
            cout << "wgrep: cannot open file" << endl;
            return 1; 
        }
        wgrep_helper(file_desc, argv);
        close(file_desc);
    }
    return 0;
}
