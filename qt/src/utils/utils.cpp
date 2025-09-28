#include <QMessageBox>

#include "utils.hpp"

void debug(const QString& text, bool _debug) {
	if (!_debug)
		return;

	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.exec();
}

bool isDebug(char** envp) {
	while (*envp) {
		if (std::string(*envp) == "DEBUG_FOOTBALL_MASTER=TRUE")
			return true;
		envp++;
	}
	return false;
}