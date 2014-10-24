#ifndef MYAR_H_
#define MYAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ar.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include "oscar.h"

#define BUFFER 4096

void helptext(); 
void append(int, char*);
void append_all(int, char*);
void check_archive(int);
void del(int*, char*, char**, int);
void extract(int fd, char* filename);
void extract_all(int fd);
void print_verbose(int fd);
void print_concise(int fd);
