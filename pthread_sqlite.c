#include "pthread_sqlite.h"
#include "sqlite3.h"

#define TEMPERATURE_MAX   1
#define TEMPERATURE_MIN   2
#define HUMIDITY_MAX      3
#define HUMIDITY_MIN      4
#define ILLUMINATION_MAX  5
#define ILLUMINATION_MIN  6
#define TELEPHONE_NUM     7
#define MODE              8
#define IFNOCICE          9
#define INTERVAL          10
#define UPDATE_PERIOD     11
#define USER_NAME         12
#define PASSWORD          13

int InsertOrAddFlag = 0;
float callback_val[15];
char callback_str[20];

extern int storageNum;
extern int goodsKinds;

extern struct storage_info storage_no_s;
extern struct env_info_clien_addr env_info_clien_addr_s;
extern struct env_info_clien_addr all_info_RT;
extern struct sqlite_operation sqlite_operation_s;


extern slinklist slinkHead, slinkTail;

//获得环境量的回调函数
static int callback_getenv (void *para, int n_column, char **column_value, char **column_name)
{
    int i;
#if DEBUG_SQLITE
    printf("n_column = %d\n", n_column);
#endif

    for (i = 0; i < n_column; i++)
    {
        if (n_column - 1 == i)
        {
            strcpy (callback_str, column_value[i]);
#if DEBUG_SQLITE
            printf("%s = %s\n", column_name[i], callback_str);
#endif
        }
        else
        {
            callback_val[i] = (float)atof(column_value[i]);
#if DEBUG_SQLITE
            printf("%s = %f\n", column_name[i], callback_val[i]);
#endif
        }
    }
    InsertOrAddFlag = (int)callback_val[1];
#if DEBUG_SQLITE
    printf("****************************\n");
#endif
    return 0;
}

//获取多个环境变量
static int callback_getenv_s (void *para, int n_column, char **column_value, char **column_name )
{
    strcpy (callback_str, column_value[0])
#if DEBUG_SQLITE
    printf("%s = %s\n", column_name[0], callback_str);
#endif
    return 0;
}

