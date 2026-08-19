/*
                    GNU GENERAL PUBLIC
                        Version 3

         此 Code 来自 ClearBox 模块，用于 NCDU 功能
*/

#include "INCLUDE/main.h"

#define SERVER_NAME "ncdu"
#define HISTORY_FILE "ncdu.db"

#define DIR_MAX 1024

struct info
{
    char name[NAME_MAX];
    char path[PATH_MAX];
    char mode;
    int64_t byte;
    double size;
    char unit;
    char history_size[64];
};

static int cmp_byte(const void * a, const void * b);

int ncdu(char * path)
{
    struct info path_info[DIR_MAX];
    char history_file[strlen(work_dir) + sizeof(HISTORY_FILE) + 2];
    snprintf(history_file, sizeof(history_file), "%s/%s", work_dir, HISTORY_FILE);
    
    sqlite3 * db;
    sqlite3_open(history_file, &db);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS kv(key TEXT UNIQUE, value TEXT);", 0, 0, 0);
    
    struct dirent * entry;
    DIR * path_dp = opendir(path);
    if (path_dp == NULL)
    {
        write_log(work_dir, SERVER_NAME, L_OPEN_PATH_FAILED, path, strerror(errno));
        return -1;
    }
    
    int count = 0;
    while ((entry = readdir(path_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // NAME
        snprintf(path_info[count].name, sizeof(path_info[count].name), "%s", entry -> d_name);
        
        // PATH
        snprintf(path_info[count].path, sizeof(path_info[count].path), "%s/%s", path, entry -> d_name);
        
        // MODE
        struct stat st;
        if (lstat(path_info[count].path, &st) == -1)
        {
            continue;
        }
        if (S_ISREG(st.st_mode))
        {
            path_info[count].mode = 'F';
        }
        else if (S_ISDIR(st.st_mode))
        {
            path_info[count].mode = 'D';
        }
        else
        {
            path_info[count].mode = 'N'; // NULL
        }
        
        // SIZE
        path_info[count].byte = get_path_size(path_info[count].path);
        path_info[count].size = byte_to_size(path_info[count].byte, &path_info[count].unit);
        
        // HISTORY
        char ** buffer;
        int64_t history_size = -1;
        int row = 0, col = 0;
        char * sql_str = NULL;
        
        sql_str = sqlite3_mprintf("SELECT value FROM kv WHERE key = '%q';", path_info[count].path);
        if (sql_str)
        {
            sqlite3_get_table(db, sql_str, &buffer, &row, &col, 0);
            if (row > 0)
            {
                history_size = strtol(buffer[1], NULL, 10);
            }
            sqlite3_free(sql_str);
            sqlite3_free_table(buffer);
        }
        
        path_info[count].history_size[0] = '\0';
        if (history_size != -1)
        {
            char unit = '\0',
                 math_unit = '\0';
            int64_t status = (path_info[count].byte - history_size);
            double size = byte_to_size(status, &unit);
            
            if (status > 0)
            {
                math_unit = '+';
            }
            else if (status < 0)
            {
                math_unit = '-';
            }
            else
            {
                math_unit = '\0';
            }
            
            snprintf(path_info[count].history_size, sizeof(path_info[count].history_size), "%c%.2f%c",
                     math_unit, size, unit);
        }
        
        count++;
        if (count >= DIR_MAX)
        {
            break;
        }
    }
    
    qsort(path_info, (size_t)count, sizeof(struct info), cmp_byte);
    
    for (int i = 0; i < count; i++)
    {
        // PRINT:
        // NAME PATH SIZE UNIT MODE
        printf("%s|%s|%.2f|%c|%s|%c\n",
                path_info[i].name,
                path_info[i].path,
                path_info[i].size,
                path_info[i].unit,
                path_info[i].history_size,
                path_info[i].mode
              );
        
        char * sql_str = sqlite3_mprintf("INSERT OR REPLACE INTO kv(key, value) VALUES ('%q', '%lld');", path_info[i].path, path_info[i].byte);
        sqlite3_exec(db, sql_str, 0, 0, 0);
        sqlite3_free(sql_str);
    }
    
    sqlite3_close(db);
    closedir(path_dp);
    return 0;
}

static int cmp_byte(const void * a, const void * b)
{
    const struct info * a_p = (const struct info *)a;
    const struct info * b_p = (const struct info *)b;
    
    if (a_p -> byte > b_p -> byte)
    {
        return -1;
    }
    if (a_p -> byte < b_p -> byte)
    {
        return 1;
    }
    return 0;
}
