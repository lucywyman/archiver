#include "myoscar.h"
int main (int argc, char **argv){
    int option;
    int options[14];
    memset(options, 0, sizeof(options));
    while((option = getopt(argc, argv, "aACdeEhmotTuvV")) != -1){
        //Is a switch statement really the best way of doing this???
        switch(option){
            case 'a' : 
                if(argc<=3){
                   printf("Not enough arguments!\n");
                   helptext();
                }
                append(argc, argv);
                break;
            case 'A' : 
                append_all(argc, argv);
                break;
            case 'C' : printf("C was selected\n");
                       break;
            case 'd' : printf("d was selected\n");
                       break;
            case 'e' : 
                       extract(argc, argv);
                       break;
            case 'E' : printf("E was selected\n");
                       break;
            case 'h' : 
                       helptext();
                       exit(0);
            case 'm' : printf("E was selected\n");
                       break;
            case 'o' : printf("E was selected\n");
                       break;
            case 't' : printf("E was selected\n");
                       break;
            case 'T' : printf("E was selected\n");
                       break;
            case 'u' : printf("E was selected\n");
                       break;
            case 'v' : printf("E was selected\n");
                       break;
            case 'V' : 
                printf("Version 1.0");
                exit(0);
            default : exit(1);
        }
    }
    return 0;
}

void helptext(){
    printf("To make go: myoscar [options] [archive file] [member[...]]\n"
            "-a : Add specified member files from command line to archive file\n"
            "-A : Add all regular files to the archive\n"
            "-C : All files marked for deletion are deleted from the archive\n"
            "-d : Delete member files from the archive\n"
            "-e : Extract member files from the archive. If no member files specified, all files will be extracted.\n"
            "-E : Same as -e, but keeping current time\n"
            "-h : Show help text\n"
            "-m : Mark members as deleted\n"
            "-o : Overwrite existing files on extract\n"
            "-t : Short table of contents\n"
            "-T : Long table of contents\n"
            "-u : Unmark members as deleted\n"
            "-v : Verbose processing\n"
            "-V : Print version information\n");
    exit(0);
}

