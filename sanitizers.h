#ifndef _sanitizers_h
#define _sanitizers_h 

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

extern char **environ;

void sanitize_environment(int preservec, char **preservev);
void sanitize_files(void);

#endif
