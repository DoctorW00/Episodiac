#include "einstellungen.h"
#include "ui_einstellungen.h"
#include "mainwindow.h"

Einstellungen::Einstellungen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Einstellungen)
{
    ui->setupUi(this);

    ui->plainTextEdit->setStyleSheet("background: transparent;");
    ui->plainTextEdit_2->setStyleSheet("background: transparent;");
    //ui->pushButton->setStyleSheet("background-image: url(':/bilder/icons/save.png'); background-repeat: no-repeat;");
    //ui->pushButton_2->setStyleSheet("background-image: url(':/bilder/icons/close.png'); background-repeat: no-repeat;");
    ui->pushButton->setIcon(QIcon(":/bilder/icons/save.png"));
    ui->pushButton_2->setIcon(QIcon(":/bilder/icons/close.png"));
    ui->pushButton_3->setIcon(QIcon(":/bilder/icons/add.png"));
    ui->pushButton_4->setIcon(QIcon(":/bilder/icons/remove.png"));
    //ui->pushButton_5->setIcon(QIcon(":/bilder/icons/add.png"));
    //ui->pushButton_6->setIcon(QIcon(":/bilder/icons/remove.png"));

    ui->pushButton_5->setIcon(QIcon(":/bilder/icons/thetvdb.png"));
    ui->pushButton_6->setIcon(QIcon(":/bilder/icons/tmdb.png"));
    ui->pushButton_7->setIcon(QIcon(":/bilder/icons/tvmaze.png"));
    ui->pushButton_8->setIcon(QIcon(":/bilder/icons/omdb.png"));

    // neue sprache nur 2 zweichen und nur lower case buchstaben
    ui->lineEdit_5->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-z]{2}"), this));

    QString apiKey_tvdb = "1D62F2F90030C444";
    //QString apiKey_tmdb = "57983e31fb435df4df77afb854740ea9";
    QString apiKey_tmdb = "f7f51775877e0bb6703520952b3c7840";
    QString apiKey_tvMaze = "";

    QString tvlang = "de";
    QStringList tvlangList = (QStringList() << "de" << "en");

    //QString eRegEx = "[sS](\\d+)[eE](\\d+)";
    //QString eRegEx = "[sS](\\d+)[eE](\\d+)|[SstT](\\d+).*[epiEPI]_(\\d+)|(\\d+)[xX](\\d+)|([0-9]){1,2}([0-9]){1,2}";
    //QString eRegEx = "(?:[sS]|[SstT]|[a^])?(\\d{1,2})(?:[xX]|[eE]|[.*epiEPI_]|[\\d{4}])(\\d{1,3})";
    //QString eRegEx = "\\b(?:[sS])?(\\d{1,2})(?:[xX]|[eE])?(\\d{1,2})\\b";
    QString eRegEx = "\\b(?:[a-zA-Z]*)(?:[sS])?(\\d{1,2})(?:[xX]|[eE])?(\\d{1,2})(?:[a-zA-Z]*)\\b";
    QString eRegEx2 = "\\b(?:[a-zA-Z]*)(?:[eE])?(\\d{1,3})(?:[a-zA-Z]*)\\b";
    //QString serienAufbau = "{SERIE} - S{STAFFEL2}E{EPISODE2} - {TITEL}";
    QString serienAufbau = "{SERIES} - S{SEASON2}E{EPISODE2} - {TITLE}";
    int OrdnerStrukturCheck = 0;
    QString OrdnerStruktur = "{SERIES} ({YEAR})/Season {SEASON2}";
    QStringList VideoFilters = (QStringList() << "*.avi" << "*.mp4" << "*.mkv" << "*.wmv" << "*.mov" << "*.m4v" << "*.mpg" << "*.mpeg" << "*.flv" << "*.divx" << "*.xvid");

    QString episoden_suche_tvdb = "http://thetvdb.com/api/GetSeries.php?language={TVLANG}&seriesname={SEARCH}";
    QString episoden_info_tvdb = "http://thetvdb.com/api/{TVDB_API_KEY}/series/{ID}/all/{TVLANG}.xml";
    QString episoden_suche_tmdb = "http://api.tmdb.org/3/search/tv?query={SEARCH}&api_key={TMDB_API_KEY}&language={TVLANG}";
    QString episoden_info_tmdb = "http://api.tmdb.org/3/tv/{ID}/season/{SEASON}/episode/{EPISODE}?api_key={TMDB_API_KEY}&language={TVLANG}";
    QString episoden_suche_tvmaze = "http://api.tvmaze.com/search/shows?q={SEARCH}";
    QString episoden_info_tvmaze = "http://api.tvmaze.com/shows/{ID}/episodebynumber?season={SEASON}&number={EPISODE}";

    // OMDb API
    QString apiKey_OMDb = "";
    QString episoden_suche_omdb = "http://www.omdbapi.com/?type=series&r=json&v=1&s={SEARCH}";
    QString episoden_info_omdb = "http://www.omdbapi.com/?i={ID}&Season={SEASON}&Episode={EPISODE}";

    // rekusriver dateifilter
    QStringList regExExcludeList = (QStringList() << "sample" << "vorschau" << "preview");
    QString regExExclude = regExExcludeList.join("|");

    //Einstellungen laden
    //QSettings setup("GrafSauger", "Episodiac");
    QString appPath = qApp->applicationDirPath();
    QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
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

    QString load_vFilters = setup.value("VideoFilters").toString();
    if(!QString(load_vFilters).isEmpty())
    {
        VideoFilters = load_vFilters.split(" ");
    }

    //ui->comboBox->addItem("de");
    //ui->comboBox->addItem("en");

    if(!QString(load_tvlang).isEmpty())
    {
        tvlang = load_tvlang;
    }

    QString load_tvlangList = setup.value("tvlangList").toString();
    if(!QString(load_tvlangList).isEmpty())
    {
        tvlangList = load_tvlangList.split(" ");
    }
    ui->comboBox->addItems(tvlangList);
    ui->comboBox->setCurrentText(tvlang);

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

    ui->lineEdit->setText(apiKey_tvdb);
    ui->lineEdit_2->setText(apiKey_tmdb);
    ui->lineEdit_6->setText(apiKey_tvMaze);
    ui->lineEdit_11->setText(apiKey_OMDb);
    ui->lineEdit_3->setText(eRegEx);
    ui->lineEdit_10->setText(eRegEx2);
    ui->lineEdit_4->setText(serienAufbau);
    ui->lineEdit_15->setText(OrdnerStruktur);
    if(OrdnerStrukturCheck)
    {
        ui->checkBox->setChecked(true);
    }

    ui->lineEdit_tvdb_suche->setText(episoden_suche_tvdb);
    ui->lineEdit_tvdb_info->setText(episoden_info_tvdb);
    ui->lineEdit_tmdb_suche->setText(episoden_suche_tmdb);
    ui->lineEdit_tmdb_info->setText(episoden_info_tmdb);
    ui->lineEdit_7->setText(episoden_suche_tvmaze);
    ui->lineEdit_8->setText(episoden_info_tvmaze);
    ui->lineEdit_12->setText(episoden_suche_omdb);
    ui->lineEdit_13->setText(episoden_info_omdb);

    ui->lineEdit_9->setText(VideoFilters.join(" "));

    ui->lineEdit_14->setText(regExExcludeList.join(" "));
}

