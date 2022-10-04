//
// Created by xing on 2022-07-31.
//

#pragma once
#include <QAbstractScrollArea>
#include <functional>
#include <QHeaderView>
#include "AbstractTableView.h"


class HexView : public AbstractTableView
{
	Q_OBJECT
public:
	explicit HexView(QWidget* parent);

	void setAddressByteSize(int addressByteSize);

	void setRange(size_t base, size_t size);

	using ReadCallback = std::function<const QByteArray(uint64_t base, uint64_t size)>;
	void setReadCallBack(ReadCallback cb);

	void setCurrentAddress(uint64_t address);
	int lineCount() override;
	void updateFont(const QFont &f) override;

protected:
	void drawLine(QPainter *p, int scrollLine, int currLine) override;

private:
	int m_addressByteSize = 8;
	uint64_t m_base;
	uint64_t m_size;

	ReadCallback m_readCallback;

	int m_fontWidth = 0;
};



