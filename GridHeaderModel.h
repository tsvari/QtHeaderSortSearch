#ifndef GRIDHEADERMODEL_H
#define GRIDHEADERMODEL_H

#include <QAbstractItemModel>
#include "TableData.h"

class GridHeaderModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit GridHeaderModel(QObject *parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

	int rowCount(const QModelIndex&) const override {return 0;}
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex&, int) const override {return {};}
	QModelIndex index(int, int, const QModelIndex&) const override {return {};}
	QModelIndex parent(const QModelIndex&) const override {return {};}

	class SectionItem {
	public:
		SectionItem(const QString& displayText, const QModelIndex& index);
		QVariant data(int role) const {
			return m_itemData.value(role);
		}
		bool setData(const QVariant &value, int role) {
			if(value.isValid()) {
				m_itemData.insert(role,value);
			} else {
				m_itemData.remove(role);
			}
			return true;
		}
	private:
		QMap<int,QVariant> m_itemData;
	};
	void appendSection(const QString &displayText, const QModelIndex& index);

public slots:
	void updateIndicator(int logicalIndex);

private:
	QList<SectionItem*> m_sectionData;

};

#endif // GRIDHEADERMODEL_H
