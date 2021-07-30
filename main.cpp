#include <QCoreApplication>
#include <QFile>

#define FUNCSTART 5
#define VARISTART 6

bool ReadFile(QString path, QVector<QStringList> *funcs, QStringList *txt) {
    QFile file(path);
    //文件是否存在
    if (file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }
        //读取一行，并去空格
        QString line = file.readLine().simplified();
        //这一行是否为空
        while(!line.isEmpty()) {
            if (line.contains("main", Qt::CaseSensitive)) {
                txt->append(line);
            }
            else if (line.contains("_4w_comb", Qt::CaseSensitive)) {
                txt->append(line);
            }
            //找void函数
            else if (line.contains("void", Qt::CaseSensitive)) {
                //变量的起始位置
                int begin = line.indexOf("(");
                //变量的结束位置
                int end = line.indexOf(")");
                //截取函数名
                QString func_name = line.mid(FUNCSTART, begin - FUNCSTART);
                //截取变量列表
                QString vlist = line.mid(begin + 1, end - begin - 1);
                QStringList list = vlist.split(", ");
                for (int i = 0; i < list.length(); i++) {
                    QString temp = list[i].mid(VARISTART, -1).trimmed();
                    list[i] = temp;
                }
                //函数名
                list.insert(0, func_name);
                //函数{
                QString kong = file.readLine().simplified();
                //函数具体功能
                QString formula = file.readLine().simplified();
                //函数}
                kong = file.readLine().simplified();
                list.append(formula);
                //完成一个函数的分装
                funcs->append(list);
            }
            else {
                txt->append(line);
            }

            line = file.readLine().simplified();
        }
        file.close();
    }
    else {
        return false;
    }
    return true;
}

bool WriteFile(QString path, QVector<QStringList> *funcs, QStringList *txt) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    int txtlength = txt->length();
    int funsclength = funcs->length();

    QStringList _list;

    printf("%d\n", txtlength);
    for (int i = 0; i < txtlength; i++) {
        //file.write((*txt)[i].toLatin1() + "\n");
        if ((*txt)[i].contains("void", Qt::CaseSensitive)) {
            //file.write(txt->at(i).toLatin1());
            //是否为_4w_comb函数的输入参数
            if ((*txt)[i].contains("_4w_comb", Qt::CaseSensitive)) {
                //变量的起始位置
                int begin = (*txt)[i].indexOf("(");
                //变量的结束位置
                int end = (*txt)[i].indexOf(")");
                QString vlist = (*txt)[i].mid(begin + 1, end - begin - 1);
                _list = vlist.split(", ");
                for (int j = 0; j < _list.length(); j++) {
                    _list[j] = _list[j].mid(7, -1);
                }
            }
            file.write((*txt)[i].toLatin1() + "\n");
        }
        else if((*txt)[i].contains("(", Qt::CaseSensitive) && (*txt)[i].contains(")", Qt::CaseSensitive)) {
            //变量的起始位置
            int begin = (*txt)[i].indexOf("(");
            //变量的结束位置
            int end = (*txt)[i].indexOf(")");
            //截取函数名
            QString func_name = (*txt)[i].mid(0, begin);
            QString vlist = (*txt)[i].mid(begin + 1, end - begin - 1);
            QStringList list = vlist.split(", ");
            list[list.length() - 1] = list[list.length() - 1].mid(1, -1);

            //对_4w_comb的输入参数进行替换
            for (int k = 0; k < list.length(); k++) {
                for (int x = 0; x < _list.length(); x++) {
                    if (list[k] == _list[x]) {
                        list[k] = "*" + _list[x];
                        break;
                    }
                }
            }

            //是否在函数库中找到
            bool flag = false;
            //依次替换
            for (int j = 0; j < funsclength; j++) {
                if ((*funcs)[j][0] == func_name) {
                    QString out = "  " + (*funcs)[j][(*funcs)[j].length() - 1];
                    for (int k = 0; k < list.length(); k++) {
                        out.replace((*funcs)[j][k + 1], list[k]);
                    }
                    file.write(out.toLatin1() + "\n");
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                file.write((*txt)[i].toLatin1() + "\n");
            }

        }
        else {
            file.write((*txt)[i].toLatin1() + "\n");
        }
    }
    file.close();
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString inputpfile = "/home/yyzheng/zyy/work/SP/new.c";
    QString outputpfile = "/home/yyzheng/zyy/work/SP/out.c";
    //收集func
    QVector<QStringList> funcs;
    //输出文本
    QStringList txt;

    if (ReadFile(inputpfile, &funcs, &txt)) {
        if (WriteFile(outputpfile, &funcs, &txt)) {
            printf("file write success\n%s\n", outputpfile.toStdString().data());
        }
        else {
            printf("file write fail\n");
        }
    }
    else {
        printf("file not exist\n");
    }

    return a.exec();
}
