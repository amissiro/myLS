//Azret Missirov
//29512170

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void penetrateDir (char *dir,int n);

int isFile (char* name)
{
    struct stat buf;
    stat(name, &buf);
    return S_ISREG(buf.st_mode);
}

int isDir (char* name)
{
    struct stat buf;
    stat(name, &buf);
    return S_ISDIR(buf.st_mode);
}

char *groupmaker(gid_t uid)
{
    struct group *group_str = getgrgid(uid);
    if (group_str!=NULL) return group_str->gr_name;
    else return NULL;
}

char *uidmaker(uid_t uid)
{
    struct passwd *user_str = getpwuid(uid);
    if (user_str!=NULL) return user_str->pw_name;
    else return NULL;
}

char *timemaker(time_t t)
{
    struct tm time;
    localtime_r(&t,&time);
    static char time_str[30];
    strftime(time_str, sizeof(time_str), "%c", &time);
    time_str[strlen(time_str)-8] = '\0';
    return strchr(time_str,' ');
}

float sizemaker(size_t fsize)
{
    return (float)fsize/1024;
}


void permmaker (char *permstr, mode_t mode)
{
    char *r1,*r2,*r3,*w1,*w2,*w3,*x1,*x2,*x3,*d;
    if (S_ISDIR(mode)) d = "d"; else d = "-";
    if (mode & S_IRUSR) r1 = "r"; else r1 = "-";
    if (mode & S_IWUSR) w1 = "w"; else w1 = "-";
    if (mode & S_IXUSR) x1 = "x"; else x1 = "-";
    if (mode & S_IRGRP) r2 = "r"; else r2 = "-";
    if (mode & S_IWGRP) w2 = "w"; else w2 = "-";
    if (mode & S_IXGRP) x2 = "x"; else x2 = "-";
    if (mode & S_IROTH) r3 = "r"; else r3 = "-";
    if (mode & S_IWOTH) w3 = "w"; else w3 = "-";
    if (mode & S_IXOTH) x3 = "x"; else x3 = "-";
    sprintf (permstr, "%s%s%s%s%s%s%s%s%s%s",d,r1,w1,x1,r2,w2,x2,r3,w3,x3);
}

void print_sp (int n)
{
    printf("%*c",n*4,' ');
}
void fileInfo(char* fname, int dir)
{
    char str[1024],sl = '/'; int shift = 4;
    struct stat buf; stat(fname, &buf);
    if (dir==0) sl=' ';
    permmaker(str,buf.st_mode);
    printf("  %s %*d %-10s %-10s %8.1fK  %s %s%c\n",str ,shift,(int)buf.st_nlink,uidmaker(buf.st_uid),groupmaker(buf.st_gid),sizemaker(buf.st_size),
                timemaker(buf.st_mtime),
                strrchr(fname,'/')+1,sl);
}

void fileInfoSimple(char* fname)
{
    char str[1024]; int shift = 4;
    struct stat buf; stat(fname, &buf);
    permmaker(str,buf.st_mode);
    printf("  %s %*d %-10s %-10s %8.1fK  %s %s\n",str,shift,(int)buf.st_nlink,uidmaker(buf.st_uid),groupmaker(buf.st_gid),sizemaker(buf.st_size),
                timemaker(buf.st_mtime),fname);
}


void dirInfo(char* fname)
{
    printf("%s:\n",fname);
}

char *pathmaker (char *dir, char *d_name)
{
    char * allocpath  = malloc(strlen(dir) + strlen(d_name)+10000);
    strcpy(allocpath, dir);
    strcat(allocpath, "/");
    strcat(allocpath, d_name);
    return allocpath;
}


void loop1(DIR *dirp,struct dirent *dr, char * dir, int n){
    while(NULL != (dr = readdir(dirp))){
        if (strcmp(dr->d_name,".")!=0 && strcmp(dr->d_name,"..")!=0){
            char *path = pathmaker(dir, dr->d_name);
            if (isFile(path)==1) {print_sp(n-1);fileInfo(path,0);}
            if (isDir(path)==1)  {print_sp(n-1);fileInfo(path,1);}
            free(path);
        }
    }
}

void loop2(DIR *dirp,struct dirent *dr, char * dir, int n)
{
    while(NULL != (dr = readdir(dirp))){
        if (strcmp(dr->d_name,".")!=0 && strcmp(dr->d_name,"..")!=0){
            char *path = path = pathmaker(dir, dr->d_name);
            if (isDir(path)==1) { print_sp(n);penetrateDir(path,n+1);}
            free(path);
            
        }
    }
}

void penetrateDir (char *dir,int n)
{
    dirInfo(dir);
    DIR *dirp = NULL;
    struct dirent *dr = NULL;
    if ((dirp = opendir(dir))==NULL) printf("Permission denied\n");
    else{
        loop1(dirp,dr,dir,n); dirp = opendir(dir);
        loop2(dirp,dr,dir,n); closedir(dirp);
    }
    
}


int main(int argc, char *argv[])
{
    int i;
    if (argc==1){penetrateDir(".",1);}
    else{
    for (i=1; i < argc; i++){
        if (isFile(argv[i])){fileInfoSimple(argv[i]);}
        else{
        if (argv[i][strlen(argv[i])-1]=='/')
            argv[i][strlen(argv[i])-1] = '\0';
            penetrateDir(argv[i],1);}
    }
  }
}
