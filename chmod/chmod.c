#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  char fn[]="temp.file";
  int file_descriptor;
  struct stat info;

  if ((file_descriptor = creat(fn, S_IWUSR)) == -1)
    perror("creat() error");
  else {
    if (stat(fn, &info)!= 0)
       perror("stat() error");
    else {
      printf("original permissions were: %08o\n", info.st_mode);
    }
    if (chmod(fn, S_IRWXU|S_IRWXG|S_IRWXO) != 0)
      perror("chmod() error");
    else {
      if (stat(fn, &info)!= 0)
         perror("stat() error");
      else { 
         printf("after chmod(), permissions are: %08o\n", info.st_mode);
      }
    }
    if (close(file_descriptor)!= 0)
       perror("close() error");
    if (unlink(fn)!= 0)
       perror("unlink() error");
  }
}
