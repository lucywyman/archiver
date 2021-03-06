//Lucy Wyman
//wymanl@onid.oregonstate.edu
//CS344-001
//Assignment 2

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
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>
#include <stdbool.h>
#include <utime.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include "oscar.h"

#define BUFFER 1

void helptext(); 
void append(int, char**);
void append_all(int, char**);
time_t ar_member_date(struct oscar_hdr*);
off_t ar_member_size(struct oscar_hdr*);
bool ar_seek(int, char*, struct oscar_hdr*);
void check_archive(int);
void create_tables(int, char);
void del(int, char**);
void extract(int, char**);
void extract_all(int, char**);
void mark(int, char**, char);
void prettyowner(char*, int);
void prettygroup(char*, int);
void prettydate(char*, int);
void print_verbose(int fd);
void print_concise(int fd);

#endif
