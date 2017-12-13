#ifndef _sanitizers_h
#define _sanitizers_h 

extern char **environ;

void sanitize_environment(int preservec, char **preservev);

#endif
