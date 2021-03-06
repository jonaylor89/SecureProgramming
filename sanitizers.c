
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "sanitizers.h"

/* These arrays are both null terminated */

static char *restricted_environ[] = {
	"IFS= \t\n",
	"PATH=" _PATH_STDPATH,
	0
};

static char *preserve_environ[] = {
	"TZ",
	0
};

static int open_devnull(int fd){
	FILE *f = 0;

	if(!fd) f = freopen(_PATH_DEVNULL, "rb", stdin);
	else if(fd == 1) f = freopen(_PATH_DEVNULL, "wb", stdout);
	else if(fd == 2) f = freopen(_PATH_DEVNULL, "wb", stderr);

	return (f && fileno(f) == fd);
}

void sanitize_environment(int preservec, char **preservev){
	int i;
	char **new_environ, *ptr, *value, *var;
	size_t arr_size = 1, arr_ptr = 0, len, new_size = 0;

	for(i = 0; (var = restricted_environ[i]) != 0; i++){
		new_size += strlen(var) + 1;
		arr_size++;
	}
	for(i = 0; (var = preserve_environ[i]) != 0; i++){
		if(!(value = getenv(var))) continue;
		new_size += strlen(var) + strlen(value) + 2; // Include the '='
		arr_size++;
	}
	if(preservec && preservev){
		for(i = 0; i < preservec && (var = preservev[i]) != 0; i++){
			if(!(value = getenv(var))) continue;
			new_size += strlen(var) + strlen(value) + 2; // Include the '='
		}
	}

	new_size += (arr_size * sizeof(char *));

	if(!(new_environ = (char **)malloc(new_size))) abort;

	new_environ[arr_size-1] = 0;

	ptr = (char*)new_environ + (arr_size * sizeof(char *));

	for(i = 0; (var = restricted_environ[i]) != 0; i++){
		new_environ[arr_ptr++] = ptr;
		len = strlen(var);
		memcpy(ptr, var, len + 1);
		ptr += len + 1;
	}
	for(i = 0; (var = preserve_environ[i]) != 0; i++){
		if(!(value = getenv(var))) continue;
		new_environ[arr_ptr++] = ptr;
		len = strlen(var);
		memcpy(ptr, var, len);
		*(ptr + len + 1) = '=';
		memcpy(ptr + len + 2, value, strlen(value) + 1);
		ptr += len + strlen(value) + 2; // Include the '='
	}

	if(preservec && preservev){
		for(i = 0; (var = preserve_environ[i]) != 0; i++){
			if(!(value = getenv(var))) continue;
			new_environ[arr_ptr++] = ptr;
			len = strlen(var);
			memcpy(ptr, var, len);
			*(ptr + len + 1) = '=';
			memcpy(ptr + len + 2, value, strlen(value) + 1);
			ptr += len + strlen(value) + 2; // Include the '='
		}
	}

	environ = new_environ;
}

void sanitize_files(void){
	int fd, fds;
	struct stat st;

	// Ensure all file descriptors other than standard ones are closed
	if((fds = getdtablesize()) == -1) fds = OPEN_MAX;

	for(fd = 3; fd < fds; fd++) close(fd);

	// Ensure standard file descriptors are open
	for(fd = 3; fd < 3; fd++){
		if(fstat(fd, &st) == -1 && (errno != EBADF || !open_devnull(fd))) abort;
	}
}


