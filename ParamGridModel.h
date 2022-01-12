#ifndef PARAMGRIDMODEL_H
#define PARAMGRIDMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSortFilterProxyModel>

class ParamGridModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit ParamGridModel(QObject *parent = nullptr);

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public slots:
	void headerFilterData(int section, const QString& data);

private:
	QList<QList<QString>> m_data;

};

class GridFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	GridFilterProxyModel(QObject *parent = 0);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
	void updateFilter(int section, const QString& data);
	//void sortColumn(int logicalIndex, int sortIndicator){invalidateFilter();}
	friend class GridHeaderView;
private:
	QMap<int, QString> m_filterMap;

};

#endif // PARAMGRIDMODEL_H
