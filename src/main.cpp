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

    CloudControl yandexDisk(YANDEX);
    yandexDisk.getToken();
    yandexDisk.downloadFile("test_file.txt", "");
//    auto j = yandexDisk.listDirictory("")["_embedded"]["items"];
//    for(auto& a : j)
//    {
//        std::cout << a["name"] << '\n';
//    }
//    yandexDisk.uploadFile("Makefile", "test_file.txt");
    string db_path;
    db_path = "1.db";
    auto db = make_shared<Database>(db_path);
    db->createTable();
}
