#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
static const char *code =
    "static int scmp(const char *a, const char *b) {\n"
    "  while (*a && (*a == *b)) a++, b++;\n"
    "  return *(const unsigned char *) a - *(const unsigned char *) b;\n"
    "}\n"
    "const char *mg_unlist(size_t no);\n"
    "const char *mg_unlist(size_t no) {\n"
    "  return packed_files[no].name;\n"
    "}\n"
    "const char *mg_unpack(const char *path, size_t *size, time_t *mtime);\n"
    "const char *mg_unpack(const char *name, size_t *size, time_t *mtime) {\n"
    "  const struct packed_file *p;\n"
    "  for (p = packed_files; p->name != NULL; p++) {\n"
    "    if (scmp(p->name, name) != 0) continue;\n"
    "    if (size != NULL) *size = p->size - 1;\n"
    "    if (mtime != NULL) *mtime = p->mtime;\n"
    "    return (const char *) p->data;\n"
    "  }\n"
    "  return NULL;\n"
    "}\n";
void MainWindow::on_createButton_clicked()
{
    int k, j, ch;
    QString curretFiepath = QDir::currentPath();
    ui->tview->append("current file path:"+curretFiepath);
    QDir dir(curretFiepath);
    QFileInfoList list = dir.entryInfoList();
    QString retpath;
    QString outfilename = curretFiepath + "/packed_fs.c";         // 输出文件的路径+文件名
    FILE *outfp = fopen(outfilename.toStdString().c_str(), "w+"); // 创建输出文件
    k = 0;
    fprintf(outfp, "%s", "#include <stddef.h>\n");
    fprintf(outfp, "%s", "#include <string.h>\n");
    fprintf(outfp, "%s", "#include <time.h>\n");
    fprintf(outfp, "%s", "\n");
    for (int i = 0; i < list.size(); i++) // 遍历每个文件
    {
        QFileInfo info = list.at(i); // 得到每个文件的信息
        if (info.suffix() == "html" ||
            info.suffix() == "shtml" ||
            info.suffix() == "js" ||
            info.suffix() == "css")
        {
            k++;
            retpath = info.path() + "/" + info.fileName(); // 文件路径+文件名
            char ascii[12];
            FILE *fp = fopen(retpath.toStdString().c_str(), "rb");
            fprintf(outfp, "static const unsigned char v%d[] = {\n", k); // 第几个文件就是v几
            for (j = 0; (ch = fgetc(fp)) != EOF; j++)                    // 挨个读取文件的每个字节
            {
                if (j == (int)sizeof(ascii))
                {
                    fprintf(outfp, " // %.*s\n", j, ascii);
                    j = 0;
                }
                ascii[j] = (char)((ch >= ' ' && ch <= '~' && ch != '\\') ? ch : '.');
                fprintf(outfp, " %3u,", ch);
            }
            // Append zero byte at the end, to make text files appear in memory
            // as nul-terminated strings.
            // printf(" 0 // %.*s\n", (int) sizeof(ascii), ascii);
            fprintf(outfp, " 0 // %.*s\n};\n", j, ascii);
            fclose(fp);
            ui->tview->append("read from :  "+retpath);
        }
    }
    fprintf(outfp, "%s", "\nstatic const struct packed_file {\n");
    fprintf(outfp, "%s", "  const char *name;\n");
    fprintf(outfp, "%s", "  const unsigned char *data;\n");
    fprintf(outfp, "%s", "  size_t size;\n");
    fprintf(outfp, "%s", "  time_t mtime;\n");
    fprintf(outfp, "%s", "} packed_files[] = {\n");
    k = 0;
    for (int i = 0; i < list.size(); i++) // 遍历每个文件
    {
        QFileInfo inf = list.at(i); // 得到每个文件的信息
        if (inf.suffix() == "html" ||
            inf.suffix() == "shtml" ||
            inf.suffix() == "js" ||
            inf.suffix() == "css")
        {
            k++;
            fprintf(outfp, "  {\"/%s\", v%d, sizeof(v%d), %lu},\n", inf.fileName().toStdString().c_str(), k, k, (unsigned long)inf.lastModified().toTime_t());
        }
    }
    fprintf(outfp, "%s", "  {NULL, NULL, 0, 0}\n");
    fprintf(outfp, "%s", "};\n\n");
    fprintf(outfp, "%s", code);
    ui->tview->append("created file:"+curretFiepath + "/packed
                      _fs.c");
    fclose(outfp);
}
