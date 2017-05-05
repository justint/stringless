/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#define MNoVersionString
#include <maya/MFnPlugin.h>
#undef MNoVersionString

#include "StringlessMayaDevice.h"
#include "StringlessMayaCmd.h"

MStatus initializePlugin( MObject obj )
{ 
    MStatus   status;
    MFnPlugin plugin( obj, "Justin Tennant", "2017", "Any");

    status = plugin.registerNode( "StringlessMayaDevice", 
        StringlessMayaDevice::id,
        StringlessMayaDevice::creator,
        StringlessMayaDevice::initialize,
        MPxNode::kThreadedDeviceNode );
    if( !status ) {
        status.perror("failed to registerNode StringlessMayaDevice");
    }

    status = plugin.registerCommand( "StringlessMayaCmd", 
                                     StringlessMayaCmd::creator, 
                                     StringlessMayaCmd::newSyntax );
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status;
    MFnPlugin plugin( obj );

    status = plugin.deregisterCommand("StringlessMayaCmd");
    if(!status)
    {
        status.perror("deregisterCommand");
        return status;
    }

    status = plugin.deregisterNode( StringlessMayaDevice::id );
    if( !status ) {
        status.perror("failed to deregisterNode StringlessMayaCmd");
    }
    return status;
}