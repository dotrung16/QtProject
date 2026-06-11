#include "MainWindow.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFileDialog>
#include <QStyle>
#include <QSize>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle("Music Player");
	resize(700, 500);

	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	QVBoxLayout *mainLayout = new QVBoxLayout(central);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	m_playlistWidget = new QListWidget();
	m_playlistWidget->setStyleSheet(
		"QListWidget { border: none; padding: 10px; font-size: 14px; }"
		"QListWidget::item { padding: 5px; border-radius: 5px; }"
		"QListWidget::item::selected { background-color: #E95420; color: white }"
	);
	mainLayout->addWidget(m_playlistWidget);

	QWidget *controlDock = new QWidget();
	controlDock->setStyleSheet(
		"QWidget { background-color: #dcdcdc; border-top: 1px solid #b3b3b3 }"
		"QPushButton { background: transparent; border: none; padding 10px; border-radius: 15px }"
		"QPushButton::hover { background-color: #c8c8c8 }"
		"QPushButton::pressed { background-color: #E95420 }"
	);

	QHBoxLayout *controlLayout = new QHBoxLayout(controlDock);
	controlLayout->setContentsMargins(20, 10, 20, 10);
	controlLayout->setAlignment(Qt::AlignCenter);

	QSize stdSize(32, 32);
	QSize playSize(48, 48);

	m_btnLoad = new QPushButton();
	m_btnLoad->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	m_btnLoad->setIconSize(stdSize);
	m_btnLoad->setToolTip("Load Music");

	m_btnPlay = new QPushButton();
	m_btnPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	m_btnPlay->setIconSize(playSize);

	m_btnPause = new QPushButton();
	m_btnPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	m_btnPause->setIconSize(stdSize);

	m_btnStop = new QPushButton();
	m_btnStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	m_btnStop->setIconSize(stdSize);

	controlLayout->addWidget(m_btnLoad);
	controlLayout->addSpacing(20);
	controlLayout->addWidget(m_btnPlay);
	controlLayout->addWidget(m_btnPause);
	controlLayout->addWidget(m_btnStop);

	mainLayout->addWidget(controlDock);

	m_player = new QMediaPlayer(this);
	m_audioOutput = new QAudioOutput(this);
	m_player->setAudioOutput(m_audioOutput);
	m_audioOutput->setVolume(0.5);

	connect(m_btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
	connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
	connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
	connect(m_btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
}

void MainWindow::onLoadClicked() {
	QStringList files = QFileDialog::getOpenFileNames(this, "Select Music Files", "", "Audio Files (*.mp3 *wav *.flac)");

	for (const QString &file : files) {
		m_playlistWidget->addItem(file);
	}
}

void MainWindow::onPlayClicked() {
	if (m_playlistWidget->currentItem()) {
		QString filePath = m_playlistWidget->currentItem()->text();
		m_player->setSource(QUrl::fromLocalFile(filePath));
		m_player->play();
	}
}

void MainWindow::onPauseClicked() {
	m_player->pause();
}

void MainWindow::onStopClicked() {
	m_player->stop();
}




