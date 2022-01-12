#ifndef TABLEDATA_H
#define TABLEDATA_H

#include <comutil.h>
#include <QMessageBox>

namespace {
	template<class T>
	QString string_cast(const QString& val) {
		return val;
	}
}

class ErrorContainer {
public:
	QStringList errors() {return m_errors;}
	bool success(){return !m_errors.size();}
	static ErrorContainer* instance() {
		return m_instance;
	}
	void record(const QString& error) {
		m_errors << error;
	}
	void clearErrors() {
		if(m_errors.length()>0) {
			m_errors.clear();
		}
	}
	void errorMessageBox() {
		if(m_errors.size() > 0) {
			QMessageBox::warning(nullptr, "Error!!!!", m_errors.join("\n"));
		}
	}
private:
	QStringList m_errors;
	static ErrorContainer* m_instance;
};

#define jdactivex_scope (*ErrorContainer::instance())

#define jdactivex_error_clear \
	{ jdactivex_scope.clearErrors();}

#define jdactivex_try jdactivex_error_clear try

#define jdactivex_catch_quiet(quietError) \
	catch(char* catchedError) { jdactivex_scope.clearErrors(); \
								jdactivex_scope.record(quietError); \
								jdactivex_scope.record(catchedError);  }

#define jdactivex_catch_report(topError) jdactivex_catch_quiet(topError) \
								{ jdactivex_scope.errorMessageBox(); }

//==================================================================================
// 2D Table data -------------------------------------------------------------------
//==================================================================================

struct IJDNumericData {
	IJDNumericData(const IJDNumericData& newData) {
		m_data = newData.m_data;
	}
	IJDNumericData(const QString& data):m_data(data) {}
	void PutString(const QString& data){
		if(data.toDouble() == 0.00) {
			throw "Zerro value";
		}
		m_data=data;
	}
	double GetNumeric() {return m_data.toDouble();}
	QString GetString() {return m_data;}

private:
	QString m_data;
};

struct IJDDataPtr {
	IJDDataPtr(std::shared_ptr<IJDNumericData> numericData):m_pNumericData(numericData){}
	std::shared_ptr<IJDNumericData> numericData(){return m_pNumericData;}
private:
	std::shared_ptr<IJDNumericData> m_pNumericData;
};

struct DisplayFormat{};
struct DataFormat {
	static QString FormatValue(IJDDataPtr data, DisplayFormat displayFormat){
		Q_UNUSED(displayFormat)
		return QString::number(data.numericData()->GetNumeric());
	}
};

typedef std::vector<std::shared_ptr<IJDNumericData>> NumericDataArray;

class IJD2DFnTableData {
	IJD2DFnTableData() = default;
public:
	struct InOutData {
		InOutData(const QString& input, const QString& output):m_inputData(input),m_outputData(output) {}
		QString m_inputData;
		QString m_outputData;
	};

	IJD2DFnTableData(std::initializer_list<InOutData> data) {
		for(InOutData ob:data) {
			int idx = InsertInput(ob.m_inputData);
			GetOutputCellData(idx)->PutString(ob.m_outputData);
		}
	}

	int GetInputCount() {
		return m_inputCellData.size();
	}

	std::shared_ptr<IJDNumericData> GetInputCellData(int nIndex) {
		return m_inputCellData[nIndex];
	}

	std::shared_ptr<IJDNumericData> GetOutputCellData(int nIndex) {
		return m_outputCellData[nIndex];
	}

	int InsertInput(const QString& data) {
		if(data.toDouble() == 0.00) {
			throw "Zerro value";
		}
		m_inputCellData.push_back(std::make_shared<IJDNumericData>(data));
		m_outputCellData.push_back(std::make_shared<IJDNumericData>(""));
		return m_inputCellData.size() - 1;
	}

