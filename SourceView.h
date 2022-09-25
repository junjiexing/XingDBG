#pragma once

#include <QWidget>

class QTabWidget;

class SourceView : public QWidget
{
	Q_OBJECT

public:
	SourceView();
	~SourceView() override;

	void addSourceFile(QString const& path);
private:
	QTabWidget* m_tab = nullptr;
};

