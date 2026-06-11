#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;
private slots:
	void onButtonClicked();
private:
    QPushButton *m_button;
    QLabel *m_label;
};