	void RemoveInput(int nRowIndex) {
		if(nRowIndex == 2) {
			// simulate JD exceptions
			//throw "Simulation of JD error!";
		}
		m_inputCellData.erase(m_inputCellData.begin() + nRowIndex);
		m_outputCellData.erase(m_outputCellData.begin() + nRowIndex);
	}

private:
	NumericDataArray m_inputCellData;
	NumericDataArray m_outputCellData;
};

#define IJD2DFnTableDataPtr IJD2DFnTableData*

//==================================================================================
// 3D Table data -------------------------------------------------------------------
//==================================================================================
typedef std::vector<std::shared_ptr<NumericDataArray>> ArrayOfNumericDataArrays;

struct IJDValueSource {};
typedef IJDValueSource* IJDValueSourcePtr;

struct IJDScalarType{
	double NumericToScaled(IJDValueSource * pValueSource, double nNumericValue) {
		Q_UNUSED(pValueSource)
		return nNumericValue;
	}
};
typedef IJDScalarType* IJDScalarTypePtr;

class IJD3DFnTableData {

public:
	~IJD3DFnTableData(){
		delete m_scalarType;
		delete m_valueSource;
	}
	int InsertInputX(const QString& data) {
		// simulate JDActiveX exception
		if(data.toDouble() == 0.00) {
			//throw "Zerro value";
		}
		m_inputXCellData.push_back(std::make_shared<IJDNumericData>(data));
		for(int i = 0; i<m_inputYCellData.size(); i++){
			std::shared_ptr<NumericDataArray> arrCells = m_outputData.at(i);
			arrCells->push_back(std::make_shared<IJDNumericData>(""));
		}
		return m_inputXCellData.size() - 1;
	}

	int InsertInputY(const QString& data) {
		// simulate JDActiveX exception
		if(data.toDouble() == 0.00) {
			throw "Zerro value";
		}
		m_inputYCellData.push_back(std::make_shared<IJDNumericData>(data));
		m_outputData.push_back(std::shared_ptr<NumericDataArray>(new NumericDataArray{}));
		std::shared_ptr<NumericDataArray> arrCells = m_outputData.at(m_outputData.size()-1);

		for(int i = 0; i<m_inputXCellData.size(); i++) {
			arrCells->push_back(std::make_shared<IJDNumericData>(""));
		}
		return m_inputYCellData.size() - 1;
	}

	std::shared_ptr<IJDNumericData> GetInputXCellData(int nColumnIndex) {
		return m_inputXCellData[nColumnIndex];
	}
	std::shared_ptr<IJDNumericData> GetInputYCellData(int nRowIndex) {
		return m_inputYCellData[nRowIndex];
	}
	std::shared_ptr<IJDNumericData> GetOutputCellData(int nRowIndex, int nColumnIndex) {
		return m_outputData[nRowIndex]->at(nColumnIndex);
	}

	int GetInputXCount(){
		return m_inputXCellData.size();
	}
	int GetInputYCount(){
		return m_inputYCellData.size();
	}

	IJDScalarType* GetInputXCellType(){return m_scalarType;}
	IJDScalarType* GetInputYCellType(){return m_scalarType;}
	IJDScalarType* GetOutputCellType(){return m_scalarType;}
	IJDValueSource* GetValueSource(){return m_valueSource;}

	void RemoveInputX(int columnIndex){
		m_inputXCellData.erase(m_inputXCellData.begin() + columnIndex);
		if(m_outputData.size() > columnIndex) {
			m_outputData[columnIndex]->erase(m_outputData[columnIndex]->begin() + columnIndex);
		}
	}
	void RemoveInputY(int rowIndex){
		if(rowIndex == 2) {
			// simulate JD exceptions
			throw "Simulation of JD error!";
		}
		m_inputYCellData.erase(m_inputYCellData.begin() + rowIndex);
		m_outputData.erase(m_outputData.begin() + rowIndex);
	}

private:
	NumericDataArray m_inputXCellData;
	NumericDataArray m_inputYCellData;
	ArrayOfNumericDataArrays m_outputData;

