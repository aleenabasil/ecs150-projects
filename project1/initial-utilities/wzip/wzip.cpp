//Wzip -- Aleena Basil
#include<iostream>
#include<fcntl.h>
#include<unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    const int buff_num = 4096;
    char buff[buff_num];
    int file_desc;
    char curr = '\0';
    int count = 0;

    ///check if files were provided
    if(argc < 2){
        cout << "wzip: file1 [file2 ...]" << endl;
        return 1;
    }

    //go through each file
    for(int i = 1; i <argc; i++){
        file_desc = open(argv[i], O_RDONLY);
        //if file cannot be opened
        if(file_desc < 0){
            cout << "wzip: cannot open file" << endl;
            return 1;
        }

        ssize_t res;

        //read file into buffer until the end of the file
        while((res = read(file_desc, buff, buff_num)) > 0){
            //go through each charracter
            for(int j = 0; j < res; j++){
                //same character, increase count
                if(curr == buff[j]){
                    count++;
                }
                else{
                    //write prev
                    if(count > 0){
                        write(STDOUT_FILENO, &count, sizeof(count));
                        write(STDOUT_FILENO, &curr, sizeof(curr));
                    }
                    curr = buff[j];
                    count = 1;
                }
            }
        }
        //read error
        if(res < 0){
            return 1;
        }

        if(close(file_desc) < 0){
            return 1;
        }

    }
    //last char count
    if(count > 0){
        write(STDOUT_FILENO, &count, sizeof(count));
        write(STDOUT_FILENO, &curr, sizeof(curr));
    }
    return 0;
}
