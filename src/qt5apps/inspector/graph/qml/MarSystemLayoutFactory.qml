import QtQuick 2.0

Item {
    Item {
        id: private_data
        property var layouts: {
            'Series':  Qt.createComponent("SeriesLayout.qml"),
                    'Fanout': Qt.createComponent("FanoutLayout.qml"),
                    'Fanin': Qt.createComponent("FanoutLayout.qml"),
                    'FanOutIn': Qt.createComponent("FanoutLayout.qml"),
                    'Parallel': Qt.createComponent("FanoutLayout.qml"),
        }
    }

    function get( type )
    {
        var component = private_data.layouts[type];
        if (component == undefined)
            component = private_data.layouts["Series"];
        return component;
    }
}
