
#include "OscMapper.h"

OscMapper::OscMapper(quint16 port, QObject* p, MarSystemQtWrapper *mwr)
{
	oscServer_ = new QOscServer (port, p);
	mwr_=mwr;

	QObject::connect(oscServer_, SIGNAL( data( QString, QVariant ) ), this, SLOT(convertMessages (QString, QVariant)));

}

OscMapper::~OscMapper()
{
	delete oscServer_;
}

void OscMapper::registerQtSlot(QObject *object, QString path, QVariant::Type type)
{
	PathObject* integerobj = new PathObject(path, type, oscServer_);

	switch(type)
	{
	case QVariant::Type::Bool:
		QObject::connect( integerobj, SIGNAL( data( bool ) ), object, SLOT( setValue( bool ) ) );
		break;
	case QVariant::Type::Int:
		QObject::connect( integerobj, SIGNAL( data( int ) ), object, SLOT( setValue( int ) ) );
		break;
	case QVariant::Type::Double:
		QObject::connect( integerobj, SIGNAL( data( double ) ), object, SLOT( setValue( double ) ) );			
		break;
	case QVariant::Type::String:
		QObject::connect( integerobj, SIGNAL( data( string ) ), object, SLOT( setValue( string ) ) );		
		break;
	}

}

