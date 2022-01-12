#include "GridHeaderView.h"

#include "GridHeaderModel.h"

#include <QPainter>
#include <QBrush>
#include <QTableView>
#include <QComboBox>
#include <QApplication>
#include "ParamGridModel.h"


namespace {
	const QString spaceHolderText = "filter";
	const int leftRightMargin = 4;
}

GridHeaderView::GridHeaderView(QTableView *parent) :
	QHeaderView(Qt::Horizontal, parent)
{
	connect(this, &QHeaderView::sectionDoubleClicked, this, &GridHeaderView::doubleClicked);
	connect(this, &QHeaderView::sectionClicked, this, &GridHeaderView::clicked);

	setSectionsClickable(true);
}

void GridHeaderView::commitFilterData(int logicalIndex, QObject* object)
{
	// Popup list is a couse of editing finish so prevent it
	if(QComboBox* comboBox = qobject_cast<QComboBox*>(object)) {
		if(comboBox->view()->isVisible()) {
			return;
		}
	}

	QString data = DataTableDelegate::getContentFromEditor(qobject_cast<QWidget*>(object));
	model()->setHeaderData(logicalIndex, Qt::Horizontal, data, Qt::EditRole);

	// send data to TableView Model
	emit updateFilterData(logicalIndex, data);

	closeFilterEditor();
}

void GridHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	QRect topRect(rect), bottomRect(topRect);
	topRect.setBottomRight(QPoint(rect.right(), rect.bottom() - rect.height()/2 - 2));

	bottomRect.setTopLeft(QPoint(rect.left(), rect.top() + rect.height()/2));
	bottomRect.setBottomRight(QPoint(bottomRect.right(), bottomRect.bottom() - 2));

	QRect leftIconRect = rect;
	leftIconRect.setBottomRight(QPoint(rect.right(), rect.bottom() - 2));

	QStyleOptionHeader opt;	
	initStyleOption(&opt);

	opt.rect = logicalIndex != 0?topRect:leftIconRect;
	opt.text = model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();
	opt.textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
	opt.iconAlignment = Qt::AlignVCenter | Qt::AlignTop;

	int sortIndicator = model()->headerData(logicalIndex, Qt::Horizontal, Qt::InitialSortOrderRole).toInt();
	if(sortIndicator == QStyleOptionHeader::SortUp) {
		opt.icon = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
	} else if(sortIndicator == QStyleOptionHeader::SortDown){
		opt.icon = QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
	}

	style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
	style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);

	if(logicalIndex > 0) {
		QString filterText = model()->headerData(logicalIndex, Qt::Horizontal, Qt::EditRole).toString();
		opt.text = filterText.isEmpty()?spaceHolderText:filterText;
		opt.rect = bottomRect;
		opt.textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
		opt.icon = QIcon();// no sorting icon need here

		style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
		// text is too close to the left frame by default so shift a little to right
		bottomRect.setTopLeft(QPoint(bottomRect.left() + leftRightMargin, bottomRect.top()));
		opt.rect = bottomRect;
		opt.palette.setBrush(QPalette::ButtonText, opt.palette.brush(filterText.isEmpty()?QPalette::Mid:QPalette::LinkVisited));
		style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);
	} else {
		QVariant iconData = model()->headerData(logicalIndex, Qt::Horizontal, Qt::DecorationRole);
		if(iconData.isValid()) {
			QIcon icon = qvariant_cast<QIcon>(iconData);
			QSize iconSize = icon.actualSize(QSize(24,24), QIcon::Normal, QIcon::On);
			icon.paint(painter,
					   opt.rect.right() - iconSize.width() - leftRightMargin,
					   opt.rect.bottom() - iconSize.height() - leftRightMargin,
					   iconSize.width(), iconSize.height());
		}
	}
}

bool GridHeaderView::eventFilter(QObject* object, QEvent* event)
{
	if(event->type() == QEvent::FocusOut &&
			(qobject_cast<QComboBox*>(object) || qobject_cast<MapInputG4Editor*>(object))) {
		commitFilterData(m_filterColumnIndex, object);
	}
	return QHeaderView::eventFilter(object, event);
}

void GridHeaderView::setModel(QAbstractItemModel *model)
{
	connect(parent(), SIGNAL(updateHeaderIndicator(int)), model, SLOT(updateIndicator(int)));
	connect(this, SIGNAL(updateHeaderIndicator(int)), model, SLOT(updateIndicator(int)));

	QHeaderView::setModel(model);
}

