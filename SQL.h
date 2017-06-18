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
#include <fstream>
#include <mysql/mysql.h>
using namespace std;
class SQL{
    public:
    SQL(string passwd,string filename = "words.txt"): _passwd(passwd),_filename(filename) {} 
    // get passwd & words file`s name
    ~SQL() {mysql_close(db);}               // close MySQL connect
    int init_words_db();
    int connect_db();                       // connect MySQL Server
    int query_words(string find_words);     // query_words

    private:
    int _query_db(string sql);
    int _txt2db();                           // store words to DB
    string _filename;
    string _passwd;
    MYSQL *db;
    MYSQL_ROW row;
    MYSQL_RES* result;
    MYSQL_FIELD* field; 
};
int SQL::init_words_db()
{
    return _txt2db();
}
int SQL::_query_db(string sql)
{
    //if(mysql_query(db,sql.c_str())){
    if(mysql_real_query(db,sql.c_str(),sql.size())){
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
int SQL::_txt2db()
{
    ifstream words_file(_filename);
    string en,zh;
    while(!words_file.eof()){

    words_file >> en >> zh;
    string sql("INESERT INTO MyDic(en,zh) VALUES("+ en +"," + zh +")");
    
    }

    words_file.close();
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
