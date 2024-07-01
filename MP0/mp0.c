#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "errno.h"
#define MAX_DEPTH 20
// modified from user/count.c
int dir_number;
int file_number;
int cal_occur(char *path, char key){
  int cnt = 0;
  for (int i = 0; i < strlen(path); i++) {
      if (path[i] == key) cnt++;
  }
  return cnt;
}
void traverse(char *path, char *key) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
   
    if ((fd = open(path, 0)) < 0) {
        printf("%s [error opening dir]\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf("%s [error opening dir]\n", path);
        return;
    }
    /*int cnt = 0, n;
        while ((n = read(fd, buf, sizeof(buf))) > 0){
            for (int i = 0; i < n; i++){
                if (buf[i] == *key) cnt++;
            }
        }
        fprintf(2, "%s %d\n", path, cnt);
    if (st.type == T_FILE) {
        file_num++;
    }
    else if (st.type == T_DIR){
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
         while (read(fd, &de, sizeof(de)) == sizeof(de)){
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            traverse(buf, key);
         }
         dir_num++;
    }*/

    //到最底
    if (st.type != T_DIR) return;

    int cnt = cal_occur(path, *key);
    printf("%s %d\n", path, cnt);
    
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';
        //get file info
        struct stat tmp;
        stat(buf, &tmp);
        //traverse now dir
        if (tmp.type == T_FILE){
            file_number++;
            printf("%s %d\n", buf, cal_occur(buf, *key));
        }
        else if (tmp.type == T_DIR){
            dir_number++;
            traverse(buf, key);
        }
    }
    close(fd);
    return;
}

int main(int argc, char *argv[]) {
    int pid, ret = 0;
    int fds[2];
    if (argc < 2) {
        printf("tree: missing argv[1]\n");
        exit(-1);
    }

    // Create pipes
    if (pipe(fds) < 0) {
        printf("tree: pipe failed\n");
        exit(-1);
    }

    // Create child process
    pid = fork();
    if (pid == 0) { // Child
        traverse(argv[1], argv[2]);
        write(fds[1], &file_number, sizeof(int));
        write(fds[1], &dir_number, sizeof(int));
        exit(0);
    } else if (pid > 0) { // Parent
        // wait for child to exit
        // wait(0);
        int file_num_now, dir_num_now;
        read(fds[0], &file_num_now, sizeof(int));
        read(fds[0], &dir_num_now, sizeof(int));
        printf("\n%d directories, %d files\n", dir_num_now, file_num_now);
    }

    close(fds[0]);
    close(fds[1]);
    exit(ret);
}



