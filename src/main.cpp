/*
 * main.cpp
 *
 *  Created on: May 9, 2017
 *      Author: Ivan Larkov
 */

#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
#ifdef LOGFILE
    // вывод лога в файл
    ofstream ofs("/home/sabbat/logfile");
    clog.rdbuf(ofs.rdbuf());
#endif
    // если передан аргумент, то вывести его длину и значение
    if(argc > 1) clog << strlen(argv[1]) << endl;
    clog << argv[1] << endl;

    CloudControl yandexDisk;
//    if(!yandexDisk.getFirstCopy())
//    {
//        yandexDisk.transerToken(argv[1]);
//        exit(EXIT_FAILURE);
//    }
//    auto response = yandexDisk.simpleRequest("upload?path=/Приложения/Todoom/test2&overwrite=true", GET);
//    cout << response["href"] << endl;
//    for(auto a : response)
//    {
//        cout << a << endl;
//    }
    string db_path;
    db_path = "1.db";
    auto db = make_shared<Database>(db_path);
    db->createTable();
}
