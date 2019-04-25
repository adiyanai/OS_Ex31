#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#define ARGS_NUM 3
#define PRINT_ERROR_AND_EXIT write(2, "Error in system call\n", strlen("Error in system call\n")); exit(-1);
#define CLOSE_FILES close(fd1); close(fd2);

/**
 * check if the files identical, similar or different
 * @param file1 - first file
 * @param file2 - second file
 * @return 1 if they are identical, 3 if they are similar and 2 if they are different
 */
int check_files (char *file1, char *file2) {
    int fd1, fd2, answer, flag;
    int to_read[2] = {1,1};
    char c[2] = {' ',' '};
    ssize_t read_c[2] = {0,0};
    answer = 1;
    flag = 0;

    // tests whether the files exist and whether the files can be accessed for reading
    if (access(file1, F_OK) == 0 && access(file1, R_OK) == 0) {
        // try to open file 1
        if ((fd1 = open(file1, O_RDONLY)) == -1) {
            PRINT_ERROR_AND_EXIT;
        }
    } else {
        PRINT_ERROR_AND_EXIT;
    }
    if (access(file2, F_OK) == 0 && access(file2, R_OK) == 0) {
        // try to open file 2
        if ((fd2 = open(file2, O_RDONLY)) == -1) {
            PRINT_ERROR_AND_EXIT;
        }
    } else {
        PRINT_ERROR_AND_EXIT;
    }


    while (1) {
        // file1
        if (to_read[0]) {
            do {
                // read one character from the two files
                read_c[0] = read(fd1, &c[0], 1);
                if (read_c[0] == -1) {
                    PRINT_ERROR_AND_EXIT;
                }
                // skip enters and whitespaces if the files are not identical
                if ((answer == 1) || (isspace(c[0]) == 0)) {
                    if (answer == 1) {
                        flag = 1;
                    }
                    break;
                }
            } while (read_c[0] != 0);
        }

        // file2
        if (to_read[1]) {
            do {
                // read one character from the two files
                read_c[1] = read(fd2, &c[1], 1);
                if (read_c[1] == -1) {
                    PRINT_ERROR_AND_EXIT;
                }
                // skip enters and whitespaces if the files are not identical
                if ((answer == 1) || (isspace(c[1]) == 0)) {
                    if (answer == 1) {
                        flag = 1;
                    }
                    break;
                }
            } while (read_c[1] != 0);
        }

        // if we finish to read the two files
        if ((read_c[0] == 0) && (read_c[1] == 0)) {
            CLOSE_FILES;
            return answer;
        } else if (read_c[0] == 0 || read_c[1] == 0) {
            CLOSE_FILES;
            return 2;
        }

        // the files are identical
        if ((c[0] == c[1]) && (answer == 1)) {
            continue;
            // the files are similar
        } else if ((tolower(c[0]) == tolower(c[1])) || (flag == 1)) {
            if (answer == 1) {
                answer = 3;
            }
            // if one of the chars is a space or \n
            if (flag == 1) {
                flag = 0;
                // if c[0] a letter don't read the next char
                if (isspace(c[0]) == 0) {
                    to_read[0] = 0;
                }
                // if c[1] a letter don't read the next char
                if (isspace(c[1]) == 0) {
                    to_read[1] = 0;
                }
            } else {
                to_read[0] = 1;
                to_read[1] = 1;
            }

            continue;
            // the files are different
        } else {
            CLOSE_FILES;
            return 2;
        }
    }
}

int main(int argc, char *argv[]) {
    char *path1, *path2;
    if (argc != ARGS_NUM) {
        printf("unexpected number of arguments\n");
        return -1;
    }
    path1 = argv[1];
    path2 = argv[2];

    // check the files
    return check_files(path1, path2);
}