/*
 * > File Name: SQL.h
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年06月17日 星期六 22时21分58秒
 */

#ifndef _SQL_H
#define _SQL_H
#include <mysql/mysql.h>
class SQL{
    public;
    SQL(string passwd,string filename = ){} // get passwd & words file`s name
    ~SQL() {}                               // close MySQL connect
    int connect_db();                       // connect MySQL Server
    int txt2db();                           // store words to DB
    int query_words();                      // query_words

    private:
    int _query_db(string );
    string _filename;
    string _passwd;
    MYSQL *db;
    MYSQL_ROW row;
    MYSQL_RES* result;
    MYSQL_FIELD* field; 
};
int SQL::connect_db()
{
    db = mysql_init(db);
    if(!db){
        cout << "connect DB ERROR: " << mysql_error(db) << endl;
        return 1;
    }

}
#endif
