
#include "OscMapper.h"
namespace MarsyasQt
{
OscMapper::OscMapper(QHostAddress inputHost, quint16 inputPort, QHostAddress outputHost, quint16 outputPort, QObject* p, MarSystemQtWrapper *mwr)
{
	mwr_=mwr;

	oscServer_ = new QOscServer (inputHost, inputPort, p);
	QObject::connect(oscServer_, SIGNAL( data( QString, QVariant ) ), this, SLOT( updctrl (QString, QVariant)));

	oscClient_ = new QOscClient (outputHost, outputPort, p);
	QObject::connect(mwr_, SIGNAL( ctrlChanged ( MarControlPtr ) ), this, SLOT( ctrlChanged ( MarControlPtr )));
}

OscMapper::~OscMapper()
{
	delete oscServer_;
	delete oscClient_;
}

void OscMapper::registerInputQtSlot(QObject *object, QString path, QVariant::Type type)
{
	PathObject* integerobj = new PathObject(path, type, oscServer_);

	switch(type)
	{
	case QVariant::Bool:
		QObject::connect( integerobj, SIGNAL( data( bool ) ), object, SLOT( setValue( bool ) ) );
		break;
	case QVariant::Int:
		QObject::connect( integerobj, SIGNAL( data( int ) ), object, SLOT( setValue( int ) ) );
		break;
	case QVariant::Double:
		QObject::connect( integerobj, SIGNAL( data( double ) ), object, SLOT( setValue( double ) ) );			
		break;
	case QVariant::String:
		QObject::connect( integerobj, SIGNAL( data( string ) ), object, SLOT( setValue( string ) ) );		
		break;
	}

}

void OscMapper::registerOutputQtSlot(QObject *object, QString path, QVariant::Type type)
{
    PathObject* integerobj = new PathObject(path, type, oscClient_);

	switch(type)
	{
	case QVariant::Bool:
		QObject::connect( object, SIGNAL( valueChanged( bool ) ), integerobj, SLOT( send( bool ) ) );
		break;
	case QVariant::Int:
		QObject::connect( object, SIGNAL( valueChanged( int ) ), integerobj, SLOT( send( int ) ) );
		break;
	case QVariant::Double:
		QObject::connect( object, SIGNAL( valueChanged( double ) ), integerobj, SLOT( send( double ) ) );			
		break;
	case QVariant::String:
		QObject::connect( object, SIGNAL( valueChanged( string ) ), integerobj, SLOT( send( string ) ));		
		break;
	}
}
} //namespace

