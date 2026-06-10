#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	QPushButton button("Hello!");

	button.resize(800, 600);
	button.show();

	return app.exec();
}