#pragma once

#include <QMainWindow>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QListWidget;
class QMediaPlayer;
class QAudioOutput;
class QSlider;
class QLabel;
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

	void onDurationChanged(qint64 duration);
	void onPositionChanged(qint64 position);
	void onSeekRequested(int position);
	void onVolumeChanged(int volume);
	void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
	void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
private:
	void updateButtonStates();
	QString formatTime(qint64 milliseconds);

    QListWidget *m_playlistWidget = nullptr;
    QPushButton *m_btnLoad = nullptr;
    QPushButton *m_btnPlay = nullptr;
    QPushButton *m_btnPause = nullptr;
    QPushButton *m_btnStop = nullptr;

    QSlider *m_progressSlider = nullptr;
    QLabel *m_timeLabel = nullptr;
    QSlider *m_volumeSlider = nullptr;
    QLabel *m_nowPlayingLabel = nullptr;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
};

