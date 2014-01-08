find_package(Qt5Xml 5.1 REQUIRED)

set(APP
  MarGrid2
)

set(APP_SOURCES
  MainWindow.cpp
  Colormaps/GreyScaleColormap.cpp
  Colormaps/SpectraColormap.cpp
  Colormaps/Colormap.cpp
  Dialogs/iTunesPage.cpp
  Dialogs/PreferencesDialog.cpp
  Interface/Parser.cpp
  Logger/Logger.cpp
  Midi/MidiListener.cpp
  Music/MusicCollection.cpp
  Music/MusicLink.cpp
  Music/MusicTrack.cpp
  Parsers/iTunesXmlHandler.cpp
  Parsers/iTunesXmlWriter.cpp
  Widgets/Grid.cpp
  Widgets/OSCReceiver.cpp
  Widgets/Keypad.cpp
  Widgets/PlayBox.cpp
  Widgets/Playlist.cpp
  Widgets/PlaylistItem.cpp
  Widgets/Tracklist.cpp
  Widgets/GridDisplay.cpp
  Widgets/ColourMapDisplay.cpp
  Widgets/SquareRenderArea.cpp
  Widgets/OptionsDialog.cpp
  main.cpp
)

set(APP_HEADERS
  Dialogs/PreferencesDialog.h
  Interface/MyDisplay.h
  Midi/MidiListener.h
  Widgets/Grid.h
  Widgets/OSCReceiver.h
  Widgets/Keypad.h
  Widgets/Playlist.h
  Widgets/GridDisplay.h
  Widgets/ColourMapDisplay.h
  Widgets/SquareRenderArea.h
  Widgets/OptionsDialog.h
  MainWindow.h
)

set(APP_RESOURCES
  MarGrid2.qrc
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )
qt5_add_resources( APP_RESOURCES_SRC ${APP_RESOURCES} )

marsyas_qt5_app( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} ${APP_RESOURCES_SRC} )

qt5_use_modules( ${APP} Widgets Xml )
