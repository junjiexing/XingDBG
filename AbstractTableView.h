//
// Created by xing on 2022-08-07.
//

#pragma once
#include <QAbstractScrollArea>
#include <QHeaderView>
#include <QPainter>


class AbstractTableView : public QAbstractScrollArea
{
	Q_OBJECT

public:
	explicit AbstractTableView(QWidget* parent);

	virtual ~AbstractTableView() override;

	virtual void init();

	virtual void refresh();

	virtual int lineCount() = 0;

	virtual int lineHeight();

	void setColumns(QStringList const& labels);

	virtual void updateFont(QFont const&f);

	void resizeHeaderSection(int index, int size);
	int headerSectionSize(int index);

	void scrollToLine(int line);
	int linePos();

	int selectedLine();

protected:
	virtual void viewportPaintEvent();
	virtual void viewportResizeEvent();
	virtual bool viewportEvent(QEvent *event) override;

	void onMouseButtonPress(QMouseEvent* event);

	virtual void drawLine(QPainter* p, int scrollLine, int currLine) = 0;

	void updateScrollBar();

	int m_selectedLine = 0;

private:
	QHeaderView* m_header;
	int m_lineHeight;
};



