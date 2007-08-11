
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
	OscMapper(QHostAddress inputHost, quint16 inputPort, QHostAddress outputHost, quint16 outputPort, QObject* p, MarSystemQtWrapper *mwr);
	~OscMapper();

	void registerInputQtSlot(QObject *object, QString path, QVariant::Type type);

	void registerOutputQtSlot(QObject *object, QString path, QVariant::Type type);

	public slots:
		void updctrl(QString path, QVariant data)
		{

			string control = path.toStdString();
			control.erase(0, 1);
			MarControlPtr cval=NULL;
			QString tmp;

			QVariant::Type targetType;
			if(path.contains("mrs_natural/"))
				data.convert(QVariant::Type::Int);
			else if(path.contains("mrs_bool/"))
				data.convert(QVariant::Type::Bool);
			else if(path.contains("mrs_string/"))
				data.convert(QVariant::Type::String);
			else if(path.contains("mrs_real/"))
				data.convert(QVariant::Type::Double);
			else if(path.contains("mrs_realvec/"))
				data.convert(QVariant::Type::List);

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
				tmp = data.toString();
				cval = tmp.toStdString();
				break;
			case QVariant::Type::List:
				mrs_natural i=0;
				QList<QVariant> list = data.toList();
				realvec vec(list.size());

				QList<QVariant>::iterator vsi;
				for (vsi = list.begin();
					vsi != list.end(); ++vsi)
				{
					vec(i++) = (*vsi).toDouble();
				}
				cval = vec;
				break;
			}
			mwr_->updctrl(control, cval);
		}

		void ctrlChanged(MarControlPtr ctrl)
		{
			QString oscPath("/");
			oscPath+=QString().fromStdString(ctrl->getName());

			QVariant oscData;
			if(oscPath.contains("mrs_natural/"))
				oscData = ctrl->toNatural();
			else if(oscPath.contains("mrs_bool/"))
				oscData = ctrl->toBool();
			else if(oscPath.contains("mrs_string/"))
				oscData = QString().fromStdString(ctrl->toString());
			else if(oscPath.contains("mrs_realvec/"))
			{
				realvec vec = ctrl->toVec();
				QList<QVariant> list;
				for (mrs_natural i=0 ; i< vec.getSize() ; i++)
					list.push_back(vec(i));
				oscData = QVariant(list);
			}
			else if(oscPath.contains("mrs_real/"))
				oscData = ctrl->toReal();

			oscClient_->sendData(oscPath, oscData);
		}

private:
	MarSystemQtWrapper *mwr_;
	QOscServer *oscServer_;
	QOscClient *oscClient_;
};

#endif