	IJDScalarType* m_scalarType = new IJDScalarType;
	IJDValueSource* m_valueSource = new IJDValueSource;
};

#define IJD3DFnTableDataPtr IJD3DFnTableData*

//==================================================================================
// Hysteresis Table data -----------------------------------------------------------
//==================================================================================
struct IJDHysteresisTableType{
	QString GetInputName(){return m_inputName;}
private:
	QString m_inputName = "VoltageHysTable";
};

typedef IJDHysteresisTableType* IJDHysteresisTableTypePtr;

class IJDHysteresisTableData {
public:
	IJDHysteresisTableData() = default;
	~IJDHysteresisTableData(){
		delete m_scalarType;
		delete m_valueSource;
	}

	IJDScalarType* GetUpDownCellType(){return m_scalarType;}
	IJDScalarType* GetOutputCellType(){return m_scalarType;}
	IJDValueSource* GetValueSource(){return m_valueSource;}

	std::shared_ptr<IJDNumericData> GetUpCellData(int nIndex) {
		return m_upCellData[nIndex];
	}
	std::shared_ptr<IJDNumericData> GetDownCellData(int nIndex) {
		return m_downCellData[nIndex];
	}
	std::shared_ptr<IJDNumericData> GetOutputCellData(int nIndex) {
		return m_outputCellData[nIndex];
	}

	int GetUpDownRowCount() {
		return m_upCellData.size();
	}

	IJDHysteresisTableTypePtr GetType(){return m_hysteresisTableType;}

	int InsertRow(const QString& bstrScaledUpValue, const QString& bstrScaledDownValue) {
		if(bstrScaledUpValue.toDouble() == 0.00 || bstrScaledDownValue.toDouble() == 0.00) {
			throw "Zerro value";
		}
		m_upCellData.push_back(std::make_shared<IJDNumericData>(bstrScaledUpValue));
		m_downCellData.push_back(std::make_shared<IJDNumericData>(bstrScaledDownValue));
		m_outputCellData.insert(m_outputCellData.end()-1, std::make_shared<IJDNumericData>(""));
		return m_upCellData.size() - 1;
	}

	void RemoveRow(int row) {
		if(row == 2) {
			// simulate JD exceptions
			throw "Simulation of JD error!";
		}
		m_upCellData.erase(m_upCellData.begin() + row);
		m_downCellData.erase(m_downCellData.begin() + row);
		m_outputCellData.erase(m_outputCellData.begin() + row);
	}

private:
	NumericDataArray m_upCellData = {};
	NumericDataArray m_downCellData = {};
	NumericDataArray m_outputCellData = {std::make_shared<IJDNumericData>("0")};

	IJDScalarType* m_scalarType = new IJDScalarType;
	IJDValueSource* m_valueSource = new IJDValueSource;

	IJDHysteresisTableTypePtr m_hysteresisTableType = new IJDHysteresisTableType;
};

typedef IJDHysteresisTableData* IJDHysteresisTableDataPtr;
typedef IJDScalarType* IJDStateTypePtr;

#include <QTableView>
#include <QKeyEvent>
class DataTable : public QTableView
{
	Q_OBJECT

public:
	DataTable(QWidget *parent = nullptr) : QTableView(parent) {}
	DisplayFormat displayFormat() {return m_displayFormat;}

protected:
	void keyPressEvent(QKeyEvent *event) override {
		QTableView::keyPressEvent(event);
		const QModelIndex  index = currentIndex();
		if( (editTriggers() & QAbstractItemView::EditKeyPressed) &&
				(index.row() == model()->rowCount() - 1 || index.column() == model()->columnCount() - 1)) {
			emit startEdit(index);
		}
	}
	QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override {
		if(pendingColumn) {
			switch (cursorAction) {
				case MoveNext: {
					QModelIndex nextIndex = QTableView::moveCursor(MoveDown, modifiers);
					return nextIndex == currentIndex() ? QModelIndex() : nextIndex;
				}
				case MovePrevious: {
					QModelIndex prevIndex = QTableView::moveCursor(MoveUp, modifiers);
					return prevIndex == currentIndex() ? QModelIndex() : prevIndex;
				}
			}
		}
		return QTableView::moveCursor(cursorAction, modifiers);
	}

public slots:
	void setPendingColumn(bool pending) {
		pendingColumn = pending;
	}

