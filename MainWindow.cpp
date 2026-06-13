#include "MainWindow.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFileDialog>
#include <QStyle>
#include <QSize>
#include <QFileInfo>
#include <QSlider>
#include <QLabel>
#include <QTime>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle("Music Player");
	resize(700, 500);
	setAcceptDrops(true);

	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	QVBoxLayout *mainLayout = new QVBoxLayout(central);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	m_playlistWidget = new QListWidget(central);
	m_playlistWidget->setStyleSheet(
		"QListWidget { border: none; padding: 10px; font-size: 14px; }"
		"QListWidget::item { padding: 5px; border-radius: 5px; }"
		"QListWidget::item::selected { background-color: #E95420; color: white }"
	);
	mainLayout->addWidget(m_playlistWidget);

	QWidget *controlDock = new QWidget(central);
	controlDock->setStyleSheet(
		"QWidget { background-color: #dcdcdc; border-top: 1px solid #b3b3b3 }"
		"QPushButton { background: transparent; border: none; padding: 10px; border-radius: 15px }"
		"QPushButton::hover { background-color: #c8c8c8 }"
		"QPushButton::pressed { background-color: #E95420 }"
		"QPushButton::disabled { opacity: 0.5; }"
	);

	QVBoxLayout *dockLayout = new QVBoxLayout(controlDock);

	m_nowPlayingLabel = new QLabel("Ready", controlDock);
	m_nowPlayingLabel->setAlignment(Qt::AlignCenter);
	m_nowPlayingLabel->setStyleSheet("font-weight: bold; color: #333; margin-bottom: 5px;");
	dockLayout->addWidget(m_nowPlayingLabel);

	QHBoxLayout *progressLayout = new QHBoxLayout();
	m_timeLabel = new QLabel("00:00 / 00:00", controlDock);
	m_progressSlider = new QSlider(Qt::Horizontal, controlDock);
	m_progressSlider->setRange(0, 0);

	progressLayout->addWidget(m_progressSlider);
	progressLayout->addWidget(m_timeLabel);
	dockLayout->addLayout(progressLayout);

	QHBoxLayout *controlLayout = new QHBoxLayout();
	controlLayout->setContentsMargins(20, 10, 20, 10);
	controlLayout->setAlignment(Qt::AlignCenter);

	m_btnLoad = new QPushButton(controlDock);
	m_btnLoad->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	m_btnLoad->setIconSize(QSize(32, 32));
	m_btnLoad->setToolTip("Load Music");

	m_btnPlay = new QPushButton(controlDock);
	m_btnPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	m_btnPlay->setIconSize(QSize(48, 48));
	m_btnPlay->setToolTip("Play/Restart");

	m_btnPause = new QPushButton(controlDock);
	m_btnPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	m_btnPause->setIconSize(QSize(32, 32));
	m_btnPause->setToolTip("Pause");

	m_btnStop = new QPushButton(controlDock);
	m_btnStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	m_btnStop->setIconSize(QSize(32, 32));
	m_btnStop->setToolTip("Stop");

	QHBoxLayout *volumeLayout = new QHBoxLayout();
	QLabel *volumeIcon = new QLabel(controlDock);
	volumeIcon->setPixmap(style()->standardIcon(QStyle::SP_MediaVolume).pixmap(24, 24));
	m_volumeSlider = new QSlider(Qt::Horizontal, controlDock);
	m_volumeSlider->setRange(0, 100);
	m_volumeSlider->setValue(50);
	m_volumeSlider->setMaximumWidth(100);
	volumeLayout->addWidget(volumeIcon);
	volumeLayout->addWidget(m_volumeSlider);

	controlLayout->addWidget(m_btnLoad);
	controlLayout->addSpacing(20);
	controlLayout->addWidget(m_btnPlay);
	controlLayout->addWidget(m_btnPause);
	controlLayout->addWidget(m_btnStop);
	controlLayout->addSpacing(20);
	controlLayout->addLayout(volumeLayout);

	dockLayout->addLayout(controlLayout);
	mainLayout->addWidget(controlDock);

	m_player = new QMediaPlayer(this);
	m_audioOutput = new QAudioOutput(this);
	m_player->setAudioOutput(m_audioOutput);
	m_audioOutput->setVolume(0.5);

	connect(m_btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
	connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
	connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
	connect(m_btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);

	connect(m_playlistWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onPlayClicked);

	connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
	connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
	connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onSeekRequested);
	connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
	connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
	connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);

	connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
		qWarning() << "Media Player Error: " << errorString;
	});

	updateButtonStates();
}