Einstellungen::~Einstellungen()
{
    delete ui;
}

void Einstellungen::on_pushButton_2_clicked()
{
    this->close();
}

void Einstellungen::on_pushButton_clicked()
{
    if(ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty() || ui->lineEdit_3->text().isEmpty())
    {
        QMessageBox::warning(this, tr("TV Serien"), tr("Nicht gespeichert!\n\nEin oder mehrere Eingabefelder sind leer oder enthält ungültige Daten."));
    }
    else
    {
        //QSettings setup("GrafSauger", "Episodiac");
        QString appPath = qApp->applicationDirPath();
        QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
        setup.beginGroup("Einstellungen");

        // rekursiver dateifilter
        setup.setValue("rekursivFilters", ui->lineEdit_14->text());

        setup.setValue("tvdb_api_key", ui->lineEdit->text());
        setup.setValue("tmdb_api_key", ui->lineEdit_2->text());
        setup.setValue("tvmaze_api_key", ui->lineEdit_6->text());
        setup.setValue("omdb_api_key", ui->lineEdit_11->text());

        setup.setValue("sprache_episoden", ui->comboBox->currentText());

        QStringList tvlangList;
        int comboCount = ui->comboBox->count();
        for(int i = 0; i < comboCount; i++)
        {
            ui->comboBox->setCurrentIndex(i);
            tvlangList << ui->comboBox->currentText();
        }
        setup.setValue("tvlangList", tvlangList.join(" "));

        setup.setValue("regex_suche", ui->lineEdit_3->text());
        setup.setValue("regex2_suche", ui->lineEdit_10->text());

        QString serienAufbauA = ui->lineEdit_4->text();
        serienAufbauA.remove(QRegularExpression("[\\\\?:|,;*=<>/^\\[\\]]"));
        setup.setValue("serienAufbau", serienAufbauA);

        QString OrdnerStrukturA = ui->lineEdit_15->text();
        OrdnerStrukturA.remove(QRegularExpression("[\\\\?:|,;*=<>^\\[\\]]"));
        setup.setValue("OrdnerStruktur", OrdnerStrukturA);

        int struktur = 0;
        if(ui->checkBox->isChecked())
        {
            struktur = 1;
        }
        setup.setValue("OrdnerStrukturCheck", struktur);

        setup.setValue("episoden_suche_tvdb", ui->lineEdit_tvdb_suche->text());
        setup.setValue("episoden_info_tvdb", ui->lineEdit_tvdb_info->text());
        setup.setValue("episoden_suche_tmdb", ui->lineEdit_tmdb_suche->text());
        setup.setValue("episoden_info_tmdb", ui->lineEdit_tmdb_info->text());
        setup.setValue("episoden_suche_tvmaze", ui->lineEdit_7->text());
        setup.setValue("episoden_info_tvmaze", ui->lineEdit_8->text());
        setup.setValue("episoden_suche_omdb", ui->lineEdit_12->text());
        setup.setValue("episoden_info_omdb", ui->lineEdit_13->text());

        setup.setValue("VideoFilters", ui->lineEdit_9->text());

        setup.endGroup();

        //Einstellungen laden
        QString tvlang = "de";
        //QSettings setup2("GrafSauger", "Episodiac");
        QSettings setup2(appPath + "/config.ini", QSettings::IniFormat);
        setup2.beginGroup("Einstellungen");
        QString load_tvlang = setup2.value("sprache_episoden").toString();
        if(!QString(load_tvlang).isEmpty())
        {
            tvlang = load_tvlang;
        }
        ui->comboBox->setCurrentText(tvlang);
        setup2.endGroup();

        QMessageBox::information(this, tr("TV Serien"), tr("Einstellungen gespeichert!"));
    }
}