void GridHeaderView::closeFilterEditor()
{
	if(m_filterWidget) {
		m_filterWidget->close();
		m_filterWidget = nullptr;
	}
}

void GridHeaderView::doubleClicked(int logicalIndex)
{
	closeFilterEditor();

	int height = model()->headerData(logicalIndex, Qt::Horizontal, Qt::SizeHintRole).toSize().height();
	QRect sectionRect = QRect(sectionViewportPosition(logicalIndex), height/2, sectionSize(logicalIndex), height/2);
	if(sectionRect.contains(mapFromGlobal(QCursor::pos()))) {
		IJDParameterPtr param = model()->headerData(logicalIndex, Qt::Horizontal, TableColumnProvider::ParameterRole).value<IJDParameterPtr>();
		IJDTypePtr type = model()->headerData(logicalIndex, Qt::Horizontal, TableColumnProvider::EffectiveTypeRole).value<IJDTypePtr>();
		if(!param || !type) {
			return;
		}
		QWidget* widget = DataTableDelegate::createDataEditorImpl(model()->headerData(logicalIndex, Qt::Horizontal, Qt::EditRole).toString(),
																string_cast<QString>(param->GetName()), type, param->GetConfig(),true, this);
		if(!widget) {
			return;
		}
		widget->setAttribute(Qt::WA_DeleteOnClose);

		if(QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
			lineEdit->setPlaceholderText(spaceHolderText);
			connect(lineEdit, &QLineEdit::editingFinished, [=]() {
				commitFilterData(logicalIndex, lineEdit);
			});
		} else if(QComboBox* comboBox = qobject_cast<QComboBox*>(widget)) {
			comboBox->lineEdit()->setPlaceholderText(spaceHolderText);
			comboBox->installEventFilter(this);
			connect(comboBox->lineEdit(), &QLineEdit::editingFinished, [=]() {
				commitFilterData(logicalIndex, comboBox);
			});
		} else if(MapInputG4Editor* editor = qobject_cast<MapInputG4Editor*>(widget)) {
			editor->setPlaceholderText(spaceHolderText);
			editor->installEventFilter(this);
		}

		m_filterColumnIndex = logicalIndex;
		m_filterWidget = widget;

		widget->setGeometry(sectionRect);
		widget->move(sectionRect.left(), sectionRect.top());
		widget->setFocus();
		widget->show();
	}
}

void GridHeaderView::clicked(int logicalIndex)
{
	closeFilterEditor();

	int height = model()->headerData(logicalIndex, Qt::Horizontal, Qt::SizeHintRole).toSize().height();
	QRect topRect = QRect(sectionViewportPosition(logicalIndex), 0, sectionSize(logicalIndex), height/2);
	if(logicalIndex>0 && !topRect.contains(mapFromGlobal(QCursor::pos()))) {
		return;
	}

	Qt::SortOrder order = Qt::AscendingOrder;
	if(model()->headerData(logicalIndex, Qt::Horizontal, Qt::InitialSortOrderRole).toInt() == QStyleOptionHeader::SortUp) {
		order = Qt::DescendingOrder;
	}

	emit updateHeaderIndicator(logicalIndex);
	// refresh
	viewport()->update();

	emit sortColumn(logicalIndex, order);
}

GridViewTable::GridViewTable(QWidget *parent) :
	QTableView( parent )
{
	setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	/* filter the column headers so that we can allow context menus */
	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader()->setSectionsMovable(false);
	verticalHeader()->setHighlightSections(true);
	horizontalHeader()->setSectionsMovable(false);
	horizontalHeader()->setHighlightSections(false);
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragEnabled(true);
	viewport()->setAcceptDrops(true);
	setHorizontalScrollMode(ScrollPerPixel);
	setVerticalScrollMode(ScrollPerPixel);
	setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),SLOT(slotCurrentChanged(QModelIndex)));
	connect(horizontalHeader(), &QHeaderView::sectionHandleDoubleClicked, [=]() {
		for(QModelIndex& index: selectionModel()->selectedIndexes()) {
			resizeColumnToContents(index.column());
		}
	});
	connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(columnClicked(int)));

	// from DataTable c'tor
	connect(this,SIGNAL(displayFormat(DataFormat::DisplayFormat)),viewport(),SLOT(update()));
	horizontalHeader()->installEventFilter(this);
	verticalHeader()->installEventFilter(this);


	//setSortingEnabled(true);
}

void GridViewTable::sortByColumn(int column, Qt::SortOrder order)
{
	emit updateHeaderIndicator(column);
	QTableView::sortByColumn(column, order);
}


