/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   StringlessMayaCmd.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 * 
 * Created on April 28, 2017, 7:32 PM
 */

#include <maya/MFnDependencyNode.h>
#include <maya/MIOStream.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

#include "StringlessMayaCmd.h"

#include "../../src/Reader.h"
#include "../../src/MemoryManager.h"

StringlessMayaCmd::~StringlessMayaCmd() { }

void* StringlessMayaCmd::creator() {
    return new StringlessMayaCmd();
}

MSyntax StringlessMayaCmd::newSyntax() {
    MSyntax syntax;
    syntax.addFlag(kStartFlag, kStartFlagLong, MSyntax::kBoolean);
    syntax.addFlag(kDeviceNameFlag, kDeviceNameFlagLong, MSyntax::kString);
    return syntax;
}

MStatus StringlessMayaCmd::parseArgs(const MArgList& args) {
    MStatus status = MStatus::kSuccess;
    MArgDatabase argData(syntax(), args);
    
    if(argData.isFlagSet(kStartFlag))
        status = argData.getFlagArgument(kStartFlag, 0, mStart);
    
    if(argData.isFlagSet(kDeviceNameFlag))
        status = argData.getFlagArgument(kDeviceNameFlag, 0, mDeviceName);
    
    return status;
}

MStatus StringlessMayaCmd::doIt(const MArgList& args) {
    MStatus status;
    
    status = parseArgs(args);
    if(status != MStatus::kSuccess) {
        MGlobal::displayError("Parameters are not correct.");
        return status;
    }
    
    MSelectionList sl;
    sl.add(mDeviceName);
    MObject deviceObj;
    status = sl.getDependNode(0, deviceObj);
    if(status != MStatus::kSuccess) {
        MGlobal::displayError("Please create your device first.");
        return status;
    }
    MFnDependencyNode fnDevice(deviceObj);
    //MString ip = fnDevice.findPlug("inputIp", &status).asString();
    
    if(mStart) {
        /*
         * 
         *  do the thing
         * 
         */
        
        
        
    } else {
        /*
         * 
         * stop the thing
         * 
         */
    }
    
    return MStatus::kSuccess;
}

