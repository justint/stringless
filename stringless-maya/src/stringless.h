/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   stringless.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on March 6, 2017, 12:42 AM
 */

#ifndef STRINGLESS_H
#define STRINGLESS_H

#include <QtCore/QPointer>
#include <QtGui/QPushButton>
#include <maya/MPxCommand.h>

class Stringless : public MPxCommand
{
public:
	static void		cleanup();
	static void*            creator()	{ return new Stringless(); }

	MStatus			doIt(const MArgList& args);

	static QPointer<QPushButton>    	button;
	static const MString			commandName;
};

#endif /* STRINGLESS_H */

