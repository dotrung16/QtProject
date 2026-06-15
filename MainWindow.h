#pragma once

#include <QMainWindow>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
class QPushButton;
class QListWidget;
class QAudioOutput;
class QSlider;
class QLabel;
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow { 
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;

private slots:
	// UI Actions
	void onLoadClicked();
	void onPlayClicked();
	void onPauseClicked();
	void onStopClicked();

	// Player Events
	void onDurationChanged(qint64 duration);
	void onPositionChanged(qint64 position);
	void onSeekRequested(int position);
	void onVolumeChanged(int volume);
	void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
	void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
	void setupUI();
	void setupConnections();
	void addMedia(const QStringList &filePaths);
	void updateUI();
	QString formatTime(qint64 milliseconds);

	// UI Components
	QListWidget *m_playlistWidget = nullptr;
	QPushButton *m_btnLoad = nullptr;
	QPushButton *m_btnPlay = nullptr;
	QPushButton *m_btnPause = nullptr;
	QPushButton *m_btnStop = nullptr;
	QSlider *m_progressSlider = nullptr;
	QSlider *m_volumeSlider = nullptr;
	QLabel *m_timeLabel = nullptr;
	QLabel *m_nowPlayingLabel = nullptr;

	// Media Logic
	QMediaPlayer *m_player = nullptr;
	QAudioOutput *m_audioOutput = nullptr;

protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
};

