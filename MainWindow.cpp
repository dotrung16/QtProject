#include "MainWindow.h"
#include <QtWidgets>
#include <QtMultimedia>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle("Music Player");
	resize(700, 500);
	setAcceptDrops(true);

	setupUI();

	m_player = new QMediaPlayer(this);
	m_audioOutput = new QAudioOutput(this);

	m_player->setAudioOutput(m_audioOutput);
	m_audioOutput->setVolume(0.5);
	
	setupConnections();
}

void MainWindow::setupUI() {
	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	QVBoxLayout *mainLayout = new QVBoxLayout(central);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	// Playlist Area
	m_playlistWidget = new QListWidget(central);
	m_playlistWidget->setStyleSheet(
		"QListWidget { border: none; padding: 10px; font-size:14px; }"
		"QListWidget::item { padding: 5px; border-radius: 5px; }"
		"QListWidget::item::selected { background-color: #E95420; color: white }"
	);

	mainLayout->addWidget(m_playlistWidget);

	// Control Bar
	QWidget *controlDock = new QWidget(central);
	controlDock->setStyleSheet(
		"QWidget { background-color: #dcdcdc; border-top: 1px solid #b3b3b3 }"
		"QPushButton { background: transparent; border: none; padding: 10px; border-radius: 15px }"
		"QPushButton::hover { background-color: #c8c8c8 }"
		"QPushButton::pressed { background-color: #E95420 }"
	);
	QVBoxLayout *dockLayout = new QVBoxLayout(controlDock);

	m_nowPlayingLabel = new QLabel("Ready", controlDock);
	m_nowPlayingLabel->setAlignment(Qt::AlignCenter);
	m_nowPlayingLabel->setStyleSheet("font-weight: bold; color: #333; margin-bottom: 5px;");

	dockLayout->addWidget(m_nowPlayingLabel);

	// Progress
	QHBoxLayout *progressLayout = new QHBoxLayout();

	m_timeLabel = new QLabel("00:00 / 00:00", controlDock);
	m_progressSlider = new QSlider(Qt::Horizontal, controlDock);
	m_progressSlider->setRange(0, 0);

	progressLayout->addWidget(m_progressSlider);
	progressLayout->addWidget(m_timeLabel);

	dockLayout->addLayout(progressLayout);

	// Buttons and Volume
	QHBoxLayout *controlLayout = new QHBoxLayout();
	m_btnLoad = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "", controlDock);
	m_btnPlay = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "", controlDock);
	m_btnPause = new QPushButton(style()->standardIcon(QStyle::SP_MediaPause), "", controlDock);
	m_btnStop = new QPushButton(style()->standardIcon(QStyle::SP_MediaStop), "", controlDock);

	m_btnPlay->setIconSize(QSize(48, 48));

	m_volumeSlider = new QSlider(Qt::Horizontal, controlDock);
	m_volumeSlider->setRange(0, 100);
	m_volumeSlider->setValue(50);
	m_volumeSlider->setMaximumWidth(100);

	controlLayout->addWidget(m_btnLoad);
	controlLayout->addSpacing(20);
	controlLayout->addWidget(m_btnPlay);
	controlLayout->addWidget(m_btnPause);
	controlLayout->addWidget(m_btnStop);
	controlLayout->addWidget(new QLabel("Vol: ", controlDock));
	controlLayout->addWidget(m_volumeSlider);

	dockLayout->addLayout(controlLayout);
	mainLayout->addWidget(controlDock);
}

void MainWindow::setupConnections() {
	connect(m_btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
	connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
	connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
	connect(m_btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
	connect(m_playlistWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onPlayClicked);

	// Player Signal mapping
	connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
	connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
	connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onSeekRequested);
	connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
	connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);
	connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
}

void MainWindow::addMedia(const QStringList &filePaths) {
	for (const QString &path : filePaths) {
		QFileInfo info(path);
		QString ext = info.suffix().toLower();

		if (ext == "mp3" || ext == "wav" || ext == "flac") {
			QListWidgetItem *item = new QListWidgetItem(info.fileName());
			item->setData(Qt::UserRole, path);
			m_playlistWidget->addItem(item);
		}
	}

	if (m_playlistWidget->count() > 0 && !m_playlistWidget->currentItem()) {
		m_playlistWidget->setCurrentRow(0);
	}

	updateUI(); 
}

void MainWindow::onLoadClicked() {
	QStringList files = QFileDialog::getOpenFileNames(this, "Select Music", "", "Audio (*.mp3 *.wav *.flac)");

	if (!files.isEmpty()) addMedia(files);
}

void MainWindow::onPlayClicked() {
	auto *item = m_playlistWidget->currentItem();

	if (!item) return;

	QUrl url = QUrl::fromLocalFile(item->data(Qt::UserRole).toString());

	if (m_player->source() != url) {
		m_player->setSource(url);
	}

	m_player->play();

	m_nowPlayingLabel->setText("Playing: " + item->text());
}

void MainWindow::onPauseClicked() { m_player->pause(); }

void MainWindow::onStopClicked() {
	m_player->stop();
	m_nowPlayingLabel->setText("Ready");
	m_progressSlider->setValue(0);
}

void MainWindow::updateUI() {
	bool hasItems = m_playlistWidget->count() > 0;
	auto state = m_player->playbackState();

	m_btnPlay->setEnabled(hasItems);
	m_btnPause->setEnabled(state == QMediaPlayer::PlayingState);
	m_btnStop->setEnabled(state != QMediaPlayer::StoppedState);
}

void MainWindow::onDurationChanged(qint64 duration) {
	m_progressSlider->setMaximum(static_cast<int>(duration));
	// Reset the time label with the new total duration
	m_timeLabel->setText(formatTime(0) + " / " + formatTime(m_player->duration()));
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
	// QAudioOutput volume is 0.0 to 1.0, Slider is 0 to 100
	m_audioOutput->setVolume(volume / 100.0f);
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
	if (status == QMediaPlayer::EndOfMedia) {
		int nextRow = m_playlistWidget->currentRow() + 1;

		if (nextRow < m_playlistWidget->count()) {
			m_playlistWidget->setCurrentRow(nextRow);
			onPlayClicked(); // Start the next track
		} 
		else {
			// End of playlist reached
			onStopClicked();
		}
	}
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
	updateUI();
}

QString MainWindow::formatTime(qint64 milliseconds) {
	// Convert ms to a human-readable mm:ss format
	qint64 seconds = milliseconds / 1000;
	qint64 minutes = seconds / 60;

	return QString("%1:%2")
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0'));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	// Only accept the drag if it contains file URLs
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event) {
	const QMimeData *mimeData = event->mimeData();

	if (mimeData->hasUrls()) {
		QStringList paths;

		for (const QUrl &url : mimeData->urls()) {
			if (url.isLocalFile()) {
				paths << url.toLocalFile();
			}
		}

		if (!paths.isEmpty()) {
			addMedia(paths);
			event->acceptProposedAction();
		}
	}
}