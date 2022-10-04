//
// Created by xing on 2022-08-07.
//

#include "AbstractTableView.h"
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>


AbstractTableView::AbstractTableView(QWidget *parent)
	: QAbstractScrollArea(parent),
	  m_header(new QHeaderView(Qt::Horizontal, this))
{
	setViewportMargins(0, 20, 0, 0);
	connect(horizontalScrollBar(), &QScrollBar::sliderMoved, this, [this](int position)
	{ m_header->setOffset(position); });
	connect(m_header, &QHeaderView::sectionResized, this, [this]()
	{
		updateScrollBar();
		refresh();
	});
}

AbstractTableView::~AbstractTableView() = default;

void AbstractTableView::init()
{
	m_lineHeight = fontMetrics().height();

	refresh();
}

void AbstractTableView::refresh()
{
	viewport()->update();
}

void AbstractTableView::viewportPaintEvent()
{
	QPainter p(viewport());
	p.translate(-m_header->offset(), 0);

	QStyleOptionViewItem option;
	option.state &= ~QStyle::State_MouseOver;
	option.font = font();
	if (!hasFocus())
		option.state &= ~QStyle::State_Active;

	option.state &= ~QStyle::State_HasFocus;
	int pm = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
	option.decorationSize = QSize(pm, pm);
	option.decorationPosition = QStyleOptionViewItem::Left;
	option.decorationAlignment = Qt::AlignCenter;
	option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
	option.rect = QRect();
	option.showDecorationSelected = style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, nullptr, this);
	option.locale = locale();
	option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	option.widget = this;

	auto gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);
	p.setPen(QPen(QColor::fromRgba(static_cast<QRgb>(gridHint))));
	int secWidth = 0;
	for (int i = 0; i < m_header->count(); ++i)
	{
		secWidth += m_header->sectionSize(i);
		p.drawLine(secWidth, 0, secWidth, viewport()->height());
	}

	auto scrollLine = linePos();
	for (int i = 0; i < std::min(viewport()->height() / lineHeight() + 1, lineCount() - scrollLine); ++i)
	{
		drawLine(&p, scrollLine, scrollLine + i);
	}
}

void AbstractTableView::viewportResizeEvent()
{
	m_header->setGeometry(0, 0, viewport()->width(), viewportMargins().top());
	updateScrollBar();
}

bool AbstractTableView::viewportEvent(QEvent *event)
{
	if (event->type() == QEvent::Paint)
	{
		viewportPaintEvent();
		return true;
	}
	if (event->type() == QEvent::Resize)
	{
		viewportResizeEvent();
		return true;
	}
	if (event->type() == QEvent::MouseButtonPress)
	{
		onMouseButtonPress(static_cast<QMouseEvent*>(event));
	}
	return QAbstractScrollArea::viewportEvent(event);
}

void AbstractTableView::updateScrollBar()
{
	int maxWidth = 0;
	for (int i = 0; i < m_header->count(); ++i)
	{
		maxWidth += m_header->sectionSize(i);
	}

	if (maxWidth > viewport()->width())
	{
		horizontalScrollBar()->setMaximum(maxWidth);
		horizontalScrollBar()->setSingleStep(1);
		horizontalScrollBar()->setPageStep(viewport()->width());
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else
	{
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		horizontalScrollBar()->setValue(0);
		m_header->setOffset(0);
	}

	auto pageLineCount = viewport()->height() / fontMetrics().height();
	if (lineCount() > pageLineCount)
	{
		verticalScrollBar()->setMaximum(lineCount());
		verticalScrollBar()->setSingleStep(1);
		verticalScrollBar()->setPageStep(pageLineCount);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	else
	{
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		verticalScrollBar()->setValue(0);
	}
}

class HeaderModel: public QAbstractItemModel
{
public:
	HeaderModel(QStringList const &labels)
		: m_labels(labels) {}
	QModelIndex index(int row, int column, const QModelIndex &parent) const override
	{
		return {};
	}
	QModelIndex parent(const QModelIndex &child) const override
	{
		return {};
	}
	int rowCount(const QModelIndex &parent) const override
	{
		return 0;
	}
	int columnCount(const QModelIndex &parent) const override
	{
		return m_labels.size();
	}
	QVariant data(const QModelIndex &index, int role) const override
	{
		return {};
	}
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role != Qt::DisplayRole)
			return {};

		if (orientation == Qt::Horizontal)
		{
			return m_labels.at(section);
		}
		return {};
	}

	QStringList m_labels;
};

void AbstractTableView::setColumns(const QStringList &labels)
{
	//TODO: 是否需要delete？
	m_header->setModel(new HeaderModel(labels));

}

int AbstractTableView::lineHeight()
{
	return m_lineHeight;
}

void AbstractTableView::updateFont(const QFont &f)
{
	QWidget::setFont(f);
	m_lineHeight = fontMetrics().height();
}

void AbstractTableView::resizeHeaderSection(int index, int size)
{
	m_header->resizeSection(index, size);
}

void AbstractTableView::scrollToLine(int line)
{
	verticalScrollBar()->setValue(line);
}

int AbstractTableView::linePos()
{
	return verticalScrollBar()->value();
}

int AbstractTableView::headerSectionSize(int index)
{
	return m_header->sectionSize(index);
}

void AbstractTableView::onMouseButtonPress(QMouseEvent *event)
{
	auto y = event->pos().y();
	m_selectedLine = linePos() + y / lineHeight();
	refresh();
}
int AbstractTableView::selectedLine()
{
	return m_selectedLine;
}

