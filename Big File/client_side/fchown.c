/* CELEBF04

   This example changes the owner ID and group ID.

 */
//#define _POSIX_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#undef _POSIX_SOURCE
#include <stdio.h>

int main() {
  char fn[]="temp.file";
  FILE *stream;
  int  fd;
  struct stat info;

  if ((fd = creat(fn, S_IWUSR)) < 0)
    perror("creat() error");
  else {
    stat(fn, &info);
    printf("original owner was %d and group was %d\n", info.st_uid,
           info.st_gid);
    if (fchown(fd, 123, 456) != 0)
      perror("fchown() error");
    else {
      stat(fn, &info);
      printf("after fchown(), owner is %d and group is %d\n",
             info.st_uid, info.st_gid);
    }
    close(fd);
    unlink(fn);
  }
}
