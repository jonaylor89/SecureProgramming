#ifndef _SanitizeEnviron_h
#define _SanitizeEnviron_h 

extern char **environ;

void sanitize_environment(int preservec, char **preservev);

#endif
