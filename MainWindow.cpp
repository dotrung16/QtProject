#include "MainWindow.h"

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle("My App");
	resize(800, 600);

	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	QVBoxLayout *layout = new QVBoxLayout(central);
	layout->setAlignment(Qt::AlignCenter);
	layout->setSpacing(20);

	m_label = new QLabel("Press the button!", central);
	m_label->setAlignment(Qt::AlignCenter);

	m_button = new QPushButton("Hello!", central);
	m_button->setFixedSize(200, 50);

	layout->addWidget(m_label);
	layout->addWidget(m_button);

	connect(m_button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
}

void MainWindow::onButtonClicked() {
	m_label->setText("Button was clicked!");
}