void Einstellungen::on_pushButton_3_clicked()
{
    //lineEdit_5
    QString newLang = ui->lineEdit_5->text();
    if(QString(newLang).length() == 2)
    {
        //ui->comboBox->addItem(newLang);
        //ui->comboBox->clear();

        QStringList tvlangList;
        int comboCount = ui->comboBox->count();
        for(int i = 0; i < comboCount; i++)
        {
            ui->comboBox->setCurrentIndex(i);
            tvlangList << ui->comboBox->currentText();
            //tvlangList += ui->comboBox->currentText();
        }
        tvlangList << newLang;
        tvlangList.removeDuplicates();
        ui->comboBox->clear();
        ui->comboBox->addItems(tvlangList);

        //ui->comboBox->setCurrentText(newLang);
        ui->lineEdit_5->clear();
    }
}

void Einstellungen::on_pushButton_4_clicked()
{
    QString remLang = ui->comboBox->currentText();
    if(!remLang.isEmpty())
    {
        if(remLang == "de" || remLang == "en")
        {
            QMessageBox::information(this, tr("Sprachen"), tr("Die Sprachen <font color='red'><b>de</b></font> (Deutsch) und <font color='red'><b>en</b></font> (Englisch), können nicht gelöscht werden!"));
        }
        else
        {
            ui->comboBox->removeItem(ui->comboBox->currentIndex());

            //Einstellungen laden
            QString tvlang = "de";
            //QSettings setup("GrafSauger", "Episodiac");
            QString appPath = qApp->applicationDirPath();
            QSettings setup(appPath + "/config.ini", QSettings::IniFormat);
            setup.beginGroup("Einstellungen");
            QString load_tvlang = setup.value("sprache_episoden").toString();
            if(!QString(load_tvlang).isEmpty())
            {
                tvlang = load_tvlang;
            }
            ui->comboBox->setCurrentText(tvlang);
            setup.endGroup();
        }
    }
}

void Einstellungen::on_pushButton_5_clicked()
{
    QDesktopServices::openUrl(QUrl("http://thetvdb.com/wiki/index.php/Programmers_API"));
}

void Einstellungen::on_pushButton_6_clicked()
{
    QDesktopServices::openUrl(QUrl("http://docs.themoviedb.apiary.io/"));
}

void Einstellungen::on_pushButton_7_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.tvmaze.com/api"));
}

void Einstellungen::on_pushButton_8_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.omdbapi.com/"));
}
