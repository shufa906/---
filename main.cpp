#include "mainwindow.h"

#include <QApplication>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QWidget>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TextEditor");

    QCommandLineParser parser;
    parser.addOption({"load", "加载指定文本文件", "file"});
    parser.addOption({"shot", "窗口截图保存到指定路径后退出", "path"});
    parser.addOption({"delay", "截图前等待毫秒数", "ms", "600"});
    parser.addOption({"dialog", "启动后打开指定对话框: about/find/replace", "name"});
    parser.addOption({"select", "选中首行文字后截图", "1"});
    parser.addOption({"modified", "标记为已修改后截图", "1"});
    parser.process(a);

    MainWindow w;
    w.show();

    if (parser.isSet("load")) {
        const QString file = parser.value("load");
        if (QFile::exists(file))
            w.loadFile(file);
    }

    const QString dialog = parser.value("dialog");
    if (dialog == "about") {
        w.openAboutDialog();
        w.syncDialogGeometry();
    }
    else if (dialog == "find") {
        w.openFindDialog();
        w.fillDialogKeyword("编辑器");
        w.syncDialogGeometry();
    }
    else if (dialog == "replace") {
        w.openReplaceDialog();
        w.fillDialogKeyword("编辑器");
        w.syncDialogGeometry();
    }

    if (parser.isSet("select"))
        w.selectFirstLine();
    if (parser.isSet("modified"))
        w.appendCharForDemo();

    if (parser.isSet("shot")) {
        const int delay = parser.value("delay").toInt();
        QTimer::singleShot(delay, &w, [&w, &parser, dialog]() {
            QWidget *target = nullptr;
            const QWidgetList children = w.findChildren<QWidget *>();
            for (QWidget *c : children) {
                const QString t = c->windowTitle();
                if (dialog == "about" && (t == "About" || t.contains("Editor")))
                    target = c;
                else if (dialog == "find" && t == "Find")
                    target = c;
                else if (dialog == "replace" && t == "Replace")
                    target = c;
            }
            QPixmap shot;
            if (target) {
                shot = QPixmap::grabWindow(target->winId());
            } else {
                shot = QPixmap(w.size());
                shot.fill(Qt::white);
                w.render(&shot);
            }
            shot.save(parser.value("shot"));
            QApplication::quit();
        });
    }

    return a.exec();
}
