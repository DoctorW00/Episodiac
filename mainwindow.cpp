#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "einstellungen.h"
#include "info.h"

// TVDB API
QString apiKey_tvdb = "1D62F2F90030C444";
QString episoden_suche_tvdb = "http://thetvdb.com/api/GetSeries.php?language={TVLANG}&seriesname={SEARCH}";
QString episoden_info_tvdb = "http://thetvdb.com/api/{TVDB_API_KEY}/series/{ID}/all/{TVLANG}.xml";
// TMDb API
QString apiKey_tmdb = "f7f51775877e0bb6703520952b3c7840";
QString episoden_suche_tmdb = "http://api.tmdb.org/3/search/tv?query={SEARCH}&api_key={TMDB_API_KEY}&language={TVLANG}";
QString episoden_info_tmdb = "http://api.tmdb.org/3/tv/{ID}/season/{SEASON}/episode/{EPISODE}?api_key={TMDB_API_KEY}&language={TVLANG}";
// tvMaze API
QString apiKey_tvMaze = "";
QString episoden_suche_tvmaze = "http://api.tvmaze.com/search/shows?q={SEARCH}";
QString episoden_info_tvmaze = "http://api.tvmaze.com/shows/{ID}/episodebynumber?season={SEASON}&number={EPISODE}";
// OMDb API
QString apiKey_OMDb = "";
QString episoden_suche_omdb = "http://www.omdbapi.com/?type=series&r=json&v=1&s={SEARCH}";
QString episoden_info_omdb = "http://www.omdbapi.com/?i={ID}&Season={SEASON}&Episode={EPISODE}";
// diverses
QString xmlDoc_tvdb;
QString sereID_tvdb;
QString lastLn_tvdb;
QString getXML_tvdb = "no";
QString tvlang = "de";
QStringList tvlangList = (QStringList() << "de" << "en");
QString eRegEx = "\\b(?:[sS])?(\\d{1,2})(?:[xX]|[eE])?(\\d{1,3})\\b";
QString eRegEx2 = "\\b(?:[eE])?(\\d{1,3})\\b";
QString serienAufbau = "{SERIES} - S{SEASON2}E{EPISODE2} - {TITLE}";
QString OrdnerStruktur = "{SERIES} ({YEAR})/Season {SEASON2}";
int OrdnerStrukturCheck = 0;
QStringList VideoFilters = (QStringList() << "*.avi" << "*.mp4" << "*.mkv" << "*.wmv" << "*.mov" << "*.m4v" << "*.mpg" << "*.mpeg" << "*.flv" << "*.divx" << "*.xvid");
QStringList gSerie;
QString gPath;
QStringList gPathList;
QString g_ZIEL;
int g_ROOT = 0;
QStringList regExExcludeList = (QStringList() << "sample" << "vorschau" << "preview");
QString regExExclude;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // sprache festlegen
    QString locale = QLocale::system().name();
    if(locale.split("_").at(0) == "de")
    {
        loadLanguage("de");
    }
    else
    {
        loadLanguage("en");
    }

    // Einstellungen laden
    LoadSettings();

    // TV API Auswahl
    ui->comboBox->addItem(tr("The Movie Database (TMDb) [Staffel & Episode]"));
    ui->comboBox->addItem(tr("TheTVDB.com (TVDB) [Staffel & Episode]"));
    ui->comboBox->addItem(tr("TVmaze.com (TVmaze) [Staffel & Episode]"));
    ui->comboBox->addItem(tr("TheTVDB.com (TVDB) [Episoden]"));
    ui->comboBox->addItem(tr("OMDbapi.com (OMDb) [Staffel & Episode]"));
    ui->comboBox->setItemIcon(0, QIcon(":/bilder/icons/tmdb.png"));
    ui->comboBox->setItemIcon(1, QIcon(":/bilder/icons/thetvdb.png"));
    ui->comboBox->setItemIcon(2, QIcon(":/bilder/icons/tvmaze.png"));
    ui->comboBox->setItemIcon(3, QIcon(":/bilder/icons/thetvdb.png"));
    ui->comboBox->setItemIcon(4, QIcon(":/bilder/icons/omdb.png"));

    // Icons ToolBar
    ui->actionEinstellungen->setIcon(QIcon(":/bilder/icons/setup.png"));
    ui->actionInfo->setIcon(QIcon(":/bilder/icons/info.png"));
    ui->actionDeutsch->setIcon(QIcon(":/bilder/languages/de.png"));
    ui->actionEnglisch->setIcon(QIcon(":/bilder/languages/en.png"));

    setLogText(tr("Episodiac bereit!"), "green");

    QString sPath = QDir::homePath();
    gPath = sPath;

    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::NoSymLinks);
    dirmodel->setRootPath(sPath);

    // Verzeichnisauswahl
    ui->treeView1->setModel(dirmodel);
    ui->treeView1->hideColumn(1);
    ui->treeView1->hideColumn(2);
    ui->treeView1->hideColumn(3);

    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::NoSymLinks);
    filemodel->setNameFilters(VideoFilters);
    filemodel->setNameFilterDisables(false);
    //filemodel->setRootPath(sPath);
    ui->listView1->setModel(filemodel);
    ui->listView1->setRootIndex(filemodel->setRootPath(sPath));

    dataList = new QStringList();
    dlist = new QStringListModel(*dataList, NULL);
    //ui->listView1->setModel(dlist);
    dlist->setStringList(*dataList);

    stringList = new QStringList();
    mlist = new QStringListModel(*stringList, NULL);
    ui->listView3->setModel(mlist);
    mlist->setStringList(*stringList);

    renameList = new QStringList();
    rlist = new QStringListModel(*renameList, NULL);
    ui->listView2->setModel(rlist);
    rlist->setStringList(*renameList);

    // Button Vorschau
    ui->pushButton->setStyleSheet("background-color: green; color: white");
    ui->pushButton->setToolTip(tr("<font color='black'><b>Erstellt eine Vorschau, ändert nichts.</b></font>"));

    // Button Rename
    ui->pushButton_2->setStyleSheet("background-color: red; color: white");
    ui->pushButton_2->setToolTip(tr("<font color='black'><b>Ändert die Dateinamen</b></font>"));

    // Cover Download
    ui->pushButton_3->setIcon(QIcon(":/bilder/icons/download.png"));
    ui->pushButton_3->setToolTip(tr("<font color='black'><b>Lädt Cover, Artwork und verfügbare Fanarts herunter.</b></font>"));

    // Button rekursives Laden
    ui->pushButton_4->setIcon(QIcon(":/bilder/icons/folder.png"));
    ui->pushButton_4->setToolTip(tr("<font color='red'><b>Lädt Dateien aus Unterordnern rekursiv. Warnung: Je nach Anzahl und Ebenen von Unterverzeichnissen und Dateien, kann dies möglicherweise sehr lange dauern!</b></font>"));
    connect(ui->pushButton_4, SIGNAL(toggled(bool)), this, SLOT(clearList2()));

    // Button verschieben nach Zielverzeichnis wählen
    ui->pushButton_5->setIcon(QIcon(":/bilder/icons/copy.png"));

    // Button verschieben nach Stammorder wählen
    ui->pushButton_6->setIcon(QIcon(":/bilder/icons/copy.png"));

    // Suchfeld TV Serien (API)
    ui->lineEdit->setToolTip(tr("<font color='green'><b>Suchfeld für TV Serien. Gib einen Teil oder den ganzen Namen einer TV Serie ein und drücke [ENTER]</b></font>"));

    // Verzeichnisauswahl
    ui->treeView1->setStyleSheet("QTreeView { show-decoration-selected: 1; }");
    ui->treeView1->setStyleSheet("QTreeView::item:selected { border: 1px solid #ff0000; background-color: yellow; color: black; }");

    // TV Serien- Auswahl
    ui->listView3->setStyleSheet("QListView { show-decoration-selected: 1; }");
    ui->listView3->setStyleSheet("QListView::item:selected { border: 1px solid #ff0000; background-color: yellow; color: black; }");

    // nutzlose Nachricht
    ui->statusBar->showMessage(tr("Episodiac bereit!"));

    // macht die tv seriensuche sauber, da die serien-id nicht api übergreifend funktioniert
    connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(clearList()));

    // nutzlose Nachricht
    ui->label->setText(tr("Zielpfad: ") + gPath);

    // Ladebalken - damit sich was tut beim Laden
    progressBar = new QProgressBar(this);
    progressBar->setMaximumHeight(15);
    progressBar->setMaximumWidth(200);
    ui->statusBar->addPermanentWidget(progressBar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::doDownload(QUrl url)
{
    QString rResult;

    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    progressBar->setValue(0);

    //QNetworkRequest req{QUrl{url}};
    QNetworkRequest req(url);
    QNetworkReply *reply = mgr.get(req);
    reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        rResult = reply->readAll();
    }
    else
    {
        ui->statusBar->showMessage(tr("Verbindungfehler: ") + reply->errorString());
        setLogText(tr("Verbindungfehler: ") + reply->errorString(), "red");

        rResult = "0";
    }

    delete reply;
    return rResult;
}

QByteArray MainWindow::doCoverDownload(QUrl url)
{
    QByteArray rResult;

    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    progressBar->setValue(0);

    QNetworkRequest req(url);
    QNetworkReply *reply = mgr.get(req);
    reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        rResult = reply->readAll();
    }
    else
    {
        ui->statusBar->showMessage(tr("Verbindungfehler: ") + reply->errorString());
        setLogText(tr("Verbindungfehler: ") + reply->errorString(), "red");

        rResult = 0;
    }

    delete reply;
    return rResult;
}

void MainWindow::updateProgress(qint64 read, qint64 total)
{
    progressBar->setMaximum(total);
    progressBar->setValue(read);
}

void MainWindow::setLogText(const QString logText, QString farbe)
{
    if(QString(farbe).isEmpty())
    {
        farbe = "black";
    }
    QString datum = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
    ui->logWin->append("<font color='" + farbe + "'>[" + datum + "] " + logText + "</font>");

    QTextCursor cursor = ui->logWin->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->logWin->setTextCursor(cursor);
}

QString MainWindow::ListElements(QDomElement root, QString tagname)
{
    QDomNodeList items = root.elementsByTagName(tagname);

    QString element;
    for(int i = 0; i < items.count(); i++)
    {
        QDomNode itemnode = items.at(i);
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();
            element = itemele.text();
        }
    }
    return element;
}

