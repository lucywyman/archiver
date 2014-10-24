#include "myoscar.h"

void helptext(){
    printf("To make go: myoscar [options] [archive file] [member[...]]"
            "-a : Add specified member files from command line to archive file"
            "-A : Add all regular files to the archive"
            "-C : All files marked for deletion are deleted from the archive"
            "-d : Delete member files from the archive"
            "-e : Extract member files from the archive. If no member files specified, all files will be extracted."
            "-E : Same as -e, but keeping current time"
            "-h : Show help text"
            "-m : Mark members as deleted"
            "-o : Overwrite existing files on extract"
            "-t : Short table of contents"
            "-T : Long table of contents"
            "-u : Unmark members as deleted"
            "-v : Verbose processing"
            "-V : Print version information");
}

void append(int argc, char* argv[]){
    struct stat st;
    struct oscar_hdr header;
    int r = 0;
    char buf[BUFFER];

    open_flags = o_CREAT | O_RDWR;    
    archive_fd = open(argv[2], open_flags, 0666);

    //Can archive file be opened?
    if(archive_fd == -1){
        printf("myoscar: Could not open archive file");
        exit(1);
    }

    //Can it be stat-ed?
    if(stat(archive_fd, &st) == -1){
        printf("myoscar: Could not stat archive file");
        exit(1);
    }

    //If the archive already exists
    if((long long) st.st_size > 0){
        check_archive(archive_fd);
    } else {
        write(archive_fd, header, OSCAR_ID);
    }

    //Loop through list of files to make sure they are valid
    for(i=0; i<argc-3; i++){
        //Does member file exist?  Is it accessible?
        if(access(argv[i+3], F_OK) == -1) {
            printf("myoscar: %s: No such file or directory", filelist[i]);
            exit(1);
        }
        //Is it a named file or sym link?
        if(stat(argv[i+3], &st) == -1){
            printf("myoscar: Can't stat file");
            exit(1);
        }
    }

    //Always append to end of archive file
    lseek(archive_fd, 0, SEEK_END);

    //Append files!  This is where the magic happens
    for(i=0; i<argc-3; i++){
        file_fd = open(argv[i+3], O_RDONLY);
        if(file_fd == -1){
            printf("myoscar: Can't open file");
            exit(1);
        }       
        //Use fwrite
        stat(argv[i+3], &st);
        sprintf(header.oscar_name, "%-16s", basename(argv[i+3]));
        //sprintf(header.oscar_cdate, "%-12u", stat.);
        sprintf(header.oscar_adate, "%-12u", (unsigned int) st.st_atime);
        sprintf(header.oscar_mdate, "%-12u", (unsigned int) st.st_mtime);
        sprintf(header.oscar_uid, "%-6u",  (unsigned int) st.st_uid);
        sprintf(header.oscar_gid, "%-6u",  (unsigned int) st.st_gid);
        sprintf(header.oscar_mode, "%-8o",  (unsigned int) st.st_mode);
        sprintf(header.oscar_size, "%-10u",  (unsigned int) st.st_size );
        sprintf(header.oscar_name, "%-16s",  basename(argv[i+3]));
        sprintf(header.oscar_hdr_end, "%s", OSCAR_HDR_END);

        write(archive_fd, (char*) &header, sizeof(header));

        while((r = read(file_fd, buf, BUFFER))>0){
            write(archive_fd, buf, r);
        }

        //Unix ar compatibility -- for funsies
        if(st.st_size % 2){
            write(archive_fd, "\n", 1);
        }
        close(file_fd);
    }
    close(archive_fd);        
}

void check_archive(int fd){
    char header[] = OSCAR_ID;
    char header_test[OSCAR_ID_LEN+1];
    if(read(fd, header_test, OSCAR_ID_LEN) == -1){
        printf("Ooops! That isn't an archive file");
        exit(1);
    }

    if(strcmp(header, header_test)!=0){
        printf("Not a valide archive!");
        exit(1);
    }
}

/*char* get_path(name){
  ar_name = (char*)malloc(sizeof(char)*(strlen(argv[optind]))+1);
  assert(ar_name != NULL);
  strcpy(ar_name, argv[optind]);

  return
  }*/

int main /*parse*/(int argc, char **argv){
    int option;
    int options[14];
    memset(options, 0, sizeof(options));
    for(int i=0; i<14; i++){
        printf("%d ", options[i]);
    }
    while((option = getopt(argc, argv, "aACdeEhmotTuvV")) != -1){
        //Is a switch statement really the best way of doing this???
        switch(option){
            case 'a' : 
                printf("a was selected\n");
                options[0] = 1;
                break;
            case 'A' : printf("A was selected\n");
                       break;
            case 'C' : printf("C was selected\n");
                       break;
            case 'd' : printf("d was selected\n");
                       break;
            case 'e' : printf("e was selected\n");
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
            case 'V' : printf("E was selected\n");
                       break;
            default : exit(1);
        }
    }
    return 0;
}