void MainWindow::onLoadClicked() {
	QStringList files = QFileDialog::getOpenFileNames(this, "Select Music Files", "", "Audio Files (*.mp3 *.wav *.flac)");

	for (const QString &file : files) {
		QFileInfo fileInfo(file);
		QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
		item->setData(Qt::UserRole, file);
		m_playlistWidget->addItem(item);
	}

	if (m_playlistWidget->count() > 0 && !m_playlistWidget->currentItem()) {
		m_playlistWidget->setCurrentRow(0);
	}

	updateButtonStates();
}

void MainWindow::onPlayClicked() {
	if (m_playlistWidget->currentItem()) {
		QString filePath = m_playlistWidget->currentItem()->data(Qt::UserRole).toString();
		QUrl fileUrl = QUrl::fromLocalFile(filePath);
		if (m_player->source() != fileUrl) {
			m_player->setSource(fileUrl);
		}
		m_player->play();

		m_nowPlayingLabel->setText("Playing: " + m_playlistWidget->currentItem()->text());
		m_playlistWidget->scrollToItem(m_playlistWidget->currentItem());
	}
}

void MainWindow::onPauseClicked() {
	m_player->pause();
}

void MainWindow::onStopClicked() {
	m_player->stop();
	m_progressSlider->setValue(0);
	m_timeLabel->setText(formatTime(0) + " / " + formatTime(m_player->duration()));
	m_nowPlayingLabel->setText("Ready");
}

void MainWindow::onDurationChanged(qint64 duration) {
	m_progressSlider->setMaximum(static_cast<int>(duration));
	m_timeLabel->setText(formatTime(0) + " / " + formatTime(duration));
}

void MainWindow::onPositionChanged(qint64 position) {
	if (!m_progressSlider->isSliderDown()) {
		m_progressSlider->setValue(static_cast<int>(position));
	}
	m_timeLabel->setText(formatTime(position) + " / " + formatTime(m_player->duration()));
}

void MainWindow::onSeekRequested(int position) {
	m_player->setPosition(static_cast<qint64>(position));
}

void MainWindow::onVolumeChanged(int volume) {
	m_audioOutput->setVolume(volume / 100.0f);
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
	if (status == QMediaPlayer::EndOfMedia) {
		int currentRow = m_playlistWidget->currentRow();
		if (currentRow < m_playlistWidget->count() - 1) {
			m_playlistWidget->setCurrentRow(currentRow + 1);
			onPlayClicked();
		} 
		else {
			m_player->stop();
		}
	}
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
	updateButtonStates();
}

void MainWindow::updateButtonStates() {
	bool hasItems = m_playlistWidget->count() > 0;
	bool isPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
	bool isPaused = m_player->playbackState() == QMediaPlayer::PausedState;

	m_btnPlay->setEnabled(hasItems);
	m_btnPause->setEnabled(isPlaying);
	m_btnStop->setEnabled(isPlaying || isPaused);
}

QString MainWindow::formatTime(qint64 milliseconds) {
	return QTime(0, 0).addMSecs(milliseconds).toString("mm:ss"); 
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event) {
	for (const QUrl &url : event->mimeData()->urls()) {
		if (url.isLocalFile()) {
			QString filePath = url.toLocalFile();

			if (filePath.endsWith(".mp3", Qt::CaseInsensitive) || filePath.endsWith(".wav", Qt::CaseInsensitive) || filePath.endsWith(".flac", Qt::CaseInsensitive)) {
				QFileInfo fileInfo(filePath);
				QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
				item->setData(Qt::UserRole, filePath);
				m_playlistWidget->addItem(item);
			}
		}
	}

	if (m_playlistWidget->count() > 0 && !m_playlistWidget->currentItem()) {
		m_playlistWidget->setCurrentRow(0);
	}

	updateButtonStates(); 
}