void append(int argc, char* argv[]){
    struct stat st;
    struct oscar_hdr header;
    char oscar_header[] = OSCAR_ID;
    int r, archive_fd, file_fd, open_flags;
    char buf[BUFFER];
    char temp_name[OSCAR_MAX_FILE_NAME_LEN+1]; /* Member file name, may not be NULL terminated    */
    char temp_name_len[2+1];                 /* The length of the member file name */
    char temp_cdate[OSCAR_DATE_SIZE+1];      /* File create date, decimal seconds since Epoch.  */
    /* We can store the file create time, but not set it. */
    char temp_adate[OSCAR_DATE_SIZE+1];      /* File modify date, decimal seconds since Epoch.  */
    char temp_mdate[OSCAR_DATE_SIZE+1];      /* File access date, decimal seconds since Epoch.  */
    char temp_uid[OSCAR_UGID_SIZE+1];        /* user id in ASCII decimal */
    char temp_gid[OSCAR_UGID_SIZE+1];        /* group id in ASCII decimal.  */
    char temp_mode[OSCAR_MODE_SIZE+1];       /* File mode, in ASCII octal.  */
    char temp_size[OSCAR_FILE_SIZE+1];       /* File size, in ASCII decimal.  */    
    char temp_hdr_end[OSCAR_HDR_END_LEN+1];  /* Always contains OSCAR_HDR_END.  */

    r = 0;
    open_flags = O_CREAT | O_RDWR;    
    archive_fd = open(argv[2], open_flags, 0644);

    //Can archive file be opened?
    if(archive_fd == -1){
        printf("myoscar: Could not open archive file");
        exit(1);
    }

    //Can it be stat-ed?
    if(fstat(archive_fd, &st) == -1){
        printf("myoscar: Could not stat archive file");
        exit(1);
    }
    //If the archive already exists
    if((long long) st.st_size > 0){
        check_archive(archive_fd);
    } else {
        write(archive_fd, oscar_header, OSCAR_ID_LEN);
    }

    //Loop through list of files to make sure they are valid
    for(int i=3; i<argc; i++){
        //Does member file exist?  Is it accessible?
        if(access(argv[i], F_OK) == -1) {
            printf("myoscar: %s: No such file or directory", argv[i+3]);
            exit(1);
        }
        //Is it a named file or sym link?
        if(stat(argv[i], &st) == -1){
            printf("myoscar: Can't stat file");
            exit(1);
        }
    }

    //Always append to end of archive file
    lseek(archive_fd, 0, SEEK_END);

    //Append files!  This is where the magic happens
    for(int i=3; i<argc; i++){
        file_fd = open(argv[i], O_RDONLY);
        if(file_fd == -1){
            printf("myoscar: Can't open file");
            exit(1);
        }       
        stat(argv[i], &st);

        snprintf(temp_name, OSCAR_MAX_FILE_NAME_LEN+1, "%-32s", argv[i]);
        snprintf(temp_name_len, 3, "%-2u", (int)strlen(argv[i])); 
        snprintf(temp_cdate, OSCAR_DATE_SIZE+1, "%-12u", (unsigned int) st.st_ctime);
        snprintf(temp_adate, OSCAR_DATE_SIZE +1,"%-12u", (unsigned int) st.st_atime);
        snprintf(temp_mdate, OSCAR_DATE_SIZE+1, "%-12u", (unsigned int) st.st_mtime);
        snprintf(temp_uid, OSCAR_UGID_SIZE+1, "%-8u",  (unsigned int) st.st_uid);
        snprintf(temp_gid, OSCAR_UGID_SIZE+1, "%-8u",  (unsigned int) st.st_gid);
        snprintf(temp_mode, OSCAR_MODE_SIZE+1, "%-8o",  (unsigned int) st.st_mode);
        snprintf(temp_size, OSCAR_FILE_SIZE+1, "%-16u",  (unsigned int) st.st_size );
        snprintf(temp_hdr_end, OSCAR_HDR_END_LEN+1, "%-2s", OSCAR_HDR_END);

        // Fill the header
        memcpy(&header.oscar_name, temp_name, OSCAR_MAX_FILE_NAME_LEN);
        memcpy(&header.oscar_name_len, temp_name_len, 2);
        memcpy(&header.oscar_cdate, temp_cdate, OSCAR_DATE_SIZE);
        memcpy(&header.oscar_adate, temp_adate, OSCAR_DATE_SIZE);
        memcpy(&header.oscar_mdate, temp_mdate, OSCAR_DATE_SIZE);
        memcpy(&header.oscar_uid, temp_uid, OSCAR_UGID_SIZE);
        memcpy(&header.oscar_gid, temp_gid, OSCAR_UGID_SIZE);
        memcpy(&header.oscar_mode, temp_mode, OSCAR_MODE_SIZE);
        memcpy(&header.oscar_size, temp_size, OSCAR_FILE_SIZE);
        memset(&header.oscar_deleted, ' ', 1);
        memset(&header.oscar_sha1, ' ', OSCAR_SHA_DIGEST_LEN);
        memcpy(&header.oscar_hdr_end, temp_hdr_end, OSCAR_HDR_END_LEN);

        if(write(archive_fd, &header, sizeof(header)) == -1){
            printf("Error writing to archive\n");
            exit(1);
        }

        while((read(file_fd, buf, BUFFER))!=0){
            write(archive_fd, buf, BUFFER);
        }

        //Unix ar compatibility -- for funsies
        if(st.st_size % 2){
            write(archive_fd, "\n", 1);
        }
        close(file_fd);
    }
    close(archive_fd);        
}

