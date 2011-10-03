#ifndef BASE__PCH_H_
#define BASE__PCH_H_

/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */

//
// STL
//
# include <iostream>
# include <fstream>
# include <vector>
# include <set>
# include <map>
# include <string>
# include <algorithm>
# include <functional>
# include <utility>

//
// boost
//
# include <boost/utility.hpp>
# include <boost/array.hpp>
# include <boost/shared_ptr.hpp>
# include <boost/archive/xml_iarchive.hpp>
# include <boost/archive/xml_oarchive.hpp>
# include <boost/serialization/vector.hpp>
# include <boost/serialization/string.hpp>
# define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS		// needed for msm
# define BOOST_MPL_LIMIT_VECTOR_SIZE	50			// needed for msm
# define BOOST_MPL_LIMIT_MAP_SIZE		50			// needed for msm
# include <boost/msm/back/state_machine.hpp>
# include <boost/msm/front/state_machine_def.hpp>


//
// QT
//
# include <QObject>
# include <QString>
# include <QApplication>
# include <QPushButton>
# include <QLabel>
# include <QMessageBox>
# include <QFileDialog>
# include <QTimer>
# include <QSound>
# include <QFileInfo>
# include <QtGui/QApplication>
# include <QTranslator>
# include <QComboBox>
# include <QDesktopWidget>
# include <QFontDialog>
# include <QColorDialog>
# include <QInputDialog>
# include <QSettings>
# include <QTimer>
//# include <QSplashScreen>
# include <QPainter>
# include <QtGui>
# include <QStringList>
# include <QSize>
# include <QTime>
# include <QFontDatabase>
# include <QMenu>
# include <QDialog>
# include <QMainWindow>
# include <QTableWidgetItem>
# include <QtGui/QWidget>
# include <QImage>
# include <QResizeEvent>
# include <QFont>
# include <QAbstractItemModel>
# include <QLineEdit>
# include <QMouseEvent>
# include <QFile>
# include <QDir>

//
// custom
//
# include "../util/qstring_serialization.h"


#endif // __cplusplus

#endif	// BASE__PCH_H_
