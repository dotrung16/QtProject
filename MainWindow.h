#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QListWidget;
class QMediaPlayer;
class QAudioOutput;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;
private slots:
	void onLoadClicked();
	void onPlayClicked();
	void onPauseClicked();
	void onStopClicked();
private:
    QListWidget *m_playlistWidget = nullptr;
    QPushButton *m_btnLoad = nullptr;
    QPushButton *m_btnPlay = nullptr;
    QPushButton *m_btnPause = nullptr;
    QPushButton *m_btnStop = nullptr;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
};