QString MainWindow::cleanName(QString name)
{
    QString badChars = name;
    badChars.remove(QRegularExpression("[\\\\?:|,;*=<>/^\\[\\]]"));
    return badChars;
}

QString MainWindow::cleanSearch(QString suche)
{
    QString badChars = suche;
    badChars.remove(QRegularExpression("[^A-Za-z0-9 _]"));
    return badChars;
}

QString MainWindow::episodeTMDB(QString staffel, QString episode, QString id, QString dateiendung, QString year)
{
    QString tAufbau = episoden_info_tmdb;
    tAufbau.replace("{ID}", id);
    tAufbau.replace("{SEASON}", staffel);
    tAufbau.replace("{EPISODE}", episode);
    tAufbau.replace("{TMDB_API_KEY}", apiKey_tmdb);
    tAufbau.replace("{TVLANG}", tvlang);

    ui->statusBar->showMessage(tAufbau);
    setLogText(tAufbau, "");

    QString return_episode;

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument tmdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = tmdb.object();

        QString name = getjson.value("name").toString();

        if(!QString(name).isEmpty())
        {
            //name = cleanName(name);

            QString tAufbau = serienAufbau;
            QString tEpisodeA = QString("%1").arg(episode.toInt(), 2, 10, QChar('0'));
            QString tEpisodeB = QString("%1").arg(episode.toInt(), 1, 10, QChar('0'));
            QString tStaffelA = QString("%1").arg(staffel.toInt(), 2, 10, QChar('0'));
            QString tStaffelB = QString("%1").arg(staffel.toInt(), 1, 10, QChar('0'));

            foreach (QString s, gSerie)
            {
                QStringList sinfoSplit = s.split(";;;");
                QString sInfo = sinfoSplit.value(0);
                if(sInfo == id)
                {
                    QString sSerie = sinfoSplit.value(1);
                    tAufbau.replace("{SERIE}", sSerie);
                    tAufbau.replace("{SERIES}", sSerie);
                }
            }

            // für v1.1
            tAufbau.replace("{TITEL}", name);
            tAufbau.replace("{STAFFEL1}", tStaffelB);
            tAufbau.replace("{STAFFEL2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{JAHR}", year);

            // ab 1.2
            tAufbau.replace("{TITLE}", name);
            tAufbau.replace("{SEASON1}", tStaffelB);
            tAufbau.replace("{SEASON2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{YEAR}", year);

            return_episode = tAufbau + "." + dateiendung;
        }
        else
        {
            return_episode = "0";
        }
    }
    return cleanName(return_episode);
}

QString MainWindow::episodeTVDB(QString staffel, QString episode, QString id, QString dateiendung, QString year)
{
    QString return_episode;

    sereID_tvdb = id;
    if(QString(xmlDoc_tvdb).isEmpty() || QString(sereID_tvdb) != id || QString(lastLn_tvdb) != QString(tvlang) || QString(getXML_tvdb) == "yes")
    {
        QString tAufbau = episoden_info_tvdb;
        tAufbau.replace("{SEASON}", staffel);
        tAufbau.replace("{EPISODE}", episode);
        tAufbau.replace("{TVDB_API_KEY}", apiKey_tvdb);
        tAufbau.replace("{ID}", id);
        tAufbau.replace("{TVLANG}", tvlang);

        setLogText(tAufbau, "");
        ui->statusBar->showMessage(tAufbau);

        QString myReplay = doDownload(tAufbau);

        if(!myReplay.isEmpty() && myReplay != "0")
        {
            xmlDoc_tvdb = myReplay;
        }
        else
        {
            xmlDoc_tvdb = "0";
        }
    }

    QDomDocument domXML;

    if(!domXML.setContent(xmlDoc_tvdb))
    {
        setLogText(tr("Das empfangene XML von thetvdb.com ist leer!"), "red");
        ui->statusBar->showMessage(tr("Das empfangene XML von thetvdb.com ist leer!"));
        return_episode = "0";
    }
    else
    {
        QDomElement root = domXML.firstChildElement();
        QDomNodeList serien = root.elementsByTagName("Episode");

        QString SeasonNumber;
        QString EpisodeName;
        QString EpisodeNumber;

        for(int i = 0; i < serien.count(); i++)
        {
            QDomNode snode = serien.at(i);
            if(snode.isElement())
            {
                QDomElement serie = snode.toElement();

                SeasonNumber = ListElements(serie, "SeasonNumber");
                EpisodeName = ListElements(serie, "EpisodeName");
                EpisodeNumber = ListElements(serie, "EpisodeNumber");

                QString SeasonN = QString("%1").arg(SeasonNumber.toInt(), 2, 10, QChar('0'));
                QString EpisodeN = QString("%1").arg(EpisodeNumber.toInt(), 2, 10, QChar('0'));

                QString staffelA = QString("%1").arg(staffel.toInt(), 2, 10, QChar('0'));
                QString episodeA = QString("%1").arg(episode.toInt(), 2, 10, QChar('0'));

                if(staffelA == SeasonN && episodeA == EpisodeN)
                {
                    //EpisodeName = cleanName(EpisodeName);

                    QString tAufbau = serienAufbau;
                    QString tEpisodeA = QString("%1").arg(episode.toInt(), 2, 10, QChar('0'));
                    QString tEpisodeB = QString("%1").arg(episode.toInt(), 1, 10, QChar('0'));
                    QString tStaffelA = QString("%1").arg(staffel.toInt(), 2, 10, QChar('0'));
                    QString tStaffelB = QString("%1").arg(staffel.toInt(), 1, 10, QChar('0'));

                    foreach (QString s, gSerie)
                    {
                        QStringList sinfoSplit = s.split(";;;");
                        QString sInfo = sinfoSplit.value(0);
                        if(sInfo == id)
                        {
                            QString sSerie = sinfoSplit.value(1);
                            tAufbau.replace("{SERIE}", sSerie);
                            tAufbau.replace("{SERIES}", sSerie);
                        }
                    }
                    // für v1.1
                    tAufbau.replace("{TITEL}", EpisodeName);
                    tAufbau.replace("{STAFFEL1}", tStaffelB);
                    tAufbau.replace("{STAFFEL2}", tStaffelA);
                    tAufbau.replace("{EPISODE1}", tEpisodeB);
                    tAufbau.replace("{EPISODE2}", tEpisodeA);
                    tAufbau.replace("{JAHR}", year);

                    // ab 1.2
                    tAufbau.replace("{TITLE}", EpisodeName);
                    tAufbau.replace("{SEASON1}", tStaffelB);
                    tAufbau.replace("{SEASON2}", tStaffelA);
                    tAufbau.replace("{EPISODE1}", tEpisodeB);
                    tAufbau.replace("{EPISODE2}", tEpisodeA);
                    tAufbau.replace("{YEAR}", year);

                    return_episode = tAufbau + "." + dateiendung;
                }
            }
        }
    }
    return cleanName(return_episode);
}

QString MainWindow::episodeTVMAZE(QString staffel, QString episode, QString id, QString dateiendung, QString year)
{
    QString tAufbau = episoden_info_tvmaze;
    tAufbau.replace("{ID}", id);
    tAufbau.replace("{SEASON}", staffel);
    tAufbau.replace("{EPISODE}", episode);
    tAufbau.replace("{TVMAZE_API_KEY}", apiKey_tvMaze);
    tAufbau.replace("{TVLANG}", tvlang);

    ui->statusBar->showMessage(tAufbau);
    setLogText(tAufbau, "");

    QString return_episode;

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument tmdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = tmdb.object();

        QString name = getjson.value("name").toString();

        if(!QString(name).isEmpty())
        {
            //name = cleanName(name);

            QString tAufbau = serienAufbau;
            QString tEpisodeA = QString("%1").arg(episode.toInt(), 2, 10, QChar('0'));
            QString tEpisodeB = QString("%1").arg(episode.toInt(), 1, 10, QChar('0'));
            QString tStaffelA = QString("%1").arg(staffel.toInt(), 2, 10, QChar('0'));
            QString tStaffelB = QString("%1").arg(staffel.toInt(), 1, 10, QChar('0'));

            foreach (QString s, gSerie)
            {
                QStringList sinfoSplit = s.split(";;;");
                QString sInfo = sinfoSplit.value(0);
                if(sInfo == id)
                {
                    QString sSerie = sinfoSplit.value(1);
                    tAufbau.replace("{SERIE}", sSerie);
                    tAufbau.replace("{SERIES}", sSerie);
                }
            }

            // für v1.1
            tAufbau.replace("{TITEL}", name);
            tAufbau.replace("{STAFFEL1}", tStaffelB);
            tAufbau.replace("{STAFFEL2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{JAHR}", year);

            // ab 1.2
            tAufbau.replace("{TITLE}", name);
            tAufbau.replace("{SEASON1}", tStaffelB);
            tAufbau.replace("{SEASON2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{YEAR}", year);

            return_episode = tAufbau + "." + dateiendung;
        }
        else
        {
            return_episode = "0";
        }

    }
    return cleanName(return_episode);
}

