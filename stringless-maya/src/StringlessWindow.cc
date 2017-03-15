/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   StringlessWindow.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 16, 2017, 4:35 PM
 */

#include "StringlessWindow.h"

#include <QFile>
#include <QFormBuilder>
#include <QtDesigner>
#include <QtUiTools/QUiLoader>
#include <QVBoxLayout>
#include <QWidget>

#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MQtUtil.h>
#include <maya/MSimple.h>

#include "FrameData.h"
#include "MemoryManager.h"
#include "Reader.h"


/*
// Use a Maya macro to setup a simple hello class
// with methods for initialization, etc.
//
DeclareSimpleCommand( hello, PLUGIN_COMPANY, "4.5");

// All we need to do is supply the doIt method
// which in this case only prints "Hello" followed
// by the first argument given in the command line.
//
MStatus hello::doIt( const MArgList& args )
{

	cout<<"Hello "<<args.asString(0).asChar()<<endl;
	return MS::kSuccess;
}
*/

void StringlessWindow::cleanup() {
    if (!stringlessWindow.isNull()) delete stringlessWindow;
}

QPointer<StringlessDialog>  StringlessWindow::stringlessWindow;

const MString               StringlessWindow::commandName("stringless");

StringlessDialog::StringlessDialog(QWidget* parent) : 
QWidget(parent) {
    QUiLoader   loader;
    QFile       file(":/stringless.ui");
    
    
    file.open(QFile::ReadOnly);
    fForm = loader.load(&file, this);
    file.close();
    
    if (fForm) {
        cout << "UI file loaded" << endl;
        //	Destroy the dialog when it is closed.
        fForm->setAttribute(Qt::WA_DeleteOnClose, true);
        
        fLabel = fForm->findChild<QLabel*>("label_3");
        fStartButton = fForm->findChild<QPushButton*>("startButton");
        fStartButton->setEnabled(true);
        
        fLabel->setText("Click the Start button to grab a frame");
        
        connect(fStartButton, SIGNAL(released()), this, SLOT(start()));
                
        fForm->show();
        
    } else {
        cout << "Unable to load UI file" << endl;
    }
}

void StringlessDialog::accept() {
    cout << "User pressed accept" << endl;
}

void StringlessDialog::start() {
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 2;

    Stringless::MemoryManager memory_manager(shared_memory_name,
                                             shared_memory_size,
                                             Stringless::MemoryManager::read);

    if (memory_manager.init()) {
        cout << "Memory manager failure" << endl;
        return;
    }

    Stringless::Reader reader((Stringless::FrameData*)memory_manager.address());
    const Stringless::FrameData *incoming_data;
    incoming_data = reader.read();

    QString labelText = "Point 0 x: ";
    char point[10];
    sprintf(point, "%f", incoming_data[0].points[0].x);
    labelText.append(point);

    fLabel->setText(labelText);
}


MStatus StringlessWindow::doIt(const MArgList& /* args */)
{
    if (stringlessWindow.isNull()) {
        stringlessWindow = new StringlessDialog();
    } else {
        stringlessWindow->showNormal();
        stringlessWindow->raise();
    }
    /*
    //	Create a window containing a HelixButton, if one does not already
	//	exist. Otherwise just make sure that the existing window is visible.
	if (button.isNull()) {
		button = new QPushButton("Hello World");
		//button->connect(button, SIGNAL(clicked(bool)), button, SLOT(createHelix(bool)));
		button->show();
	}
	else {
		button->showNormal();
		button->raise();
	}
     */

	return MS::kSuccess;
}

MStatus initializePlugin(MObject plugin)
{
	MStatus		st;
	MFnPlugin	pluginFn(plugin, "Justin Tennant", "1.0", "Any", &st);

	if (!st) {
		MGlobal::displayError(
			MString("stringless - could not initialize plugin: ")
			+ st.errorString()
		);
		return st;
	}

	//	Register the command.
	st = pluginFn.registerCommand(StringlessWindow::commandName, 
                                      StringlessWindow::creator);

	if (!st) {
		MGlobal::displayError(
			MString("stringless - could not register '")
			+ StringlessWindow::commandName + "' command: "
			+ st.errorString()
		);
		return st;
	}

	return st;
}


MStatus uninitializePlugin(MObject plugin)
{
	MStatus		st;
	MFnPlugin	pluginFn(plugin, "Justin Tennant", "1.0", "Any", &st);

	if (!st) {
		MGlobal::displayError(
			MString("stringless - could not uninitialize plugin: ")
			+ st.errorString()
		);
		return st;
	}

	//	Make sure that there is no UI left hanging around.
	StringlessWindow::cleanup();

	//	Deregister the command.
	st = pluginFn.deregisterCommand(StringlessWindow::commandName);

	if (!st) {
		MGlobal::displayError(
			MString("stringless - could not deregister '")
			+ StringlessWindow::commandName + "' command: "
			+ st.errorString()
		);
		return st;
	}

	return st;
}
