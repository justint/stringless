/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   StringlessMayaCmd.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on April 28, 2017, 7:32 PM
 */

#ifndef STRINGLESSMAYACMD_H
#define STRINGLESSMAYACMD_H

#include <maya/MPxCommand.h>
#include "StringlessMayaDevice.h"

class StringlessMayaCmd : public MPxCommand {
public:
    StringlessMayaCmd() { 
        mDeviceName = "";
        mStart      = false;
    };
    virtual ~StringlessMayaCmd();
    
    MStatus             doIt( const MArgList& args );
    static void*        creator();
    
    static MSyntax      newSyntax();
private:
    MStatus             parseArgs( const MArgList& args );
    //static SOCKET_REF   socketInfo;
    
    MString             mDeviceName;
    bool                mStart;
    
    static constexpr char*  kStartFlag = "-s";
    static constexpr char*  kStartFlagLong = "-start";
    static constexpr char*  kDeviceNameFlag = "-dn";
    static constexpr char*  kDeviceNameFlagLong = "-device name";
};

#endif /* STRINGLESSMAYACMD_H */

