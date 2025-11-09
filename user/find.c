#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// 从 path 中提取文件名
const char *parse_filename(const char *path) {
  static char buf[DIRSIZ + 1];
  const char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

void find(const char *path, const char *filename) {
  if (path == 0 || filename == 0) {
    fprintf(2, "find: invalid arguments\n");
    return;
  }

  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    const char *base_name = parse_filename(path);
    // printf("Checking file: '%s' (base name: '%s')\n", path, base_name);
    if (strcmp(base_name, filename) == 0) {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    if (strlen(path) + 1 /* char '/' */ + DIRSIZ + 1 /* char '/' */
        > sizeof buf) {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0) {
        continue;
      }

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, filename);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: find <path> <filename>\n");
    exit(0);
  }
  const char *path = argv[1];
  const char *filename = argv[2];
  find(path, filename);
  exit(0);
}
