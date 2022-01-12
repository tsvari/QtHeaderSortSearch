#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ParamGridModel.h"
#include "GridHeaderView.h"
#include "GridHeaderModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	tblValues = new GridViewTable(this);

	GridHeaderView* gridHeader = new GridHeaderView(tblValues);
	ParamGridModel* tableModel = new ParamGridModel(this);
	GridFilterProxyModel* proxyModel = new GridFilterProxyModel(this);
	GridHeaderModel* headerModel = new GridHeaderModel(gridHeader);

	QModelIndex columnIndex1 = tableModel->index(0, 1);
	QModelIndex columnIndex2 = tableModel->index(0, 2);
	QModelIndex columnIndex3 = tableModel->index(0, 3);

	headerModel->appendSection("", QModelIndex());
	headerModel->appendSection("11", columnIndex1);
	headerModel->appendSection("22", columnIndex2);
	headerModel->appendSection("33", columnIndex3);

	headerModel->setHeaderData(0, Qt::Horizontal, QIcon{":icons/filterHeader.png"}, Qt::DecorationRole);
	gridHeader->setModel(headerModel);
	proxyModel->setSourceModel(tableModel);

	// Table view gets filtered data to sort/filter data in tables
	connect(gridHeader, &GridHeaderView::updateFilterData, proxyModel, &GridFilterProxyModel::updateFilter);
	connect(gridHeader, &GridHeaderView::sortColumn, proxyModel, &GridFilterProxyModel::sort);

	tblValues->setModel(proxyModel);
	tblValues->setHorizontalHeader(gridHeader);
	tblValues->sortByColumn(0, Qt::AscendingOrder);

	ui->centralwidget->layout()->addWidget(tblValues);
}

MainWindow::~MainWindow()
{
	delete ui;
}

