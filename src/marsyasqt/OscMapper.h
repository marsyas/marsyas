
#ifndef OSCWRAPPER_H
#define OSCWRAPPER_H

#include "MarSystemQtWrapper.h"
#include "qoscserver.h"
#include "qoscclient.h"


using namespace std;
using namespace Marsyas;

namespace MarsyasQt
{
/**
	\brief Interface between OSC and Marsyas and Qt
	\ingroup MarsyasQt

	Long description
*/
class OscMapper: public QObject
{
	Q_OBJECT
public:

	// create an OscMapper (both osc client/server) attached to the specified MarSystemQtWrapper
	OscMapper(QHostAddress inputHost, quint16 inputPort, QHostAddress outputHost, quint16 outputPort, QObject* p, MarSystemQtWrapper *mwr);
	~OscMapper();

	// link a specific Qt Object with the reception of Osc messages of the specfied path
	void registerInputQtSlot(QObject *object, QString path, QVariant::Type type);

	// link a specific Qt Object with the emmission of Osc messages of the specified path
	// Notice : assumming that the valuechanged () signal is send by the Qt Object
	void registerOutputQtSlot(QObject *object, QString path, QVariant::Type type);

	public slots:

		// convert Osc messages recieved by the server into Marsyas paths and values
		void updctrl(QString path, QVariant data)
		{

			string control = path.toStdString();
			control.erase(0, 1);
			MarControlPtr cval;
			QString tmp;

			QVariant::Type targetType;
			if(path.contains("mrs_natural/"))
				data.convert(QVariant::Int);
			else if(path.contains("mrs_bool/"))
				data.convert(QVariant::Bool);
			else if(path.contains("mrs_string/"))
				data.convert(QVariant::String);
			else if(path.contains("mrs_real/"))
				data.convert(QVariant::Double);
			else if(path.contains("mrs_realvec/"))
				data.convert(QVariant::List);

			switch(data.type())
			{
			case QVariant::Bool:
				cval = data.toBool();
				break;
			case QVariant::Int:
				cval = data.toInt();
				break;
			case QVariant::Double:
				cval = data.toDouble();
				break;
			case QVariant::String:
				tmp = data.toString();
				cval = tmp.toStdString();
				break;
			case QVariant::List:
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
			cout << cval;
			mwr_->updctrl(control, cval);
		}

		// convert Marsyas path and values into Qt formats to be sent to the Osc client
		void ctrlChanged(MarControlPtr ctrl)
		{
			QString oscPath("/");
			oscPath+=QString().fromStdString(ctrl->getName());

			QVariant oscData;
			if(oscPath.contains("mrs_natural/"))
				oscData = (int) ctrl->to<mrs_natural>();
			else if(oscPath.contains("mrs_bool/"))
				oscData = ctrl->to<mrs_bool>();
			else if(oscPath.contains("mrs_string/"))
				oscData = QString().fromStdString(ctrl->to<mrs_string>());
			else if(oscPath.contains("mrs_realvec/"))
			{
				realvec vec = ctrl->to<mrs_realvec>();
				QList<QVariant> list;
				for (mrs_natural i=0 ; i< vec.getSize() ; i++)
					list.push_back(vec(i));
				oscData = QVariant(list);
			}
			else if(oscPath.contains("mrs_real/"))
				oscData = ctrl->to<mrs_real>();

			oscClient_->sendData(oscPath, oscData);
		}

private:
	MarSystemQtWrapper *mwr_;
	QOscServer *oscServer_;
	QOscClient *oscClient_;
};
} //namespace
#endif