void append_all(int argc, char* argv[]){
    DIR* directory;
    int i = 3;
    char** list = malloc(10*sizeof(char*)); 
    struct dirent *file;
    struct stat st;
    //Literally the worst hack ever
    list[0] = "a";
    list[1] = "b";
    list[2] = malloc((strlen(argv[2])+1)*sizeof(char));
    strcpy(list[2], argv[2]);

    //Make sure current directory is a thing     
    directory = opendir(".");
    if(directory == NULL){
        printf("Could not open current directory");
        exit(1);
    }

    //Iterate through file list to create list of regular files
    while((file=readdir(directory))!= NULL) {
        if((stat(file->d_name, &st))==-1){
            printf("Could not stat file");
            exit(1);
        }
        //If it's a regular file
        if(S_ISREG(st.st_mode) != 0){
            if(strcmp(file->d_name, argv[2]) != 0
                    && strcmp(file->d_name, "a.out") !=0){
                //Is this literally the worst way to do this? I wouldn't be surprised
                //Separate function
                list[i] = malloc((strlen(file->d_name)+1) * sizeof(char));
                strcpy(list[i], file->d_name);
                i++;
                argc++;
                //list = realloc(list, i*sizeof(char*));
            }
        }
    }

    append(argc, list);
    //First two "ghost elements" not malloc'd
    for(int j=2; j<argc; j++){
        free(list[j]);
    }   
    free(list);
}

