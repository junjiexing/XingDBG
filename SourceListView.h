#pragma once

#include <QWidget>


class SourceListView : public QWidget
{
	Q_OBJECT

public:
	SourceListView();
	~SourceListView() override;
private:

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;

};

