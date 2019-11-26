#define _POSIX_SOURCE
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>
#include <string.h>

int main() {
  char fn[]="creat.file", text[]="This is a test";
  int fd;

  if ((fd = creat(fn, S_IRUSR | S_IWUSR)) < 0)
    perror("creat() error");
  else {
    printf("test");	  
    write(fd, text, strlen(text));
    close(fd);
    //unlink(fn);
  }
return(fd);
}
