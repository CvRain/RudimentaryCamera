#include <QApplication>
#include <QPushButton>

#include "main_window.hpp"

int main (int argc, char *argv[])
{
	QApplication a (argc, argv);
	const auto main_window = new MainWindow;
	main_window->show();

	return QApplication::exec ();
}