QStringList MainWindow::episodeTVDBList(QString id, QString year)
{
    QStringList return_episode;

    sereID_tvdb = id;
    if(QString(xmlDoc_tvdb).isEmpty() || QString(sereID_tvdb) != id || QString(lastLn_tvdb) != QString(tvlang) || QString(getXML_tvdb) == "yes")
    {
        QString tAufbau = episoden_info_tvdb;
        tAufbau.replace("{TVDB_API_KEY}", apiKey_tvdb);
        tAufbau.replace("{ID}", id);
        tAufbau.replace("{TVLANG}", tvlang);

        setLogText(tAufbau, "");
        ui->statusBar->showMessage(tAufbau);

        QString myReplay = doDownload(tAufbau);

        if(!myReplay.isEmpty() && myReplay != "0")
        {
            xmlDoc_tvdb = myReplay;
        }
        else
        {
            xmlDoc_tvdb = "0";
        }
    }

    QDomDocument domXML;

    if(!domXML.setContent(xmlDoc_tvdb))
    {
        setLogText(tr("Das empfangene XML von thetvdb.com ist leer!"), "red");
        ui->statusBar->showMessage(tr("Das empfangene XML von thetvdb.com ist leer!"));
        return_episode << "0";
    }
    else
    {
        QDomElement root = domXML.firstChildElement();
        QDomNodeList serien = root.elementsByTagName("Episode");

        QString SeasonNumber;
        QString EpisodeName;
        QString EpisodeNumber;

        for(int i = 0; i < serien.count(); i++)
        {
            QDomNode snode = serien.at(i);
            if(snode.isElement())
            {
                QDomElement serie = snode.toElement();

                SeasonNumber = ListElements(serie, "SeasonNumber");
                EpisodeName = ListElements(serie, "EpisodeName");
                EpisodeNumber = ListElements(serie, "EpisodeNumber");

                if(SeasonNumber != "0")
                {
                    //EpisodeName = cleanName(EpisodeName);

                    QString tAufbau = serienAufbau;
                    QString tEpisodeA = QString("%1").arg(EpisodeNumber.toInt(), 2, 10, QChar('0'));
                    QString tEpisodeB = QString("%1").arg(EpisodeNumber.toInt(), 1, 10, QChar('0'));
                    QString tStaffelA = QString("%1").arg(SeasonNumber.toInt(), 2, 10, QChar('0'));
                    QString tStaffelB = QString("%1").arg(SeasonNumber.toInt(), 1, 10, QChar('0'));

                    foreach (QString s, gSerie)
                    {
                        QStringList sinfoSplit = s.split(";;;");
                        QString sInfo = sinfoSplit.value(0);
                        if(sInfo == id)
                        {
                            QString sSerie = sinfoSplit.value(1);
                            tAufbau.replace("{SERIE}", sSerie);
                            tAufbau.replace("{SERIES}", sSerie);
                        }
                    }
                    // für v1.1
                    tAufbau.replace("{TITEL}", EpisodeName);
                    tAufbau.replace("{STAFFEL1}", tStaffelB);
                    tAufbau.replace("{STAFFEL2}", tStaffelA);
                    tAufbau.replace("{EPISODE1}", tEpisodeB);
                    tAufbau.replace("{EPISODE2}", tEpisodeA);
                    tAufbau.replace("{JAHR}", year);

                    // ab 1.2
                    tAufbau.replace("{TITLE}", EpisodeName);
                    tAufbau.replace("{SEASON1}", tStaffelB);
                    tAufbau.replace("{SEASON2}", tStaffelA);
                    tAufbau.replace("{EPISODE1}", tEpisodeB);
                    tAufbau.replace("{EPISODE2}", tEpisodeA);
                    tAufbau.replace("{YEAR}", year);

                    return_episode << cleanName(tAufbau);
                }
            }
        }
    }
    return return_episode;
}

QString MainWindow::episodeOMDB(QString staffel, QString episode, QString id, QString dateiendung, QString year)
{
    QString tAufbau = episoden_info_omdb;
    tAufbau.replace("{ID}", id);
    tAufbau.replace("{SEASON}", staffel);
    tAufbau.replace("{EPISODE}", episode);
    tAufbau.replace("{TMDB_API_KEY}", apiKey_tmdb);
    tAufbau.replace("{TVLANG}", tvlang);

    ui->statusBar->showMessage(tAufbau);
    setLogText(tAufbau, "");

    QString return_episode;

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument omdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = omdb.object();

        QString name = getjson.value("Title").toString();

        if(!QString(name).isEmpty())
        {
            //name = cleanName(name);

            QString tAufbau = serienAufbau;
            QString tEpisodeA = QString("%1").arg(episode.toInt(), 2, 10, QChar('0'));
            QString tEpisodeB = QString("%1").arg(episode.toInt(), 1, 10, QChar('0'));
            QString tStaffelA = QString("%1").arg(staffel.toInt(), 2, 10, QChar('0'));
            QString tStaffelB = QString("%1").arg(staffel.toInt(), 1, 10, QChar('0'));

            foreach (QString s, gSerie)
            {
                QStringList sinfoSplit = s.split(";;;");
                QString sInfo = sinfoSplit.value(0);
                if(sInfo == id)
                {
                    QString sSerie = sinfoSplit.value(1);
                    tAufbau.replace("{SERIE}", sSerie);
                    tAufbau.replace("{SERIES}", sSerie);
                }
            }

            // für v1.1
            tAufbau.replace("{TITEL}", name);
            tAufbau.replace("{STAFFEL1}", tStaffelB);
            tAufbau.replace("{STAFFEL2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{JAHR}", year);

            // ab 1.2
            tAufbau.replace("{TITLE}", name);
            tAufbau.replace("{SEASON1}", tStaffelB);
            tAufbau.replace("{SEASON2}", tStaffelA);
            tAufbau.replace("{EPISODE1}", tEpisodeB);
            tAufbau.replace("{EPISODE2}", tEpisodeA);
            tAufbau.replace("{YEAR}", year);

            return_episode = tAufbau + "." + dateiendung;
        }
        else
        {
            return_episode = "0";
        }

    }
    return cleanName(return_episode);
}

void MainWindow::LoadSettings()
{
    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    windowSize = size();
    width = windowSize.width();
    height = windowSize.height();

    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 30;

    QString appPath = qApp->applicationDirPath();

    //QSettings setup("GrafSauger", "Episodiac");
    QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
    setup.beginGroup("MainWindow");
    QRect thisRect = setup.value("position", QRect(QPoint(x,y),QSize(1024,680))).toRect();
    setGeometry(thisRect);
    setup.endGroup();

    setup.beginGroup("Einstellungen");

    // rekusriver dateifilter laden
    QString load_regExExclude = setup.value("rekursivFilters").toString();
    if(!QString(load_regExExclude).isEmpty())
    {
        regExExcludeList = load_regExExclude.split(" ");
        regExExclude = regExExcludeList.join("|");
    }

    QString load_apiKey_tvdb = setup.value("tvdb_api_key").toString();
    QString load_apiKey_tmdb = setup.value("tmdb_api_key").toString();
    QString load_apiKey_tvmaze = setup.value("tvmaze_api_key").toString();
    QString load_apiKey_omdb = setup.value("omdb_api_key").toString();
    QString load_tvlang = setup.value("sprache_episoden").toString();
    QString load_regex_suche = setup.value("regex_suche").toString();
    QString load_regex2_suche = setup.value("regex2_suche").toString();
    QString load_serienAufbau = setup.value("serienAufbau").toString();
    QString load_OrdnerStruktur = setup.value("OrdnerStruktur").toString();
    int load_OrdnerStrukturCheck = setup.value("OrdnerStrukturCheck").toInt();

    QString load_episoden_suche_tvdb = setup.value("episoden_suche_tvdb").toString();
    QString load_episoden_info_tvdb = setup.value("episoden_info_tvdb").toString();
    QString load_episoden_suche_tmdb = setup.value("episoden_suche_tmdb").toString();
    QString load_episoden_info_tmdb = setup.value("episoden_info_tmdb").toString();
    QString load_episoden_suche_tvmaze = setup.value("episoden_suche_tvmaze").toString();
    QString load_episoden_info_tvmaze = setup.value("episoden_info_tvmaze").toString();
    QString load_episoden_suche_omdb = setup.value("episoden_suche_omdb").toString();
    QString load_episoden_info_omdb = setup.value("episoden_info_omdb").toString();

    QString load_episodiac_lang = setup.value("episodiac_lang").toString();
    if(!QString(load_episodiac_lang).isEmpty())
    {
        loadLanguage(load_episodiac_lang);
    }

    QString load_vFilters = setup.value("VideoFilters").toString();
    if(!QString(load_vFilters).isEmpty())
    {
        VideoFilters = load_vFilters.split(" ");
    }

    QString load_tvlangList = setup.value("tvlangList").toString();
    if(!QString(load_tvlangList).isEmpty())
    {
        tvlangList = load_tvlangList.split(" ");
    }

    if(!QString(load_apiKey_tvdb).isEmpty())
    {
        apiKey_tvdb = load_apiKey_tvdb;
    }

    if(!QString(load_apiKey_tmdb).isEmpty())
    {
        apiKey_tmdb = load_apiKey_tmdb;
    }

    if(!QString(load_apiKey_tvmaze).isEmpty())
    {
        apiKey_tvMaze = load_apiKey_tvmaze;
    }

    if(!QString(load_apiKey_omdb).isEmpty())
    {
        apiKey_OMDb = load_apiKey_omdb;
    }

    if(!QString(load_tvlang).isEmpty())
    {
        tvlang = load_tvlang;
    }

    if(!QString(load_regex_suche).isEmpty())
    {
        eRegEx = load_regex_suche;
    }

    if(!QString(load_regex2_suche).isEmpty())
    {
        eRegEx2 = load_regex2_suche;
    }

    if(!QString(load_serienAufbau).isEmpty())
    {
        serienAufbau = load_serienAufbau;
    }

    if(!QString(load_OrdnerStruktur).isEmpty())
    {
        OrdnerStruktur = load_OrdnerStruktur;
    }

    if(load_OrdnerStrukturCheck)
    {
        OrdnerStrukturCheck = 1;
    }

    if(!QString(load_episoden_suche_tvdb).isEmpty())
    {
        episoden_suche_tvdb = load_episoden_suche_tvdb;
    }

    if(!QString(load_episoden_info_tvdb).isEmpty())
    {
        episoden_info_tvdb = load_episoden_info_tvdb;
    }

    if(!QString(load_episoden_suche_tmdb).isEmpty())
    {
        episoden_suche_tmdb = load_episoden_suche_tmdb;
    }

    if(!QString(load_episoden_info_tmdb).isEmpty())
    {
        episoden_info_tmdb = load_episoden_info_tmdb;
    }

    if(!QString(load_episoden_suche_tvmaze).isEmpty())
    {
        episoden_suche_tvmaze = load_episoden_suche_tvmaze;
    }

    if(!QString(load_episoden_info_tvmaze).isEmpty())
    {
        episoden_info_tvmaze = load_episoden_info_tvmaze;
    }

    if(!QString(load_episoden_suche_omdb).isEmpty())
    {
        episoden_suche_omdb = load_episoden_suche_omdb;
    }

    if(!QString(load_episoden_info_omdb).isEmpty())
    {
        episoden_info_omdb = load_episoden_info_omdb;
    }
    setup.endGroup();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   //QSettings setup("GrafSauger", "Episodiac");
   QString appPath = qApp->applicationDirPath();
   QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
   setup.beginGroup("MainWindow");
   setup.setValue("position", this->geometry());
   setup.endGroup();
}

void MainWindow::moveEvent(QMoveEvent* event)
{
   QMainWindow::moveEvent(event);

   //QSettings setup("GrafSauger", "Episodiac");
   QString appPath = qApp->applicationDirPath();
   QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
   setup.beginGroup("MainWindow");
   setup.setValue("position", this->geometry());
   setup.endGroup();
}

