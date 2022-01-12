#include "ParamGridModel.h"
#include <QMessageBox>
#include <QIcon>
#include <QDebug>

#include "TableData.h"

namespace {
	static IJDParameter param;
	static IJDType type1(1);
	static IJDType type2(0);
	static IJDType type3(3);
}

ParamGridModel::ParamGridModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	m_data.append({"parameter2", "1", "4bbc", "podcast8"});
	m_data.append({"parameter0", "3", "1bbc", "podcastcc"});
	m_data.append({"parameter1", "3", "bbc", "podcast7"});
	m_data.append({"parameter8", "2", "7bbc", "podcast0"});
}

int ParamGridModel::rowCount(const QModelIndex &parent) const
{
	return m_data.size();
}

int ParamGridModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

QModelIndex ParamGridModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row,column);
}

QVariant ParamGridModel::data(const QModelIndex &index, int role) const
{
	if(role == TableColumnProvider::ParameterRole) {
		IJDParameter* parameter = &param;
		QString name = parameter->GetName();
		return QVariant::fromValue<IJDParameterPtr>(&param);
	} else  if(role == TableColumnProvider::EffectiveTypeRole) {
		if(index.column() == 1) {
			return QVariant::fromValue<IJDTypePtr>(&type1);
		} else if(index.column() == 2){
			return QVariant::fromValue<IJDTypePtr>(&type2);
		} else if(index.column() == 3) {
			return QVariant::fromValue<IJDTypePtr>(&type3);
		}
	} else if(role == Qt::DisplayRole) {
		return m_data[index.row()][index.column()];
	}

	return {};
}

QVariant ParamGridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return {};//QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags ParamGridModel::flags(const QModelIndex &index) const
{
	return QAbstractTableModel::flags(index);
}

bool ParamGridModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return QAbstractTableModel::setData(index, value, role);
}

void ParamGridModel::headerFilterData(int section, const QString &data)
{
	//QMessageBox::warning(nullptr, "Data from Header", QString("Signal from section: %1 \n data: ").arg(section) + data);
	qDebug()<<QString("Signal from section: %1 \n data: ").arg(section) + data;
}

GridFilterProxyModel::GridFilterProxyModel(QObject *parent):
	QSortFilterProxyModel(parent)
{
}

bool GridFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QMapIterator<int, QString> it(m_filterMap);
	while(it.hasNext()) {
		it.next();
		QVariant data = sourceModel()->data(sourceModel()->index(sourceRow, it.key()), Qt::DisplayRole);
		if(!data.isValid()) {
			return false;
		}
		if(!data.toString().contains(it.value())) {
			return false;
		}
	}
	return true;
}

void GridFilterProxyModel::updateFilter(int section, const QString &data)
{
	if(data.isEmpty()) {
		m_filterMap.remove(section);
	} else {
		m_filterMap.insert(section, data);
	}
	invalidateFilter();
}

bool GridFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
}

QVariant GridFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
		return section+1;
	}
	return QSortFilterProxyModel::headerData(section, orientation, role);
}
