/*
 * > File Name: demo.cpp
 * > Author: Jack Kang
 * > Mail: kangyijie@xiyoulinux.org
 * > Created Time: 2017年06月18日 星期日 10时09分49秒
 */

#include <iostream>
#include "SQL.h"
using namespace std;
int main(void)
{
    string passwd;
    cout << "passwd :" <<endl;
    cin >> passwd;
    SQL demo(passwd);
    if(demo.connect_db()){
        return 1;
    }
    cout << "input you find to seach word: ";
    string word;
    cin >> word;
    if(demo.query_words(word)){
        return 1;
    }
    return 0;
}