void MainWindow::on_treeView1_clicked(const QModelIndex &index)
{
    QString sPath = dirmodel->fileInfo(index).absoluteFilePath();
    ui->treeView1->expand(dirmodel->setRootPath(sPath));
    setLogText(tr("Aktueller Arbeitspfad gesetzt: ") + sPath, "");
    gPath = sPath;

    ui->label->setText(tr("Zielpfad: ") + sPath);

    if(ui->pushButton_4->isChecked())
    {
        loadRekursiv();
    }
    else
    {
        ui->listView1->setModel(filemodel);
        ui->listView1->setRootIndex(filemodel->setRootPath(sPath));
        filemodel->setNameFilters(VideoFilters);
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    QString tv_suche = cleanSearch(ui->lineEdit->text());
    if(!QString(tv_suche).isEmpty())
    {
        LoadSettings();

        if(ui->comboBox->currentIndex() == 0)
        {
            sendRequest_tmdb(tv_suche);
        }
        else if(ui->comboBox->currentIndex() == 1)
        {
            sendRequest_tvdb(tv_suche);
        }
        else if(ui->comboBox->currentIndex() == 2)
        {
            sendRequest_tvmaze(tv_suche);
        }
        else if(ui->comboBox->currentIndex() == 3)
        {
            sendRequest_tvdb(tv_suche);
        }
        else
        {
            sendRequest_omdb(tv_suche);
        }
    }
    ui->lineEdit->selectAll();
}

void MainWindow::sendRequest_tmdb(const QString &tv_suche)
{
    LoadSettings();

    QString tAufbau = episoden_suche_tmdb;
    tAufbau.replace("{TVLANG}", tvlang);
    tAufbau.replace("{TMDB_API_KEY}", apiKey_tmdb);
    tAufbau.replace("{SEARCH}", tv_suche);

    setLogText(tAufbau, "");
    ui->statusBar->showMessage(tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument tmdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = tmdb.object();

        QJsonValue agentsArrayValue = getjson.value("results");
        QJsonArray agentsArray = agentsArrayValue.toArray();

        stringList->clear();
        mlist->setStringList(*stringList);

        double vDouble1 = getjson.value("total_results").toDouble();
        QString total_results = QString::number(vDouble1);

        if(QString(total_results) == "0")
        {
            stringList->append(tr("Nichts gefunden ..."));
            mlist->setStringList(*stringList);
            ui->statusBar->showMessage(tr("Nichts gefunden ..."));
        }
        else
        {
            foreach (const QJsonValue & v, agentsArray)
            {
                double vDouble = v.toObject().value("id").toDouble();
                QString serie_ID = QString::number(vDouble);

                QString serie_Name = v.toObject().value("name").toString();
                QString serie_Original = v.toObject().value("original_name").toString();
                QString serie_Datum = v.toObject().value("first_air_date").toString();

                // Für weitere Verwendung in die Liste damit
                gSerie << serie_ID + ";;;" + serie_Name;

                if(!QString(serie_Original).isEmpty())
                {
                    if(!QString(serie_Datum).isEmpty())
                    {
                        stringList->append("[" + serie_ID + "] " + serie_Name + " (" + serie_Original + " <" + serie_Datum + ">)");
                    }
                    else
                    {
                        stringList->append("[" + serie_ID + "] " + serie_Name + " (" + serie_Original + ")");
                    }
                }
                else
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name);
                }
            }
            mlist->setStringList(*stringList);
            ui->statusBar->showMessage(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."));
            setLogText(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."), "");
        }
    }
}

void MainWindow::sendRequest_tvdb(const QString &tv_suche)
{
    LoadSettings();

    QString tAufbau = episoden_suche_tvdb;
    tAufbau.replace("{TVLANG}", tvlang);
    tAufbau.replace("{SEARCH}", tv_suche);

    setLogText(tAufbau, "");
    ui->statusBar->showMessage(tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tvdbXml = myReplay;
        QDomDocument domXML;

        if(!domXML.setContent(tvdbXml))
        {
            ui->statusBar->showMessage(tr("XML Fehler!"));
        }
        else
        {
            QDomElement root = domXML.firstChildElement();

            stringList->clear();
            mlist->setStringList(*stringList);

            QString serie_ID;
            QString serie_Name;
            QString serie_Datum;

            QDomNodeList serien = root.elementsByTagName("Series");
            if(serien.count())
            {
                for(int i = 0; i < serien.count(); i++)
                {
                    QDomNode snode = serien.at(i);
                    if(snode.isElement())
                    {
                        QDomElement serie = snode.toElement();

                        serie_ID = ListElements(serie, "seriesid");
                        serie_Name = ListElements(serie, "SeriesName");
                        serie_Datum = ListElements(serie, "FirstAired");

                        // Für weitere Verwendung in die Liste damit
                        gSerie << serie_ID + ";;;" + serie_Name;
                    }

                    if(!QString(serie_Datum).isEmpty())
                    {
                        stringList->append("[" + serie_ID + "] " + serie_Name + " (<" + serie_Datum + ">)");
                    }
                    else
                    {
                        stringList->append("[" + serie_ID + "] " + serie_Name);
                    }
                }
                mlist->setStringList(*stringList);
                QString total_results = QString::number(serien.count());
                ui->statusBar->showMessage(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."));
                setLogText(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."), "");
            }
            else
            {
                stringList->append(tr("Nichts gefunden ..."));
                mlist->setStringList(*stringList);
                ui->statusBar->showMessage(tr("Nichts gefunden ..."));
            }
        }
    }
}

void MainWindow::sendRequest_tvmaze(const QString &tv_suche)
{
    LoadSettings();

    QString tAufbau = episoden_suche_tvmaze;
    tAufbau.replace("{SEARCH}", tv_suche);

    setLogText(tAufbau, "");
    ui->statusBar->showMessage(tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        stringList->clear();
        mlist->setStringList(*stringList);

        QString tvmazeJson = myReplay;

        QJsonDocument tvmaze = QJsonDocument::fromJson(tvmazeJson.toUtf8());
        QJsonArray getArray = tvmaze.array();

        int total_results = 0;
        foreach(const QJsonValue & v, getArray)
        {
            total_results++;

            QJsonObject getShow = v.toObject().value("show").toObject();

            double vDouble = getShow.value("id").toDouble();
            QString serie_ID = QString::number(vDouble);

            QString serie_Name = getShow.value("name").toString();
            QString serie_sprache = getShow.value("language").toString();
            QString serie_Datum = getShow.value("premiered").toString();

            gSerie << serie_ID + ";;;" + serie_Name;

            if(!QString(serie_Datum).isEmpty())
            {
                if(!QString(serie_sprache).isEmpty())
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name + " <" + serie_Datum + "> (" + serie_sprache + ")");
                }
                else
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name + " <" + serie_Datum + ">");
                }
            }
            else
            {
                if(!QString(serie_sprache).isEmpty())
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name + " (" + serie_sprache + ")");
                }
                else
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name);
                }
            }
        }

        mlist->setStringList(*stringList);
        ui->statusBar->showMessage(tr("Suche war erfolgreich! ") + QString::number(total_results) + tr(" Ergebnisse."));
        setLogText(tr("Suche war erfolgreich! ") + QString::number(total_results) + tr(" Ergebnisse."), "");
    }
}

void MainWindow::sendRequest_omdb(const QString &tv_suche)
{
    LoadSettings();

    QString tAufbau = episoden_suche_omdb;
    tAufbau.replace("{TVLANG}", tvlang);
    tAufbau.replace("{OMDB_API_KEY}", apiKey_OMDb);
    tAufbau.replace("{SEARCH}", tv_suche);

    setLogText(tAufbau, "");
    ui->statusBar->showMessage(tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument omdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = omdb.object();

        QJsonValue agentsArrayValue = getjson.value("Search");
        QJsonArray agentsArray = agentsArrayValue.toArray();

        stringList->clear();
        mlist->setStringList(*stringList);

        QString total_results = getjson.value("totalResults").toString();

        if(total_results.isEmpty())
        {
            stringList->append(tr("Nichts gefunden ..."));
            mlist->setStringList(*stringList);
            ui->statusBar->showMessage(tr("Nichts gefunden ..."));
        }
        else
        {
            foreach (const QJsonValue & v, agentsArray)
            {
                QString serie_ID = v.toObject().value("imdbID").toString();
                QString serie_Name = v.toObject().value("Title").toString();
                QString serie_Datum = v.toObject().value("Year").toString();

                // Für weitere Verwendung in die Liste damit
                gSerie << serie_ID + ";;;" + serie_Name;

                if(!QString(serie_Datum).isEmpty())
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name + " <" + serie_Datum + ">");
                }
                else
                {
                    stringList->append("[" + serie_ID + "] " + serie_Name);
                }
            }
            mlist->setStringList(*stringList);
            ui->statusBar->showMessage(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."));
            setLogText(tr("Suche war erfolgreich! ") + total_results + tr(" Ergebnisse."), "");
        }
    }
}

