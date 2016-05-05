#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtXml/QtXml>
#include <QtGui>
#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <QDir>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDateTime>
#include <QList>
#include <QRegularExpression>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopWidget>
#include <QProgressBar>
#include <QAbstractListModel>
#include <QFileDialog>
#include <QDebug>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        QString ListElements(QDomElement root, QString tagname);
        QString episodeTMDB(QString staffel, QString episode, QString id, QString dateiendung, QString year);
        QString episodeTVDB(QString staffel, QString episode, QString id, QString dateiendung, QString year);
        QString episodeTVMAZE(QString staffel, QString episode, QString id, QString dateiendung, QString year);
        QString episodeOMDB(QString staffel, QString episode, QString id, QString dateiendung, QString year);
        QStringList episodeTVDBList(QString id, QString year);
        QString cleanName(QString name);
        void setLogText(const QString logText, QString farbe);
        QString cleanSearch(QString suche);
        QString doDownload(QUrl url);
        QByteArray doCoverDownload(QUrl url);

    protected:
        void changeEvent(QEvent*);

    private slots:
        void on_treeView1_clicked(const QModelIndex &index);
        void on_lineEdit_returnPressed();
        void sendRequest_tmdb(const QString &suchtext);
        void sendRequest_tvdb(const QString &suchtext);
        void sendRequest_tvmaze(const QString &suchtext);
        void sendRequest_omdb(const QString &tv_suche);
        void on_pushButton_clicked();
        void on_listView1_doubleClicked(const QModelIndex &index);
        void on_pushButton_2_clicked();
        void renameEpisoden(const QString &rename);
        void renameEpisodenByNumber(const QString &rename);
        void on_actionEinstellungen_triggered();
        void resizeEvent(QResizeEvent* event);
        void moveEvent(QMoveEvent* event);
        void LoadSettings();
        void on_actionInfo_triggered();
        void on_listView3_doubleClicked();
        void clearList();
        void clearList2();
        void slotLanguageChanged(QAction* action);
        void switchTranslator(QTranslator& translator, const QString& filename);
        void updateProgress(qint64 read, qint64 total);
        void on_pushButton_3_clicked();
        void loadCover_tmdb(QString serienID);
        void loadCover_tvdb(QString serienID);
        void savePicture(QString url, QString bild, QString name);
        void on_actionEnglisch_triggered();
        void on_actionDeutsch_triggered();
        void saveLanguageSettings(QString lang);
        void on_pushButton_4_clicked();
        void loadRekursiv();
        void on_pushButton_5_clicked();
        void on_pushButton_6_clicked();

    private:
        Ui::MainWindow *ui;
        QFileSystemModel *dirmodel;
        QFileSystemModel *filemodel;
        QNetworkAccessManager *mgr;
        QStringList *dataList;
        QStringListModel *dlist;
        QStringListModel *mlist;
        QStringList *stringList;
        QStringListModel *rlist;
        QStringList *renameList;
        QModelIndex *parentIndex;
        QDesktopWidget *desktop;
        void loadLanguage(const QString& rLanguage);
        void createLanguageMenu(void);
        QTranslator m_translator;
        QString m_currLang;
        QString m_langPath;
        QProgressBar *progressBar;
};

#endif // MAINWINDOW_H