//创建数据库表
int Create_table (void)
{
    char sql[1024];
    sqlite3 *db;
    char *err_msg = 0;
    int recode;

    recode = sqlite3_open(SQLITE_OPEN, &db);
    if (recode != SQLITE_OK)
    {
        printf("Can't Open Database:%s!\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    else
    {
#if DEBUG_SQLITE
    printf ("open OK!\n");
#endif
        sprintf (sql, "create table env(dev_no int not null,\
                                        temperatureMAX float not null,\
                                        temperatureMIN float not nul,\
                                        humidityMAX float not null,\
                                        humidityMIN float not null,\
                                        illuminationMax float not null,\
                                        illuminationMIN float not null,\
                                        telephoneNum varchar(11) not null,\
                                        mode int not null,\
                                        ifnocice int not null,\
                                        interval int not null,\
                                        updatePeriod int not null,\
                                        username varchar(20) not null,\
                                        password int not null);");
        retcode = sqlite3_exec (db, sql, NULL, NULL, &err_msg);
        if (SQLITE_OK != retcode)
        {
            if (strcmp(err_msg, "table env already exists"))
            {
                printf("Error:%s\n", err_msg);
                sqlite3_close(db);
                return 1;
            }
            else{
                printf("table env already exist, so open it ok\n");
            }
        }
        else{
//#if DEBUG_SQLITE
            Init_table_env();
            printf("create env OK!\n");
//#endif
        }
        sprintf (sql, "create table collect_env(dev_no32 int not null,\
                                                temperatureVal float not null,\
                                                humidityVal float not null,\
                                                illuminationVal float not null,\
                                                envtime varchar(10) not null);");
        recode = sqlite3_exec(db, sql, NULL, NULL, &err_msg)
        if (SQLITE_OK != recode)
        {
            if (strcmp(err_msg, "table collect_env already exists"))
            {
                printf("Error:%s", err_msg);
                sqlite3_close(db);
                return 1;
            }
            else{
                printf("table collect_env already exists, so open it ok!\n");
            }
//else
//          {
#if DEBUG_SQLITE
                printf("create collect_env Ok!\n");
#endif

#if DEBUG_SQLITE
                printf("collect_env init OK!\n");
#endif
//          }
            sprintf (sql, "create table goods(dev_no int not null,\
                                                goodsId int not null,\
                                                goods_count int not null,\
                                                goodsTime varchar(10) not null);");
            recode = sqlite3_exec (db, sql, NULL, NULL, &err_msg);
            if (recode != SQLITE_OK)
            {
                if (strcmp(err_msg, "table goods already exists"))
                {
                    printf("Error:%s", err_msg);
                    sqlite3_close(db);
                }
                else{
                    printf("table goods already exists, so open it ok");
                }
            }
//      else
//      {
#if DEBUG_SQLITE
            printf("create goods OK!\n");
#endif
            sqlite3_close(db);
            return 0;
//      }
        }
    }
}

 //初始化环境参数表
int  Init_table_env (void)
{
    char sql[1024];
    sqlite3 *db;
    char *err_msg;
    int recode;
    int i;

    for (i = 1; i <=2; i++)
    {
        sprintf (sql, "insert into env values('%d', 25.4, 10.5, 55.8, 10.8,\
                                                258, 129,'12345678901', 1, \
                                                2, 3, 4, 'binea', 123456);", i);
        recode = sqlite3_open (SQLITE_OPEN, &db);
        if (SQLITE_OK != recode)
        {
            printf("Can't Open Database:%s!\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return 1;
        }
        else{
#if DEBUG_SQLITE
            printf("Open Ok!\n");
#endif
            recode = sqlite3_exec (db, sql, NULL, NULL, &err_msg);
            if (SQLITE_OK !=  recode)
            {
                printf("Error:%s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return 1;
            }
            else
            {
#if DEBUG_SQLITE
                printf("insert env Ok!\n");
#endif
                sqlite3_close (db);
            }
        }
    }
    return 0;
}

//设置环境变量
int setEnv (float val, int dev_no_t, int no)
{
    char sql[1024];
    switch (no) {
        case 1:
            sprintf(sql, "update env set temperatureMAX = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
        case 2:
            sprintf(sql, "update env set temperatureMin = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
        case 3:
            sprintf(sql, "update env set humidityMax = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
        case 4:
            sprintf(sql, "update env set humidityMin = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
        case 5:
            sprintf(sql, "update env set illuminationMax = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
        case 6:
            sprintf(sql, "update env set illuminationMin = '%f' where dev_no = '%d';", val, dev_no_t);
            break;
#if 0
        case 7:
            sprintf(sql, "update env set telephoneNum = '%f';", val);
            break;
        case 8:
            sprintf(sql, "update env set mode = '%d';", (int)val);
            break;
        case 9:
            sprintf(sql, "update env set ifnocice = '%d';", (int)val);
            break;
        case 10:
            sprintf(sql, "update env set interval ='%d';", (int)val);
            break;
        case 11:
            sprintf(sql, "update env set updatePeriod = '%d';", (int)val);
            break;
//	    case 12:
//		    sprintf (sql, "update env set username = '%f';", val);
//		    break;
        case 13:
            sprintf(sql, "update env set password = '%d';", (int)val);
            break;
#endif
    }

    sqlite3 *db;
    char *err_msg = ""
    int recode;

    recode = sqlite3_open (SQLITE_OPEN, &db);
    if (SQLITE_OK != ret)
    {
        printf ("Can't open database:%s", err_msg);
        sqlite3_close(db);
        return 1;
    }
    else
    {
#if DEBUG_SQLITE
        printf("open OK!\n");
#endif
        recode = sqlite3_exec (db, err_msg, NULL, NULL, &err_msg);
        if (recode != SQLITE_OK)
        {
            printf("Error:%s", err_msg);
            sqlite3_close (db);
            return 1;
        }
        else
        {
#if DEBUG_SQLITE
            printf("Insert Ok!\n");
#endif
            sqlite3_close (db);
            return 0;
        }
    }
}

//设置环境变量
int setEnv_s (char *val, int no)
{
    char sql[1024];
    switch (no)
    {
        case 7:
            sprintf (sql, "update env set telephoneNum = '%s';", val);
        case 12:
            sprintf (sql, "update env set username = '%s'l", val);
    }

    sqlite3 *db;
    char *err_msg;
    int recode;
    recode = sqlite3_open (SQLITE_OPEN, &db);
    if (recode != SQLITE_OK)
    {
        printf ("Can't Open database:%s\n", sqlite3_errmsg(db));
        sqlite3_close (db);
        return 1;
    }
    else
    {
#if DEBUG_SQLITE
        printf("Open OK!\n");
#endif
        retcode = sqlite3_exec (db, sql, NULL, NULL, &err_msg);
        if (recode != SQLITE_OK)
        {
            printf("Error:%s", err_msg);
            sqlite3_close (db);
            return 1;
        }
        else
        {
#if DEBUG_SQLITE
            printf("Insert Ok!\n");
#endif
            sqlite3_close (db);
            return 0;
        }
    }
}

int getEnv_select (int storageNum_t, int no)
{
    char sql[1024];
    char *err_msg;
    int recode;
    sqlite3 *db;

    switch (no)
    {
        case 1:
            sprintf (sql, "select temperatureMax from env where dev_no = '%d';", storageNum_t);
            break;
        case 2:
            sprintf (sql, "select temperatureMin from env where dev_no = '%d';", storageNum_t);
            break;
        case 3:
            sprintf (sql, "select humidityMax from env where dev_no = '%d';", storageNum_t);
            break;
        case 4:
            sprintf (sql, "select humidityMin from env where dev_no = '%d';", storageNum_t);
            break;
        case 5:
            sprintf (sql, "select illuminationMax from env where dev_no = '%d';", storageNum_t);
            break;
        case 6:
            sprintf (sql, "select illuminationMin from env where dev_no = '%d';", storageNum_t);
            break;
#if 0
        case 7:
            sprintf (sql, "select telephoneNum from env;");
            break;
        case 8:
            sprintf (sql, "select mode from env;");
            break;
        case 9:
            sprintf (sql, "select ifnocice from env;");
            break;
        case 10:
            sprintf (sql, "select interval from env;");
            break;
        case 11:
            sprintf (sql, "select updatePeriod from env;");
            break;
        case 12:
            sprintf (sql, "select username from env;");
            break;
        case 13:
            sprintf (sql, "select password from env;");
            break;
#endif
    }

    recode = sqlite3_open (SQLITE_OPEN, &db);
    if (recode != SQLITE_OK)
    {
        printf("Can't open database:%s!\n", sqlite3_errmsg(db));
        sqlite3_close (db);
        return 1;
    }
    else
    {
#if DEBUG_SQLITE
        printf("Open Success\n", );
#endif
        if (7 == no || 12 == no)
        {
            recode = sqlite3_exec (db, sql, callback_getenv_s, NULL, &err_msg);
        }else
        {
            recode = sqlite3_exec (db, sql, callback_getenv, NULL, &err_msg);
        }
        if (recode != SQLITE_OK)
        {
            printf("Error:%s", err_msg);
            sqlite3_close (db);
            return 1;
        }
        else
        {
#if DEBUG_SQLITE
            printf ("get env ok!\n");
#endif
            sqlite3_close (db);
            return 0;
        }

    }
}

int updateEnv (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t)  //更新整个环境参数
{
    setEnv (env_info_clien_addr.storage_no[storageNum_t].temperatureMAX, storageNum_t, TEMPERATURE_MAX);
    setEnv (env_info_clien_addr.storage_no[storageNum_t].temperatureMIN, storageNum_t, TEMPERATURE_MIN);
    setEnv (env_info_clien_addr.storage_no[storageNum_t].humidityMAX, storageNum_t, HUMIDITY_MAX);
    setEnv (env_info_clien_addr.storage_no[storageNum_t].humidityMIN, storageNum_t, HUMIDITY_MIN);
    

}



































void sqlite_task(struct env_info_clien_addr env_info_clien_addr_t, struct sqlite_operation sqlite_operation_t, int storageNum_t, int goodsKinds_t);
int getEnv (struct env_info_clien_addr *env_info_clien_addr_t, int storageNum_t);  //获得整个环境参数
int insertCollect_env (struct env_info_clien_addr env_info_clien_addr_t, int storage_num);
int getCollect_env (char itime_t[], int storage_num_t);
int getCollect_Current_env (int storage_num_t);
int insertGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);  //插入新的货物信息
int deletGoods (int storageNum_t, int goodsKinds_t);  //删除指定编号货物信息
int getGoods (int storageNum_t, int goods_kinds_t);  //获得指定编号货物信息
int getGoodsInfo (void);  //获得所有货物信息
int viewGoods (int storageNum_t, int goodsKinds_t);  //查看仓库是否存在此类货物
int addGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);
int reduceGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);
