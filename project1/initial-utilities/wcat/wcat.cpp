//Wwcat -- Aleena Basil
#include<iostream>
#include<fcntl.h>
#include<unistd.h>

using namespace std;

int main(int argc, char *argv[]){
    //ssize_t read(size_t count; int fd, void buf[count], size_t count);
    //linux manual
    const int buff_num = 4096;
    char buff[buff_num];


    //check if there are files
    if (argc < 2){
        return 0;
    }

    //loop through each file
    for(int i = 1; i < argc; i++){
        //read mode
        int file_desc = open(argv[i], O_RDONLY);
        //check if it failed to open
        if(file_desc < 0){
            cout << "wcat: cannot open file" << endl;
            return 1;
        }
        
        //reaf each file in chunks until reach the end of the file
        ssize_t readF;
         while ((readF = read(file_desc, buff, buff_num)) > 0) {
            //standard output
            ssize_t writeF = write(STDOUT_FILENO, buff, readF);
            // check if writing failed
            if (writeF < 0) {
                close(file_desc);
                return 1;
            }
        }
        //check if read failed
        if(readF < 0){
            close(file_desc);
            return 1; 
        }
        //file close
        if(close(file_desc) < 0){
            return 1; 
        }

    }
    return 0;
}
