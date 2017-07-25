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
    ofstream ofs("logfile");
    clog.rdbuf(ofs.rdbuf());
#endif
    // если передан аргумент, то вывести его длину и значение
//    if(argc > 1) clog << strlen(argv[1]) << endl;
//    clog << argv[1] << endl;

    YandexCloudControl yandexDisk;
    yandexDisk.getToken();

    string app_dir = "/home/sabbat/.local/share/Todoom/";
    stringbuf buffer;
    ostream os(cout.rdbuf());
    yandexDisk.syncWithCloud();
    yandexDisk.watchFolder(app_dir);
    int temp;
    cin >> temp;

/*    string db_path;
    db_path = "1.db";
    auto db = make_shared<Database>(db_path);
    db->createTable();*/
}
