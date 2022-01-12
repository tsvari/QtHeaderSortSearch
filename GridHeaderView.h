#ifndef GRIDHEADERVIEW_H
#define GRIDHEADERVIEW_H

#include <QHeaderView>
#include <QLineEdit>
#include <QTableView>

#include "TableData.h"

class GridViewTable;
class GridHeaderView : public QHeaderView
{
	Q_OBJECT
public:
	explicit GridHeaderView(QTableView *parent);
	void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
	bool eventFilter (QObject* object, QEvent* event) override;
	void setModel(QAbstractItemModel *model) override;

private slots:
	void doubleClicked(int logicalIndex);
	void clicked(int logicalIndex);

signals:
	void updateFilterData(int section, const QString& data);
	void updateHeaderIndicator(int column);
	void sortColumn(int column, Qt::SortOrder order);

private:
	void commitFilterData(int logicalIndex,  QObject* object);
	void closeFilterEditor();

	QWidget* m_filterWidget = nullptr;
	int m_filterColumnIndex = -1;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//==========================================================================================
class GridViewTable : public QTableView
{
	Q_OBJECT
public:
	explicit GridViewTable(QWidget* parent);

	IJDParameterPtr currentParam() const { return currentIndex().data(TableColumnProvider::ParameterRole).value<IJDParameterPtr>(); }

	// override standard function
	void sortByColumn(int column, Qt::SortOrder order);

signals:
	void updateHeaderIndicator(int column);

};


#endif // GRIDHEADERVIEW_H
