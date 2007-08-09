
#ifndef OSCWRAPPER_H
#define OSCWRAPPER_H

#include "MarSystemQtWrapper.h"
#include "qoscserver.h"
#include "qoscclient.h"


using namespace std;
using namespace Marsyas;

class OscMapper: public QObject
{
	Q_OBJECT

    public:
	OscMapper(quint16 port, QObject* p, MarSystemQtWrapper *mwr);
	~OscMapper();

	void registerQtSlot(QObject *object, QString path, QVariant::Type type);

	public slots:
		void convertMessages(QString path, QVariant data)
		{

			string control = path.toStdString();
			control.erase(0, 1);
			MarControlPtr cval=NULL;

			QVariant::Type targetType;
			if(path.contains("mrs_natural"))
				data.convert(QVariant::Type::Int);
			else if(path.contains("mrs_bool"))
				data.convert(QVariant::Type::Bool);
			else if(path.contains("mrs_string"))
				data.convert(QVariant::Type::String);

			switch(data.type())
			{
			case QVariant::Type::Bool:
				cval = data.toBool();
				break;
			case QVariant::Type::Int:
				cval = data.toInt();
				break;
			case QVariant::Type::Double:
				cval = data.toDouble();
				break;
			case QVariant::Type::String:
				QString tmp = data.toString();
				cval = tmp.toStdString();
				break;
			}
			mwr_->updctrl(control, cval);
		}

private:
	MarSystemQtWrapper *mwr_;
	QOscServer *oscServer_;
	QOscClient *oscClient_;
};

#endif