void MainWindow::renameEpisoden(const QString &rename)
{
    LoadSettings();

    if(ui->listView3->selectionModel()->selectedIndexes().empty())
    {
        QMessageBox::information(this, tr("TV Serien"), tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"));
        setLogText(tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"), "red");
    }
    else
    {
        QModelIndexList list = ui->listView3->selectionModel()->selectedIndexes();
        QStringList auswahl;
        foreach(const QModelIndex &index, list){
            auswahl.append(index.data(Qt::DisplayRole).toString());
        }

        QString serien_id;
        QString serien_year;
        QString series_name;
        if(ui->comboBox->currentIndex() == 4)
        {
            QRegularExpression de("\\[(tt\\d+)\\]", QRegularExpression::ExtendedPatternSyntaxOption);
            QRegularExpressionMatch match = de.match(auswahl[0]);
            if(match.hasMatch())
            {
                serien_id = match.captured(1);
            }

            QRegularExpression da("\\b(?:<)?(\\d{4})\\b", QRegularExpression::InvertedGreedinessOption);
            QRegularExpressionMatch match3 = da.match(auswahl[0]);
            if(match3.hasMatch())
            {
                serien_year = match3.captured(1);
            }

            QRegularExpression du("\\b(?:\\]\\s)(.*)(?:\\s\\()\\b", QRegularExpression::InvertedGreedinessOption);
            QRegularExpressionMatch match4 = du.match(auswahl[0]);
            if(match4.hasMatch())
            {
                series_name = match4.captured(1);
            }
        }
        else
        {
            QRegularExpression de("\\[(\\d+)\\]", QRegularExpression::ExtendedPatternSyntaxOption);
            QRegularExpressionMatch match = de.match(auswahl[0]);
            if(match.hasMatch())
            {
                serien_id = match.captured(1);
            }

            QRegularExpression da("\\b(?:<)?(\\d{4})(?:-)\\b", QRegularExpression::InvertedGreedinessOption);
            QRegularExpressionMatch match3 = da.match(auswahl[0]);
            if(match3.hasMatch())
            {
                serien_year = match3.captured(1);
            }

            QRegularExpression du("\\b(?:\\]\\s)(.*)(?:\\s\\()\\b", QRegularExpression::InvertedGreedinessOption);
            QRegularExpressionMatch match4 = du.match(auswahl[0]);
            if(match4.hasMatch())
            {
                series_name = match4.captured(1);
            }
        }

        if(!QString(serien_id).isEmpty())
        {
            renameList->clear();
            rlist->setStringList(*renameList);

            QStringList episode_list;
            if(ui->pushButton_4->isChecked())
            {
                for(int i = 0; i < dataList->count(); i++)
                {
                    episode_list << dataList->at(i);
                }
            }
            else
            {
                QModelIndex parentIndex = ui->listView1->rootIndex();
                int numRows = filemodel->rowCount(parentIndex);

                for(int row = 0; row < numRows; row++)
                {
                    QModelIndex index2 = filemodel->index(row, 0, parentIndex);
                    QString episode_datei = filemodel->data(index2, Qt::DisplayRole).toString();

                    episode_list << episode_datei;
                }
            }

            int cnt = 0;
            foreach(QString episode_datei, episode_list)
            {
                QString dateiendung;
                QRegularExpression en(".([a-zA-Z]{1,4})$", QRegularExpression::ExtendedPatternSyntaxOption);
                QRegularExpressionMatch match1 = en.match(episode_datei);
                if(match1.hasMatch())
                {
                    dateiendung = match1.captured(1);
                }

                QRegularExpression re(eRegEx, QRegularExpression::InvertedGreedinessOption | QRegularExpression::OptimizeOnFirstUsageOption);
                QRegularExpressionMatch match = re.match(episode_datei);
                if(!re.isValid())
                {
                    setLogText(tr("Kein gültiger RegEx! Bitte RegEx in den Einstellungen prüfen!"), "red");
                    QMessageBox messageBox(QMessageBox::Question, tr("RegEx"), tr("Kein gültiger RegEx!\n\nBitte RegEx in den Einstellungen prüfen!\n\nRegEx: ") + eRegEx, QMessageBox::Ok | QMessageBox::Abort, this);
                    messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                    messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                    int reply = messageBox.exec();
                    if(reply == QMessageBox::Abort)
                    {
                        break;
                    }
                    continue;
                }

                if(match.hasMatch())
                {
                    QString s_staffel = match.captured(1);
                    QString s_episode = match.captured(2);

                    if((QString(s_staffel).isEmpty() || QString(s_staffel) == "0") && (QString(s_episode).isEmpty() || QString(s_episode) == "0"))
                    {
                        setLogText(tr("Die Dateierkennung konnte weder Staffel noch Episode ermitteln. Staffel und Episode waren entweder leer oder 0!"), "red");
                        QMessageBox messageBox(QMessageBox::Question, tr("Dateierkennung"), tr("Die Dateierkennung konnte weder Staffel noch Episode ermitteln. Staffel und Episode waren entweder leer oder 0!\n\nDatei: ") + episode_datei + tr("\nStaffel: ") + s_staffel + tr("\nEpisode: ") + s_episode, QMessageBox::Ok | QMessageBox::Abort, this);
                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                        int reply = messageBox.exec();
                        if(reply == QMessageBox::Abort)
                        {
                            break;
                        }
                        cnt++;
                        continue;
                    }

                    if(QString(s_staffel).isEmpty())
                    {
                        s_staffel = "1";
                    }

                    if(!QString(s_episode).isEmpty())
                    {
                        QString episode_final;

                        if(ui->comboBox->currentIndex() == 0)
                        {
                            episode_final = episodeTMDB(s_staffel, s_episode, serien_id, dateiendung, serien_year);
                        }
                        else if(ui->comboBox->currentIndex() == 1)
                        {
                            episode_final = episodeTVDB(s_staffel, s_episode, serien_id, dateiendung, serien_year);
                        }
                        else if(ui->comboBox->currentIndex() == 2)
                        {
                            episode_final = episodeTVMAZE(s_staffel, s_episode, serien_id, dateiendung, serien_year);
                        }
                        else if(ui->comboBox->currentIndex() == 4)
                        {
                            episode_final = episodeOMDB(s_staffel, s_episode, serien_id, dateiendung, serien_year);
                        }

                        if(QString(episode_final) != "0")
                        {
                            if(QString(rename) == "true" && !QString(episode_final).isEmpty())
                            {
                                QString filePath1;
                                QString filePath2;
                                if(ui->pushButton_4->isChecked())
                                {
                                    filePath1 = gPathList.at(cnt);
                                }
                                else
                                {
                                    filePath1 = dirmodel->rootPath();
                                }

                                if(ui->pushButton_6->isChecked())
                                {
                                    filePath2 = gPath;
                                }
                                else
                                {
                                    if(!g_ZIEL.isEmpty())
                                    {
                                        filePath2 = g_ZIEL;
                                    }
                                    else
                                    {
                                        filePath2 = filePath1;
                                    }
                                }

                                if(OrdnerStrukturCheck)
                                {
                                    QString tAufbau = OrdnerStruktur;
                                    QString s_staffelA = QString("%1").arg(s_staffel.toInt(), 2, 10, QChar('0'));
                                    QString s_staffelB = QString("%1").arg(s_staffel.toInt(), 1, 10, QChar('0'));

                                    tAufbau.replace("{SERIES}", series_name);
                                    tAufbau.replace("{SEASON1}", s_staffelB);
                                    tAufbau.replace("{SEASON2}", s_staffelA);
                                    tAufbau.replace("{YEAR}", serien_year);

                                    tAufbau.replace("{SERIE}", series_name);
                                    tAufbau.replace("{STAFFEL1}", s_staffelB);
                                    tAufbau.replace("{STAFFEL2}", s_staffelA);
                                    tAufbau.replace("{JAHR}", serien_year);

                                    filePath2 = filePath2 + "/" + tAufbau;

                                    QDir sDir(filePath2);
                                    if(!sDir.exists())
                                    {
                                        setLogText(tr("Erstelle neuen Pfad: ") + filePath2, "green");

                                        if(!sDir.mkpath(filePath2))
                                        {
                                            setLogText(tr("FEHLER: Der Pfad: ") + filePath2 + tr(" konnte nicht erlstellt werden!"), "red");
                                            continue;
                                        }
                                    }
                                }

                                if(!QFile::exists(QString(filePath2 + "/" + episode_final)))
                                {
                                    if(QFile::rename(QString(filePath1 + "/" + episode_datei), QString(filePath2 + "/" + episode_final)))
                                    {
                                        ui->statusBar->showMessage("Rename: " + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final);
                                        setLogText(tr("Rename: ") + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final, "");
                                    }
                                    else
                                    {
                                        setLogText(tr("Rename von ") + episode_datei + tr(" in ") + episode_final + tr(" fehlgeschalgen!"), "red");
                                        ui->statusBar->showMessage(tr("Rename von ") + filePath1 + "/" + episode_datei), QString(filePath2 + "/" + episode_final + tr(" fehlgeschalgen!"));
                                        QMessageBox::warning(this, tr("Umbenennen fehlgeschlagen!"), tr("Umbenennen von ") + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final + tr(" fehlgeschalgen!"));
                                    }
                                }
                                else
                                {
                                    ui->statusBar->showMessage(tr("Rename Fehler: ") + episode_final + tr(" ... existiert bereits!"));
                                    setLogText(tr("Rename Fehler: ") + episode_final + tr(" ... existiert bereits!"), "red");
                                }
                            }
                            if(!QString(episode_final).isEmpty())
                            {
                                renameList->append(episode_final);
                                rlist->setStringList(*renameList);
                            }
                        }
                    }
                    else
                    {
                        setLogText(tr("RegEx konnte keine eindeutige Episode finden!"), "green");
                        QMessageBox messageBox(QMessageBox::Question, tr("Episode"), tr("RegEx konnte keine eindeutige Episode finden!\n\nBitte RegEx in den Einstellungen prüfen und ggf. ändern.\n\nDatei: ") + episode_datei, QMessageBox::Ok | QMessageBox::Abort, this);
                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                        int reply = messageBox.exec();
                        if(reply == QMessageBox::Abort)
                        {
                            break;
                        }
                        cnt++;
                        continue;
                    }
                }
                else
                {
                    setLogText(tr("Keine RegEx Erkennung möglich!"), "green");
                    QMessageBox messageBox(QMessageBox::Question, tr("RegEx Match"), tr("Keine RegEx Erkennung möglich!\nBitte ändern Sie den RegEx für folgende Datei:\n\n") + episode_datei, QMessageBox::Ok | QMessageBox::Abort, this);
                    messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                    messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                    int reply = messageBox.exec();
                    if(reply == QMessageBox::Abort)
                    {
                        break;
                    }
                    cnt++;
                    continue;
                }
                if(ui->comboBox->currentIndex() != 0)
                {
                    lastLn_tvdb = tvlang;
                }
                getXML_tvdb = "no";
                cnt++;
            }
            if(ui->pushButton_4->isChecked())
            {
                loadRekursiv();
            }
            getXML_tvdb = "yes";
            ui->statusBar->showMessage(tr("Episodiac bereit!"));
        }
    }
}

void MainWindow::renameEpisodenByNumber(const QString &rename)
{
    LoadSettings();

    if(ui->listView3->selectionModel()->selectedIndexes().empty())
    {
        QMessageBox::information(this, tr("TV Serien"), tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"));
        setLogText(tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"), "red");
    }
    else
    {
        QModelIndexList list = ui->listView3->selectionModel()->selectedIndexes();
        QStringList auswahl;
        foreach(const QModelIndex &index, list){
            auswahl.append(index.data(Qt::DisplayRole).toString());
        }

        QString serien_id;
        QRegularExpression de("\\[(\\d+)\\]", QRegularExpression::ExtendedPatternSyntaxOption);
        QRegularExpressionMatch match = de.match(auswahl[0]);
        if(match.hasMatch())
        {
            serien_id = match.captured(1);
        }

        QString serien_year;
        QRegularExpression da("\\b(?:<)?(\\d{4})(?:-)\\b", QRegularExpression::InvertedGreedinessOption);
        QRegularExpressionMatch match3 = da.match(auswahl[0]);
        if(match3.hasMatch())
        {
            serien_year = match3.captured(1);
        }

        QString series_name;
        QRegularExpression du("\\b(?:\\]\\s)(.*)(?:\\s\\()\\b", QRegularExpression::InvertedGreedinessOption);
        QRegularExpressionMatch match4 = du.match(auswahl[0]);
        if(match4.hasMatch())
        {
            series_name = match4.captured(1);
        }

        if(!QString(serien_id).isEmpty())
        {
            renameList->clear();
            rlist->setStringList(*renameList);

            QStringList episode_list;
            if(ui->pushButton_4->isChecked())
            {
                for(int i = 0; i < dataList->count(); i++)
                {
                    episode_list << dataList->at(i);
                }
            }
            else
            {
                QModelIndex parentIndex = ui->listView1->rootIndex();
                int numRows = filemodel->rowCount(parentIndex);

                for(int row = 0; row < numRows; row++)
                {
                    QModelIndex index2 = filemodel->index(row, 0, parentIndex);
                    QString episode_datei = filemodel->data(index2, Qt::DisplayRole).toString();

                    episode_list << episode_datei;
                }
            }

            int cnt = 0;

            // hole alle episoden
            QStringList episoden;
            episoden = episodeTVDBList(serien_id, serien_year);

            foreach(QString episode_datei, episode_list)
            {
                QString dateiendung;
                QRegularExpression en(".([a-zA-Z]{1,4})$", QRegularExpression::ExtendedPatternSyntaxOption);
                QRegularExpressionMatch match1 = en.match(episode_datei);
                if(match1.hasMatch())
                {
                    dateiendung = match1.captured(1);
                }

                QRegularExpression re(eRegEx2, QRegularExpression::InvertedGreedinessOption | QRegularExpression::OptimizeOnFirstUsageOption);
                QRegularExpressionMatch match = re.match(episode_datei);
                if(!re.isValid())
                {
                    setLogText(tr("Kein gültiger RegEx! Bitte RegEx in den Einstellungen prüfen!"), "red");
                    QMessageBox messageBox(QMessageBox::Question, tr("RegEx"), tr("Kein gültiger RegEx!\n\nBitte RegEx in den Einstellungen prüfen!\n\nRegEx: ") + eRegEx, QMessageBox::Ok | QMessageBox::Abort, this);
                    messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                    messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                    int reply = messageBox.exec();
                    if(reply == QMessageBox::Abort)
                    {
                        break;
                    }
                    cnt++;
                    continue;
                }

                if(match.hasMatch())
                {
                    QString s_episode = match.captured(1);

                    // haben wir eine saubere episodennummer?
                    QRegularExpression ie("^\\d*$");
                    QRegularExpressionMatch matchme = ie.match(s_episode);
                    if(!matchme.hasMatch())
                    {
                        setLogText(tr("Die Episodennummer enthält Buchstaben und konnte daher nicht sauber ausgelesen werden!"), "red");
                        QMessageBox messageBox(QMessageBox::Question, tr("Episodenerkennung"), tr("Die Episodennummer enthält Buchstaben und konnte daher nicht sauber ausgelesen werden!\n\nDatei: ") + episode_datei + tr("\nEpisode: ") + s_episode, QMessageBox::Ok | QMessageBox::Abort, this);
                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                        int reply = messageBox.exec();
                        if(reply == QMessageBox::Abort)
                        {
                            break;
                        }
                        cnt++;
                        continue;
                    }

                    if((QString(s_episode).isEmpty() || QString(s_episode) == "0"))
                    {
                        setLogText(tr("Die Dateierkennung konnte die Episode nicht ermitteln. Episode war entweder leer oder 0!"), "red");
                        QMessageBox messageBox(QMessageBox::Question, tr("Dateierkennung"), tr("Die Dateierkennung konnte die Episode nicht ermitteln. Episode war entweder leer oder 0!\n\nDatei: ") + episode_datei + tr("\nEpisode: ") + s_episode, QMessageBox::Ok | QMessageBox::Abort, this);
                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                        int reply = messageBox.exec();
                        if(reply == QMessageBox::Abort)
                        {
                            break;
                        }
                        cnt++;
                        continue;
                    }

                    if(!QString(s_episode).isEmpty())
                    {
                        if(!episoden.isEmpty() && episoden.at(0) != "0")
                        {
                            QString episode_final;
                            QString s_episode2 = QString("%1").arg(s_episode.toInt(), 1, 10, QChar('0'));

                            int epi_nummer = s_episode2.toInt() - 1;
                            if(epi_nummer < 0)
                            {
                                epi_nummer = 0;
                            }

                            if(episoden.count() >= epi_nummer)
                            {
                                episode_final = episoden.at(epi_nummer) + "." + dateiendung;
                            }
                            else
                            {
                                cnt++;
                                continue;
                            }

                            if(QString(rename) == "true")
                            {
                                QString filePath1;
                                QString filePath2;
                                if(ui->pushButton_4->isChecked())
                                {
                                    filePath1 = gPathList.at(cnt);
                                }
                                else
                                {
                                    filePath1 = dirmodel->rootPath();
                                }

                                if(ui->pushButton_6->isChecked())
                                {
                                    filePath2 = gPath;
                                }
                                else
                                {
                                    if(!g_ZIEL.isEmpty())
                                    {
                                        filePath2 = g_ZIEL;
                                    }
                                    else
                                    {
                                        filePath2 = filePath1;
                                    }
                                }

                                if(OrdnerStrukturCheck)
                                {
                                    QRegularExpression reE(eRegEx, QRegularExpression::InvertedGreedinessOption | QRegularExpression::OptimizeOnFirstUsageOption);
                                    QRegularExpressionMatch match = reE.match(episode_datei);
                                    if(!reE.isValid())
                                    {
                                        setLogText(tr("Kein gültiger RegEx! Bitte RegEx in den Einstellungen prüfen!"), "red");
                                        QMessageBox messageBox(QMessageBox::Question, tr("RegEx"), tr("Kein gültiger RegEx!\n\nBitte RegEx in den Einstellungen prüfen!\n\nRegEx: ") + eRegEx, QMessageBox::Ok | QMessageBox::Abort, this);
                                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                                        int reply = messageBox.exec();
                                        if(reply == QMessageBox::Abort)
                                        {
                                            break;
                                        }
                                        cnt++;
                                        continue;
                                    }

                                    if(match.hasMatch())
                                    {
                                        QString s_staffel = match.captured(1);
                                        QString s_episode = match.captured(2);

                                        if((QString(s_staffel).isEmpty() || QString(s_staffel) == "0") && (QString(s_episode).isEmpty() || QString(s_episode) == "0"))
                                        {
                                            setLogText(tr("Die Dateierkennung konnte weder Staffel noch Episode ermitteln. Staffel und Episode waren entweder leer oder 0!"), "red");
                                            QMessageBox messageBox(QMessageBox::Question, tr("Dateierkennung"), tr("Die Dateierkennung konnte weder Staffel noch Episode ermitteln. Staffel und Episode waren entweder leer oder 0!\n\nDatei: ") + episode_datei + tr("\nStaffel: ") + s_staffel + tr("\nEpisode: ") + s_episode, QMessageBox::Ok | QMessageBox::Abort, this);
                                            messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                                            messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                                            int reply = messageBox.exec();
                                            if(reply == QMessageBox::Abort)
                                            {
                                                break;
                                            }
                                            cnt++;
                                            continue;
                                        }

                                        QString tAufbau = OrdnerStruktur;
                                        QString s_staffelA = QString("%1").arg(s_staffel.toInt(), 2, 10, QChar('0'));
                                        QString s_staffelB = QString("%1").arg(s_staffel.toInt(), 1, 10, QChar('0'));

                                        tAufbau.replace("{SERIES}", series_name);
                                        tAufbau.replace("{SEASON1}", s_staffelB);
                                        tAufbau.replace("{SEASON2}", s_staffelA);
                                        tAufbau.replace("{YEAR}", serien_year);

                                        tAufbau.replace("{SERIE}", series_name);
                                        tAufbau.replace("{STAFFEL1}", s_staffelB);
                                        tAufbau.replace("{STAFFEL2}", s_staffelA);
                                        tAufbau.replace("{JAHR}", serien_year);

                                        filePath2 = filePath2 + "/" + tAufbau;

                                        QDir sDir(filePath2);
                                        if(!sDir.exists())
                                        {
                                            setLogText(tr("Erstelle neuen Pfad: ") + filePath2, "green");

                                            if(!sDir.mkpath(filePath2))
                                            {
                                                setLogText(tr("FEHLER: Der Pfad: ") + filePath2 + tr(" konnte nicht erlstellt werden!"), "red");
                                                continue;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        cnt++;
                                        continue;
                                    }
                                }

                                if(!QFile::exists(QString(filePath2 + "/" + episode_final)))
                                {
                                    if(QFile::rename(QString(filePath1 + "/" + episode_datei), QString(filePath2 + "/" + episode_final)))
                                    {
                                        ui->statusBar->showMessage("Rename: " + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final);
                                        setLogText(tr("Rename: ") + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final, "");
                                    }
                                    else
                                    {
                                        setLogText(tr("Rename von ") + episode_datei + tr(" in ") + episode_final + tr(" fehlgeschalgen!"), "red");
                                        ui->statusBar->showMessage(tr("Rename von ") + filePath1 + "/" + episode_datei), QString(filePath2 + "/" + episode_final + tr(" fehlgeschalgen!"));
                                        QMessageBox::warning(this, tr("Umbenennen fehlgeschlagen!"), tr("Umbenennen von ") + filePath1 + "/" + episode_datei + " >> " + filePath2 + "/" + episode_final + tr(" fehlgeschalgen!"));
                                    }
                                }
                                else
                                {
                                    ui->statusBar->showMessage(tr("Rename Fehler: ") + episode_final + tr(" ... existiert bereits!"));
                                    setLogText(tr("Rename Fehler: ") + episode_final + tr(" ... existiert bereits!"), "red");
                                }
                            }
                            if(!episoden.isEmpty() && episoden.at(0) != "0")
                            {
                                renameList->append(episode_final);
                                rlist->setStringList(*renameList);
                            }
                        }
                    }
                    else
                    {
                        setLogText(tr("RegEx konnte keine eindeutige Episode finden!"), "green");
                        QMessageBox messageBox(QMessageBox::Question, tr("Episode"), tr("RegEx konnte keine eindeutige Episode finden!\n\nBitte RegEx in den Einstellungen prüfen und ggf. ändern.\n\nDatei: ") + episode_datei, QMessageBox::Ok | QMessageBox::Abort, this);
                        messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                        messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                        int reply = messageBox.exec();
                        if(reply == QMessageBox::Abort)
                        {
                            break;
                        }
                        cnt++;
                        continue;
                    }
                }
                else
                {
                    setLogText(tr("Keine RegEx Erkennung möglich!"), "green");
                    QMessageBox messageBox(QMessageBox::Question, tr("RegEx Match"), tr("Keine RegEx Erkennung möglich!\nBitte ändern Sie den RegEx für folgende Datei:\n\n") + episode_datei, QMessageBox::Ok | QMessageBox::Abort, this);
                    messageBox.setButtonText(QMessageBox::Ok, tr("OK"));
                    messageBox.setButtonText(QMessageBox::Abort, tr("Abbrechen"));
                    int reply = messageBox.exec();
                    if(reply == QMessageBox::Abort)
                    {
                        break;
                    }
                    cnt++;
                    continue;
                }
                if(ui->comboBox->currentIndex() != 0)
                {
                    lastLn_tvdb = tvlang;
                }
                getXML_tvdb = "no";
                cnt++;
            }
            if(ui->pushButton_4->isChecked())
            {
                loadRekursiv();
            }
            getXML_tvdb = "yes";
            ui->statusBar->showMessage(tr("Episodiac bereit!"));
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->comboBox->currentIndex() == 3)
    {
        renameEpisodenByNumber("false");
    }
    else
    {
        renameEpisoden("false");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox messageBox(QMessageBox::Question, tr("TV Serien"), tr("Sollen die Dateien wirklich umbenannt werden?"), QMessageBox::Yes | QMessageBox::No, this);
    messageBox.setButtonText(QMessageBox::Yes, tr("Ja"));
    messageBox.setButtonText(QMessageBox::No, tr("Nein"));
    int reply = messageBox.exec();
    if(reply == QMessageBox::Yes)
    {
        if(ui->comboBox->currentIndex() == 3)
        {
            renameEpisodenByNumber("true");
        }
        else
        {
            renameEpisoden("true");
        }
    }
}

void MainWindow::on_listView1_doubleClicked(const QModelIndex &index)
{
    if(!ui->pushButton_4->isChecked())
    {
        QString datei = filemodel->data(index, Qt::DisplayRole).toString();
        QString startMe = dirmodel->rootPath() + "/" + datei;
        QDesktopServices::openUrl(QUrl::fromLocalFile(startMe));
    }
}

void MainWindow::on_actionEinstellungen_triggered()
{
    Einstellungen mEinstellungen;
    mEinstellungen.setWindowTitle(tr("Einstellungen"));
    mEinstellungen.setModal(true);
    mEinstellungen.setWindowFlags(Qt::Window);
    mEinstellungen.exec();
}

void MainWindow::on_actionInfo_triggered()
{
    Info mInfo;
    mInfo.setWindowTitle(tr("Info"));
    mInfo.setModal(true);
    mInfo.setWindowFlags(Qt::Window);
    mInfo.exec();
}

void MainWindow::on_listView3_doubleClicked()
{
    QModelIndexList list = ui->listView3->selectionModel()->selectedIndexes();
    QStringList auswahl;
    foreach(const QModelIndex &index, list){
        auswahl.append(index.data(Qt::DisplayRole).toString());
    }

    QString serien_id;
    QString idRegEx;
    if(ui->comboBox->currentIndex() == 4)
    {
        idRegEx = "\\[(tt\\d+)\\]";
    }
    else
    {
        idRegEx = "\\[(\\d+)\\]";
    }

    QRegularExpression de(idRegEx, QRegularExpression::ExtendedPatternSyntaxOption);
    QRegularExpressionMatch match = de.match(auswahl[0]);
    if(match.hasMatch())
    {
        serien_id = match.captured(1);

        if(ui->comboBox->currentIndex() == 0)
        {
            QDesktopServices::openUrl(QUrl("https://www.themoviedb.org/tv/" + serien_id));
            ui->statusBar->showMessage(tr("Öffne: https://www.themoviedb.org/tv/") + serien_id, 5000);
        }
        else if(ui->comboBox->currentIndex() == 1)
        {
            if(tvlang == "de")
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id + "&lid=14"));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id + "&lid=14", 5000);
            }
            else if(tvlang == "en")
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id + "&lid=7"));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id + "&lid=7", 5000);
            }
            else
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id, 5000);
            }
        }
        else if(ui->comboBox->currentIndex() == 2)
        {
            QDesktopServices::openUrl(QUrl("http://www.tvmaze.com/shows/" + serien_id));
            ui->statusBar->showMessage(tr("Öffne: http://www.tvmaze.com/shows/") + serien_id, 5000);
        }
        else if(ui->comboBox->currentIndex() == 3)
        {
            if(tvlang == "de")
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id + "&lid=14"));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id + "&lid=14", 5000);
            }
            else if(tvlang == "en")
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id + "&lid=7"));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id + "&lid=7", 5000);
            }
            else
            {
                QDesktopServices::openUrl(QUrl("http://thetvdb.com/?tab=series&id=" + serien_id));
                ui->statusBar->showMessage(tr("Öffne: http://thetvdb.com/?tab=series&id=") + serien_id, 5000);
            }
        }
        else if(ui->comboBox->currentIndex() == 4)
        {
            QDesktopServices::openUrl(QUrl("http://www.imdb.com/title/" + serien_id));
            ui->statusBar->showMessage(tr("Öffne: http://www.imdb.com/title/") + serien_id, 5000);
        }
    }
}