void extract(int argc, char* argv[], ){
    int file_fd, archive_fd, filesize, written, open_flags, file_flags;
    struct utimbuf tbuf;
    struct oscar_hdr new_header;
    struct stat st;
    char name[OSCAR_MAX_FILE_NAME_LEN+1];
    open_flags = O_RDONLY;
    file_flags = O_CREAT | O_RDWR;
    archive_fd = open(argv[2], open_flags);

    check_archive(archive_fd);

    //Can it be stat-ed?
    if(stat(argv[2], &st) == -1){
        printf("myoscar: Could not stat archive file\n");
        exit(1);
    }

    //Check that specified files are actually in archive
    for(int i=3; i<argc; i++){
        memset(name, '\0', OSCAR_MAX_FILE_NAME_LEN+1);
        memcpy(name, argv[i], strlen(argv[i])+1);
        if(read(archive_fd, &new_header, sizeof(new_header)) == -1){
            printf("Metadata could not be read\n");
            exit(1);
        }
        if(ar_seek(archive_fd, argv[i], &new_header) == false){
            printf("Could not find %s.  Skipping.\n", argv[i]);
        } else { 
            for(int k=0; k<strlen(argv[1]); k++){
                if(
            file_fd = creat(argv[i], 0770);
        }
        if(file_fd == -1){
            printf("Could not open file for extraction\n");
            exit(1);
        }
        filesize = ar_member_size(&new_header);
        written = 0;

        while(written < filesize) {
            char buffer[BUFFER];
            size_t wr_size;
            wr_size = (filesize - written < BUFFER) ? filesize - written : BUFFER;
            if(read(archive_fd, buffer, wr_size) == -1){
                printf("Error reading file\n");
                exit(1);
            }
            if(write(file_fd, buffer, wr_size) == -1){
                printf("Error writing file\n");
                exit(1);
            }
            written += wr_size;
        }
        if(close(file_fd) == -1){
            printf("Could not close file\n");
            exit(1);
        }
        tbuf.actime = ar_member_date(&new_header);
        tbuf.modtime = ar_member_date(&new_header);
        utime(argv[i], &tbuf);
    }
}

void del(int argc, char* argv[]){
    int newarhive_fd, archive_fd, open_flags, new_flags;
    //struct utimbuf tbuf;
    //struct oscar_hdr new_header;
    struct stat st;
    //char name[OSCAR_MAX_FILE_NAME_LEN+1];
    open_flags = O_RDONLY;
    new_flags = O_CREAT | O_RDWR;
    archive_fd = open(argv[2], open_flags);
    newarchive_fd = open(

    check_archive(archive_fd);

    //Can it be stat-ed?
    if(stat(argv[2], &st) == -1){
        printf("myoscar: Could not stat archive file\n");
        exit(1);
    }

    //Check that specified files are actually in archive
    for(int i=3; i<argc; i++){
        memset(name, '\0', OSCAR_MAX_FILE_NAME_LEN+1);
        memcpy(name, argv[i], strlen(argv[i])+1);
        if(read(archive_fd, &new_header, sizeof(new_header)) == -1){
            printf("Metadata could not be read\n");
            exit(1);
        }
        if(ar_seek(archive_fd, argv[i], &new_header) == false){
            printf("Could not find %s.  Skipping.\n", argv[i]);
        } else {
            
        }

        while(written < filesize) {
            char buffer[BUFFER];
            size_t wr_size;
            wr_size = (filesize - written < BUFFER) ? filesize - written : BUFFER;
            if(read(archive_fd, buffer, wr_size) == -1){
                printf("Error reading file\n");
                exit(1);
            }
            if(write(file_fd, buffer, wr_size) == -1){
                printf("Error writing file\n");
                exit(1);
            }
            written += wr_size;
        }
        if(close(file_fd) == -1){
            printf("Could not close file\n");
            exit(1);
        }
        tbuf.actime = ar_member_date(&new_header);
        tbuf.modtime = ar_member_date(&new_header);
        utime(argv[i], &tbuf);
    }       

}

bool ar_seek(int archive_fd, char* name, struct oscar_hdr *header){
    off_t filesize;
    struct stat st;

    if(fstat(archive_fd, &st) == -1){
        printf("Could not stat archive\n");
        exit(1);
    }    

    if(archive_fd<0 || name == NULL){
        printf("Not a valid file! Please try again.\n");
        exit(1);
    }
    int a = lseek(archive_fd, 0, SEEK_CUR);
    //For each member
    while(a < st.st_size){
        char filename[OSCAR_MAX_FILE_NAME_LEN+1];
        if(read(archive_fd, header, OSCAR_HDR_SIZE) == -1){
            printf("Error reading header data\n");
            exit(1);
        }
        memset(filename, '\0', OSCAR_MAX_FILE_NAME_LEN+1);
        memcpy(filename, header->oscar_name, strlen(name));
        //Compare file names to see if it's the right file
        if(strcmp(name, filename) == 0){
            return true;
        }
        filesize = ar_member_size(header);
        a = lseek(archive_fd, filesize, SEEK_CUR);
        if((lseek(archive_fd, 0, SEEK_CUR) %2) == 1){
            a = lseek(archive_fd, 1, SEEK_CUR);
        }
    }
    return false;
}


off_t ar_member_size(struct oscar_hdr *header) {
    char size[OSCAR_FILE_SIZE + 1];
    assert(header != NULL);
    memcpy(size, header->oscar_size, OSCAR_FILE_SIZE);
    size[OSCAR_FILE_SIZE] = '\0';
    return strtol(size, NULL, 10);
}

time_t ar_member_date(struct oscar_hdr *header) {
    char str[OSCAR_DATE_SIZE + 1];
    assert(header != NULL);
    memcpy(str, header->oscar_cdate, OSCAR_DATE_SIZE);
    str[OSCAR_DATE_SIZE] = '\0';
    return strtol(str, NULL, 10);
}

void check_archive(int archive_fd){
    char oscar_header[];
    if(read(archive_fd, oscar_header, OSCAR_ID_LEN) == -1){
        printf("Ooops! That isn't an archive file\n");
        exit(1);
    }
    if(strcmp(oscar_header, OSCAR_ID) != 0){
        printf("Not a valid archive file\n");
        exit(1);
    }
    
    //Can archive file be opened?
    if(archive_fd == -1){
        printf("myoscar: Could not open archive file\n");
        exit(1);
    }
    
}


