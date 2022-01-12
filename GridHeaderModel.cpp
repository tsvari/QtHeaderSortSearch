#include "GridHeaderModel.h"
#include <QIcon>

#include "TableData.h"

GridHeaderModel::GridHeaderModel(QObject *parent)
	: QAbstractItemModel(parent)
{
}

QVariant GridHeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal) {
		if(role == Qt::SizeHintRole) {
			return QSize(0, 60);
		}
		return m_sectionData[section]->data(role);
	}
	return {};
}

bool GridHeaderModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if(orientation == Qt::Horizontal) {
		return m_sectionData[section]->setData(value, role);
	}

	return false;
}

int GridHeaderModel::columnCount(const QModelIndex &parent ) const
{
	Q_UNUSED(parent)

	return m_sectionData.size();
}

void GridHeaderModel::appendSection(const QString &displayText, const QModelIndex& index)
{
	m_sectionData.append(new SectionItem(displayText, index));
}

void GridHeaderModel::updateIndicator(int logicalIndex)
{
	int sortIndicator = m_sectionData[logicalIndex]->data(Qt::InitialSortOrderRole).toInt();
	if(sortIndicator == QStyleOptionHeader::None || sortIndicator == QStyleOptionHeader::SortDown) {
		sortIndicator = QStyleOptionHeader::SortUp;
	} else {
		sortIndicator = QStyleOptionHeader::SortDown;
	}

	for(int section = 0; section < columnCount(); section++) {
		setHeaderData(section, Qt::Horizontal, (logicalIndex==section)?sortIndicator:QStyleOptionHeader::None, Qt::InitialSortOrderRole);
	}
}

GridHeaderModel::SectionItem::SectionItem(const QString &displayText, const QModelIndex &index)
{
	if(index.isValid()) {
		m_itemData.insert(Qt::DisplayRole, displayText);
		m_itemData.insert(Qt::EditRole, "");
		m_itemData.insert(Qt::InitialSortOrderRole, QStyleOptionHeader::None);

		QVariant paramVariant = index.data(TableColumnProvider::ParameterRole);
		if(paramVariant.isValid()) {
			m_itemData.insert(TableColumnProvider::ParameterRole, paramVariant);
		}

		QVariant typeVariant = index.data(TableColumnProvider::EffectiveTypeRole);
		if(typeVariant.isValid()) {
			m_itemData.insert(TableColumnProvider::EffectiveTypeRole, typeVariant);
		}
	}
}