	void currentChanged(const QModelIndex &current, const QModelIndex &previous) override {
		QTableView::currentChanged(current, previous);
		emit currentIndexChanged();
	}
signals:
	void startEdit(const QModelIndex& index);
	void currentIndexChanged();

private:
	DisplayFormat m_displayFormat;
	bool pendingColumn = false;
};

#include <QLineEdit>
#include <QComboBox>

struct IJDConfig{};
typedef IJDConfig* IJDConfigPtr;
struct IJDType {
	IJDType(int type):m_type(type){}
	int m_type = 0;
};

struct IJDParameter {
	IJDConfigPtr GetConfig() {return nullptr;}
	QString GetName() {return "";}
};

namespace TableColumnProvider {
	static int ParameterRole = Qt::UserRole + 1;
	static int EffectiveTypeRole = ParameterRole + 1;
}

typedef  IJDType* IJDTypePtr;
typedef IJDParameter* IJDParameterPtr;

#include <QTextEdit>
class MapInputG4Editor: public QTextEdit
{
	Q_OBJECT
public:
	MapInputG4Editor(QWidget *parent = 0) : QTextEdit(parent){}
};

#include <QCompleter>
#include <QComboBox>

namespace DataTableDelegate {
	static QWidget* createDataEditorImpl(const QString &value, const QString &caption, IJDType *type, IJDConfig *config = nullptr, bool initFromCell = true, QWidget *parent = nullptr)  {
		if(type->m_type == 0) {
			QLineEdit* lineEdit = new QLineEdit(parent);
			lineEdit->setText(value);
			return lineEdit;
		} else if(type->m_type == 1) {
			QComboBox* comboBox = new QComboBox(parent);
			comboBox->setEditable(true);
			comboBox->addItems({"1","2","3","4","5"});
			comboBox->setEditText(value);

			comboBox->setInsertPolicy(QComboBox::NoInsert);
			comboBox->completer()->setCompletionMode(QCompleter::InlineCompletion);

			QMetaObject::invokeMethod(comboBox, &QComboBox::showPopup, Qt::QueuedConnection);
			return comboBox;
		} else if(type->m_type == 3) {
			MapInputG4Editor* editor = new MapInputG4Editor(parent);
			editor->setText(value);
			return editor;
		}
		return nullptr;
	}
	static QString getContentFromEditor(QWidget *editor) {
		QLineEdit *le = ::qobject_cast<QLineEdit*>(editor);
		if ( le)
			return le->text();

		QTextEdit *te = ::qobject_cast<QTextEdit*>(editor);
		if (te)
			return te->toPlainText();

		QComboBox *cb = ::qobject_cast<QComboBox*>(editor);
		if ( cb)
			return cb->currentText();

		return QString();
	}
};

Q_DECLARE_METATYPE(IJDParameterPtr)
Q_DECLARE_METATYPE(IJDTypePtr)

class ParamGrid
{
public:
	ParamGrid(const QStringList &suffixes) {
		for(QStringList::const_iterator ni = suffixes.constBegin(); ni != suffixes.constEnd(); ++ni) {
			m_suffixes.push_back(heading(*ni,m_suffixes.size()));
		}
	}
	QString suffix(uint i) const { return (i >= 0 && i < m_suffixes.size() ? m_suffixes.at(i).name : QString()); }
	int numCols() { return m_suffixes.size(); }

private:
	struct heading {
		heading(const QString &n, int i) : name(n),idx(i) {}
		int idx;
		QString name;
		operator QString() const { return name; }
	};
	std::vector<heading> m_suffixes;
};

#endif // TABLEDATA_H
