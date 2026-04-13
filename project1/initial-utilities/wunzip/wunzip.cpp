//Wunzip -- Aleena Basil
#include<iostream>
#include<fcntl.h>
#include<unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    int file_desc;

    //check if files were provided
    if(argc < 2){
        cout << "wunzip: file1 [file2 ...]" << endl;
        return 1;
    }

    //go through each file
    for(int i = 1; i < argc; i++){
        file_desc = open(argv[i], O_RDONLY);
        if(file_desc < 0){
            cout << "wunzip: cannot open file" << endl;
            return 1;
        }

        int count;
        char curr;
        ssize_t res;

        //read by count-bytes repeatedly
        while((res = read(file_desc, &count, sizeof(count))) == sizeof(count)){
            if(read(file_desc, &curr, sizeof(curr)) != sizeof(curr)){
                    cout << "wunzip: error reading file" << endl;
                    close(file_desc);
                    return 1;
            }

            //write the char count times to the stdout
            for(int j = 0; j < count; j++){
                if(write(STDOUT_FILENO, &curr, sizeof(curr)) < 0){
                    close(file_desc);
                    return 1;
                }
            }

        }
        if(res < 0){
            cout << "wunzip: error reading file" << endl;
            close(file_desc);
            return 1;
        }
        
        if(close(file_desc) < 0){
            return 1;
        }
    }
    return 0;

}
