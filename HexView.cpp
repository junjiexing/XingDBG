//
// Created by xing on 2022-07-31.
//

#include "HexView.h"
#include <QPainter>
#include <QTableView>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QEvent>
#include <QDebug>


HexView::HexView(QWidget *parent)
	: AbstractTableView(parent)
{
	setColumns(QStringList() << "Address" << "Hex" << "Ascii");
}

void HexView::setRange(size_t base, size_t size)
{
	m_base = base;
	m_size = size;

	updateScrollBar();

	refresh();
}

void HexView::setReadCallBack(ReadCallback cb)
{
	m_readCallback = std::move(cb);
	refresh();
}

void HexView::setAddressByteSize(int addressByteSize)
{
	m_addressByteSize = addressByteSize;

	resizeHeaderSection(0, int(m_fontWidth * 2 * addressByteSize));
	resizeHeaderSection(1, int(m_fontWidth * 2 * 16));
	resizeHeaderSection(2, m_fontWidth * 16);

	refresh();
}

void HexView::setCurrentAddress(uint64_t address)
{
	assert(address >= m_base);
	assert(address < m_base + m_size);
	if (address < m_base | address > m_base + m_size) return;

	scrollToLine(int(address - m_base) / 16);

	refresh();
}

void HexView::drawLine(QPainter *p, int scrollLine, int currLine)
{
	if (!m_readCallback) return;


	auto fontHeight = lineHeight();
	auto dataStart = m_base + currLine * 16;
	auto dataSize = std::min(16, int(m_size - (dataStart - m_base)));

	const auto &data = m_readCallback(dataStart, dataSize);

	auto addressWidth = headerSectionSize(0);
	auto hexWidth = headerSectionSize(1);

	auto lineTop = int(fontHeight * (currLine - scrollLine));
	p->drawText(QRect{0, lineTop, addressWidth, fontHeight}, 0,
				QString("%1").arg(dataStart, m_addressByteSize * 2, 16, QLatin1Char('0')).toUpper());

	auto ptr = data.constData();
	for (int j = 0; j < data.size(); ++j)
	{
		p->drawText(QRect{addressWidth + m_fontWidth * 2 * j, lineTop, m_fontWidth * 2, fontHeight}, Qt::AlignCenter,
					QString("%1").arg(uchar(ptr[j]), 2, 16, QLatin1Char('0')).toUpper());
	}

	for (int j = 0; j < data.size(); ++j)
	{
		QRect dataRc{
			int(addressWidth + hexWidth + m_fontWidth * j),
			lineTop,
			m_fontWidth,
			fontHeight};
		p->fillRect(dataRc, palette().base());
		auto c = ptr[j];
		if (c < ' ' || c > '~')
			c = '.';
		p->drawText(dataRc, Qt::AlignCenter, QChar(c));
	}

}
int HexView::lineCount()
{
	return int(m_size / 16 + (m_size % 16 == 0 ? 0 : 1));
}
void HexView::updateFont(const QFont &f)
{
	AbstractTableView::updateFont(f);

	for (char i = ' '; i < '~'; ++i)
	{
		m_fontWidth = std::max(m_fontWidth, fontMetrics().horizontalAdvance(QLatin1Char(i)));
	}
}