void MainWindow::clearList()
{
    stringList->clear();
    mlist->setStringList(*stringList);
}

void MainWindow::clearList2()
{
    dataList->clear();
    dlist->setStringList(*dataList);

    QString sPath = dirmodel->fileInfo(ui->treeView1->currentIndex()).absoluteFilePath();
    ui->listView1->setModel(filemodel);
    ui->listView1->setRootIndex(filemodel->setRootPath(sPath));
    filemodel->setNameFilters(VideoFilters);
}

void MainWindow::on_actionDeutsch_triggered()
{
    loadLanguage("de");
    saveLanguageSettings("de");
}

void MainWindow::on_actionEnglisch_triggered()
{
    loadLanguage("en");
    saveLanguageSettings("en");
}

void MainWindow::saveLanguageSettings(QString lang)
{
    QSettings setup("GrafSauger", "Episodiac");
    setup.beginGroup("Einstellungen");
    setup.setValue("episodiac_lang", lang);
    setup.endGroup();
}

void MainWindow::slotLanguageChanged(QAction* action)
{
    if(0 != action)
    {
        loadLanguage(action->data().toString());
        //setWindowIcon(action->icon());
    }
}

void MainWindow::switchTranslator(QTranslator& translator, const QString& filename)
{
    qApp->removeTranslator(&translator);

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");

    if(translator.load(filename, m_langPath))
    {
        qApp->installTranslator(&translator);
    }
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(m_translator, QString("lang_%1.qm").arg(rLanguage));
        ui->statusBar->showMessage(tr("Aktuelle Sprache wurde zu %1 (%2) geändert!").arg(languageName).arg(rLanguage));
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if(0 != event)
    {
        switch(event->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
            break;
            case QEvent::LocaleChange:
            {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                loadLanguage(locale);
            }
            break;
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::on_pushButton_3_clicked()
{
    if(ui->listView3->selectionModel()->selectedIndexes().empty())
    {
        QMessageBox::information(this, tr("Cover Download"), tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"));
        setLogText(tr("Bitte zuerst eine TV Serie suchen und einen Namen auswählen!"), "red");
    }
    else
    {
        QModelIndexList list = ui->listView3->selectionModel()->selectedIndexes();
        QStringList auswahl;
        foreach(const QModelIndex &index, list){
            auswahl.append(index.data(Qt::DisplayRole).toString());
        }

        QString serien_id;

        if(ui->comboBox->currentIndex() == 4)
        {
            QRegularExpression de("\\[(tt\\d+)\\]", QRegularExpression::ExtendedPatternSyntaxOption);
            QRegularExpressionMatch match = de.match(auswahl[0]);
            if (match.hasMatch())
            {
                serien_id = match.captured(1);
            }
        }
        else
        {
            QRegularExpression de("\\[(\\d+)\\]", QRegularExpression::ExtendedPatternSyntaxOption);
            QRegularExpressionMatch match = de.match(auswahl[0]);
            if (match.hasMatch())
            {
                serien_id = match.captured(1);
            }
        }

        if(!serien_id.isEmpty())
        {
            if(ui->comboBox->currentIndex() == 0)
            {
                // lade bildmaterial von tmdb.org
                loadCover_tmdb(serien_id);
            }
            else if(ui->comboBox->currentIndex() == 1)
            {
                // lade cover von thetvdb.com
                loadCover_tvdb(serien_id);
            }
            else if(ui->comboBox->currentIndex() == 2)
            {
                // lade cover von tvmaze
                QMessageBox::information(this, tr("Cover Download"), tr("Cover Download von TVmaze.com wird leider nicht unterstützt!"));
                setLogText(tr("Cover Download von TVmaze.com wird leider nicht unterstützt!"), "green");
            }
            else if(ui->comboBox->currentIndex() == 3)
            {
                // lade cover von thetvdb.com
                loadCover_tvdb(serien_id);
            }
            else if(ui->comboBox->currentIndex() == 4)
            {
                // lade cover von omdb
                QMessageBox::information(this, tr("Cover Download"), tr("Cover Download von OMDbapi.com wird leider nicht unterstützt!"));
                setLogText(tr("Cover Download von OMDbapi.com wird leider nicht unterstützt!"), "green");
            }
        }
        else
        {
            QMessageBox::information(this, tr("Cover Download"), tr("Die Serien ID konnte nicht ermittel werden!"));
            setLogText(tr("Die Serien ID konnte nicht ermittel werden!"), "red");
        }
    }
}

void MainWindow::loadCover_tmdb(QString serienID)
{
    LoadSettings();

    QString tAufbau = "http://api.tmdb.org/3/tv/" + serienID + "?api_key=" + apiKey_tmdb + "&language=" + tvlang;
    QString tPicUrl = "http://image.tmdb.org/t/p/original";

    setLogText(tr("Lade Seriendaten von ") + tAufbau, "");
    ui->statusBar->showMessage(tr("Lade Seriendaten von ") + tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tmdbJson = myReplay;

        QJsonDocument tmdb = QJsonDocument::fromJson(tmdbJson.toUtf8());
        QJsonObject getjson = tmdb.object();

        QString s_name = getjson.value("name").toString();
        QString s_backdrop_path = getjson.value("backdrop_path").toString();
        QString s_poster_path = getjson.value("poster_path").toString();

        if(!s_backdrop_path.isEmpty())
        {
            savePicture(tPicUrl, s_backdrop_path, s_name + " - Fanart");
        }
        if(!s_poster_path.isEmpty())
        {
            savePicture(tPicUrl, s_poster_path, s_name + " - Cover");
        }

        QJsonValue agentsArrayValue = getjson.value("seasons");
        QJsonArray agentsArray = agentsArrayValue.toArray();

        foreach (const QJsonValue & v, agentsArray)
        {
            QString poster_path = v.toObject().value("poster_path").toString();
            double season_number = v.toObject().value("season_number").toDouble();

            if(!poster_path.isEmpty())
            {
                savePicture(tPicUrl, poster_path, s_name + " - S" + QString::number(season_number) + " - Cover");
            }
        }
    }
}

void MainWindow::loadCover_tvdb(QString serienID)
{
    LoadSettings();

    QString tAufbau = "http://thetvdb.com/api/" + apiKey_tvdb + "/series/" + serienID + "/" + tvlang + ".xml";
    QString tPicUrl = "http://thetvdb.com/banners/";

    setLogText(tr("Lade Seriendaten von ") + tAufbau, "");
    ui->statusBar->showMessage(tr("Lade Seriendaten von ") + tAufbau);

    QString myReplay = doDownload(tAufbau);

    if(!myReplay.isEmpty() && myReplay != "0")
    {
        QString tvdbXml = myReplay;
        QDomDocument domXML;

        if(!domXML.setContent(tvdbXml))
        {
            setLogText(tr("XML Fehler!"), "red");
            ui->statusBar->showMessage(tr("XML Fehler!"));
        }
        else
        {
            QDomElement root = domXML.firstChildElement();

            QString s_name;
            QString banner;
            QString fanart;
            QString poster;

            QDomNodeList serien = root.elementsByTagName("Series");
            if(serien.count())
            {
                for(int i = 0; i < serien.count(); i++)
                {
                    QDomNode snode = serien.at(i);
                    if(snode.isElement())
                    {
                        QDomElement serie = snode.toElement();

                        s_name = ListElements(serie, "SeriesName");

                        if(!s_name.isEmpty())
                        {
                            banner = ListElements(serie, "banner");
                            fanart = ListElements(serie, "fanart");
                            poster = ListElements(serie, "poster");

                            if(!banner.isEmpty())
                            {
                                savePicture(tPicUrl, banner, s_name + " - Banner");
                            }
                            if(!fanart.isEmpty())
                            {
                                savePicture(tPicUrl, fanart, s_name + " - Fanart");
                            }
                            if(!poster.isEmpty())
                            {
                                savePicture(tPicUrl, poster, s_name + " - Cover");
                            }
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::savePicture(QString url, QString bild, QString name)
{
    if(gPath.isEmpty())
    {
        gPath = QDir::homePath();
    }

    // lade bild
    QString download;
    if(!bild.isEmpty())
    {
        download = url + bild;
    }
    else
    {
        download = url;
    }
    setLogText(tr("Lade Bild: ") + download, "");
    ui->statusBar->showMessage(tr("Lade Bild: ") + download);
    QByteArray cover = doCoverDownload(download);

    if(cover.size() > 0)
    {
        QFile file(gPath + "/" + name + " (" + tvlang + ") " + ".jpg");
        if(file.exists())
        {
            QMessageBox messageBox(QMessageBox::Question, tr("Cover Download"), tr("Die Datei <b>") + gPath + "/" + name + " (" + tvlang + ") " + tr(".jpg</b> existiert bereits. Soll sie überschrieben werden?"), QMessageBox::Yes | QMessageBox::No, this);
            messageBox.setButtonText(QMessageBox::Yes, tr("Ja"));
            messageBox.setButtonText(QMessageBox::No, tr("Nein"));
            int reply = messageBox.exec();
            if(reply == QMessageBox::Yes)
            {
                if(file.open(QIODevice::WriteOnly) && file.isWritable())
                {
                    setLogText(tr("Überschreibe Bild: ") + gPath + "/" + name + " (" + tvlang + ") " + ".jpg", "");
                    ui->statusBar->showMessage(tr("Überschreibe Bild: ") + gPath + "/" + name + " (" + tvlang + ") " + ".jpg");
                    file.write(cover);
                }
                else
                {
                    setLogText(tr("Datei kann nicht zum schreiben geöffnet werden!"), "red");
                    ui->statusBar->showMessage(tr("Datei kann nicht zum schreiben geöffnet werden!"));
                }
            }
        }
        else
        {
            if(file.open(QIODevice::WriteOnly) && file.isWritable())
            {
                setLogText(tr("Speichere Bild: ") + gPath + "/" + name + " (" + tvlang + ") " + ".jpg", "");
                ui->statusBar->showMessage(tr("Speichere Bild: ") + gPath + "/" + name + " (" + tvlang + ") " + ".jpg");
                file.write(cover);
            }
            else
            {
                setLogText(tr("Datei kann nicht zum schreiben geöffnet werden!"), "red");
                ui->statusBar->showMessage(tr("Datei kann nicht zum schreiben geöffnet werden!"));
            }
        }
        file.close();
    }
    else
    {
        setLogText(tr("Cover Download fehlgeschlagen!"), "red");
        ui->statusBar->showMessage(tr("Cover Download fehlgeschlagen!"));
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    if(ui->pushButton_4->isChecked())
    {
        QMessageBox messageBox(QMessageBox::Question, tr("Rekursiv laden"), tr("Dateien aus allen vorhandenen Unterverzeichnissen zu laden kann ggf. sehr lange dauern und die App wird nicht reagieren, bis alles geladen wurde!\n\nSoll wirklich rekursiv geladen werden?"), QMessageBox::Yes | QMessageBox::No, this);
        messageBox.setButtonText(QMessageBox::Yes, tr("Ja"));
        messageBox.setButtonText(QMessageBox::No, tr("Nein"));
        int reply = messageBox.exec();
        if(reply == QMessageBox::Yes)
        {
            loadRekursiv();
            ui->pushButton_6->setEnabled(true);
        }
        else
        {
            ui->pushButton_4->setChecked(false);
            ui->pushButton_6->setEnabled(false);
            ui->pushButton_6->setChecked(false);
            ui->pushButton_6->setStyleSheet("");
            ui->label->setText(tr("Zielpfad: ") + gPath);
            g_ROOT = 0;
        }
    }
    else
    {
        ui->pushButton_4->setStyleSheet("");
        ui->pushButton_6->setEnabled(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_6->setStyleSheet("");
        ui->label->setText(tr("Zielpfad: ") + gPath);
        g_ROOT = 0;
    }
}

void MainWindow::loadRekursiv()
{
    if(ui->pushButton_4->isChecked())
    {
        ui->pushButton_4->setStyleSheet("background-color: blue; color: white");

        ui->listView1->setModel(dlist);

        dataList->clear();
        dlist->setStringList(*dataList);

        gPathList.clear();
        int cnt = 0;
        QDirIterator sub(gPath, VideoFilters, QDir::Files, QDirIterator::Subdirectories);
        while(sub.hasNext())
        {
            setLogText(tr("Lade: ") + sub.next(), "black");

            QRegularExpression de(regExExclude, QRegularExpression::OptimizeOnFirstUsageOption);
            QRegularExpressionMatch match = de.match(sub.fileName());
            if(!match.hasMatch())
            {
                dataList->append(sub.fileName());
                gPathList << sub.fileInfo().absolutePath();
                cnt++;
            }
            else
            {
                setLogText(tr("Vom Filter ausgeschlossen: ") + sub.fileName(), "green");
            }
        }
        dlist->setStringList(*dataList);
        setLogText(tr("Dateien geladen: ") + QString::number(cnt), "black");
        ui->statusBar->showMessage(tr("Dateien geladen: ") + QString::number(cnt));
    }
    else
    {
        ui->pushButton_4->setStyleSheet("");
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    QString homePath;
    if(g_ZIEL.isEmpty())
    {
        homePath = QDir::homePath();
    }
    else
    {
        homePath = g_ZIEL;
    }

    QString ziel = QFileDialog::getExistingDirectory(this, tr("Speicherziel"), homePath, QFileDialog::ShowDirsOnly);

    if(!ziel.isEmpty())
    {
        if(QDir(ziel).exists())
        {
            QFileInfo meinZiel(QFileInfo(ziel).path());
            if(meinZiel.isWritable())
            {
                g_ZIEL = ziel;
                setLogText(tr("Neues Ziel: ") + ziel, "");
                ui->statusBar->showMessage(tr("Neues Ziel: ") + ziel);

                ui->label->setText(tr("Zielpfad: ") + g_ZIEL);

                if(ui->pushButton_6->isChecked())
                {
                    g_ROOT = 0;
                    ui->pushButton_6->setChecked(false);
                    ui->pushButton_6->setStyleSheet("");
                }
            }
            else
            {
                setLogText(ziel + tr(" ... hat Schreibschutz!"), "red");
                ui->statusBar->showMessage(ziel + tr(" ... hat Schreibschutz!"));
            }
        }
        else
        {
            setLogText(ziel + tr(" ... existiert nicht!"), "red");
            ui->statusBar->showMessage(ziel + tr(" ... existiert nicht!"));
        }
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    if(ui->pushButton_6->isChecked())
    {
        g_ROOT = 1;
        ui->pushButton_6->setStyleSheet("background-color: green; color: white");
        ui->label->setText(tr("Zielpfad: ") + gPath);

        // custom ziel button deaktivieren
        ui->pushButton_5->setEnabled(false);
    }
    else
    {
        g_ROOT = 0;
        ui->pushButton_6->setStyleSheet("");

        // custom ziel button aktivieren
        ui->pushButton_5->setEnabled(true);
    }
}
