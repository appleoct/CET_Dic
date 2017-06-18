/*
 * > File Name: SQL.h
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年06月17日 星期六 22时21分58秒
 */

#ifndef _SQL_H
#define _SQL_H
#include <iostream>
#include <string>
#include <mysql/mysql.h>
using namespace std;
class SQL{
    public:
    SQL(string passwd,string filename = "words.txt"): _passwd(passwd),_filename(filename) {} 
    // get passwd & words file`s name
    ~SQL() {mysql_close(db);}               // close MySQL connect
    int connect_db();                       // connect MySQL Server
    int txt2db();                           // store words to DB
    int query_words(string find_words);     // query_words

    private:
    int _query_db(string sql);
    string _filename;
    string _passwd;
    MYSQL *db;
    MYSQL_ROW row;
    MYSQL_RES* result;
    MYSQL_FIELD* field; 
};
int SQL::_query_db(string sql)
{
    if(mysql_query(db,sql.c_str())){
        cout << "Query ERROR : " << mysql_error(db) << endl;
        return 1;
    }
    result = mysql_store_result(db);
    if(!result){
        cout << "Store ERROR: "  << mysql_error(db) << endl;
    }
    while(1){
        row = mysql_fetch_row(result);
        if(!row)
            break;
        cout << row[0] << "-->" << row[1] << endl;
    }
    return 0;

}
int SQL::txt2db()
{
    
}
int SQL::query_words(string find_words)
{
    string sql("SELECT en,zh FROM MyDic WHERE en = \'"+ find_words +"\'");
    cout << sql << endl;
    return _query_db(sql);

}
int SQL::connect_db()
{
    db = mysql_init(db);
    if(!db){
        cout << "connect DB ERROR: " << mysql_error(db) << endl;
        return 1;
    }
    if(!mysql_real_connect(db,"localhost","root",_passwd.c_str(),"mysql",0,NULL,0)){
        cout << "connect DB ERROR: " << mysql_error(db) << endl;
        return 1;
    }
    return 0 ;
}
#endif
