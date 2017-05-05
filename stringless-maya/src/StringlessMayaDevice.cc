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

#include "StringlessMayaDevice.h"

#include <maya/MArrayDataBuilder.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MSimple.h>

#include "../../ext/api_macros.h"

namespace Stringless {
MemoryManager::MemoryManager() {};
}

Stringless::FrameData*       StringlessMayaDevice::curFrameData;
Stringless::MemoryManager    StringlessMayaDevice::memoryManager;
Stringless::Reader           StringlessMayaDevice::reader;
MSpinLock                    StringlessMayaDevice::spinLock;

bool    StringlessMayaDevice::bLive = false;
bool    StringlessMayaDevice::bRecord = false;

MObject      StringlessMayaDevice::inputRecord;
MObject      StringlessMayaDevice::outputTranslate;
MObject      StringlessMayaDevice::outputTranslations;
MObject      StringlessMayaDevice::outputRotations;
MTypeId      StringlessMayaDevice::id;

StringlessMayaDevice::StringlessMayaDevice() { }

StringlessMayaDevice::~StringlessMayaDevice() {
    destroyMemoryPools();
}

void StringlessMayaDevice::postConstructor() {
    MObjectArray attrArray;
    attrArray.append( StringlessMayaDevice::outputTranslate );
    setRefreshOutputAttributes( attrArray );
    
    createMemoryPools(24, 1, sizeof(Stringless::FrameData));
}

void StringlessMayaDevice::threadHandler() {
    MStatus status;
    setDone( false );
    
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 2;

    memoryManager = Stringless::MemoryManager(shared_memory_name,
                                              shared_memory_size,
                                              Stringless::MemoryManager::read);

    if (memoryManager.init()) {
        cout << "Memory manager failure" << endl;
        return;
    }

    reader = Stringless::Reader((Stringless::FrameData*)memoryManager.address());
        
    while ( !isDone() ) {
        // Skip processing if we are not live
        if ( !isLive() )
            continue;
        
        MCharBuffer buffer;
        status = acquireDataStorage(buffer);
        if (!status)
            continue;
        
        beginThreadLoop();
        {
            curFrameData = reader.read();
            
            Stringless::FrameData* frameData = 
                    reinterpret_cast<Stringless::FrameData*>(buffer.ptr());
            
            spinLock.lock();
            frameData->frame_number = curFrameData->frame_number;
            for (int i = 0; i < 68; i++) {
                frameData->points[i].x = curFrameData->points[i].x;
                frameData->points[i].y = curFrameData->points[i].y;
            }
            spinLock.unlock();
            pushThreadData(buffer);
        }
        endThreadLoop();
    }
    setDone( true );
}

void StringlessMayaDevice::threadShutdownHandler() {
    setDone( true );
    
}

void* StringlessMayaDevice::creator() {
    return new StringlessMayaDevice;
}

MStatus StringlessMayaDevice::initialize() {
    MStatus             status;
    MFnNumericAttribute numAttr;
    MFnTypedAttribute   tAttr;
    
    inputRecord = numAttr.create("record", "ird", MFnNumericData::kBoolean, false, &status);
    MCHECKERROR(status, "create input Record");
    numAttr.setWritable(true);
    numAttr.setConnectable(false);
    ADD_ATTRIBUTE(inputRecord);
    
    outputTranslate = numAttr.create("outputTranslate", "ot", MFnNumericData::k3Float, 0.0, &status);
    MCHECKERROR(status, "create outputTranslate");
    numAttr.setHidden(true);
    ADD_ATTRIBUTE(outputTranslate);
    
    outputTranslations = numAttr.create("outputTranslations", "ots", MFnNumericData::k3Float, 0.0, &status);
    MCHECKERROR(status, "create outputTranslations");
    numAttr.setArray(true);
    numAttr.setUsesArrayDataBuilder(true);
    ADD_ATTRIBUTE(outputTranslations);
    
    outputRotations = numAttr.create("outputRotations", "ors", MFnNumericData::k3Float, 0.0, &status);
    MCHECKERROR(status, "create outputRotations");
    numAttr.setArray(true);
    numAttr.setUsesArrayDataBuilder(true);
    ADD_ATTRIBUTE(outputRotations);
    
    ATTRIBUTE_AFFECTS( outputTranslate, outputTranslations);
    ATTRIBUTE_AFFECTS( outputTranslate, outputRotations);
    
    ATTRIBUTE_AFFECTS( live, outputTranslate);
    ATTRIBUTE_AFFECTS( live, outputTranslations);
    ATTRIBUTE_AFFECTS( live, outputRotations);
    ATTRIBUTE_AFFECTS( frameRate, outputTranslate);
    ATTRIBUTE_AFFECTS( frameRate, outputTranslations);
    ATTRIBUTE_AFFECTS( frameRate, outputRotations);

    return MS::kSuccess;
}

MStatus StringlessMayaDevice::compute(const MPlug& plug, MDataBlock& block) {
    MStatus status;    
    
    if( plug == outputTranslate || plug.parent() == outputTranslate ||
        plug == outputTranslations ||  plug.parent() == outputTranslations ||
        plug == outputRotations || plug.parent() == outputRotations )
    {
        bLive = isLive();
        
        MCharBuffer buffer;
        if ( popThreadData(buffer) )
        {
            Stringless::FrameData* curData = 
                    reinterpret_cast<Stringless::FrameData*>(buffer.ptr());
            printf( "current frame is %d \n ",  curData->frame_number);
            MArrayDataHandle translationsHandle = block.outputArrayValue( outputTranslations, &status );
            MCHECKERROR(status, "Error in block.outputArrayValue for outputTranslations");
 
            MArrayDataBuilder translationsBuilder = translationsHandle.builder( &status );
            MCHECKERROR(status, "Error in translationsBuilder = translationsHandle.builder.\n");
 
            MArrayDataHandle rotationsHandle = block.outputArrayValue( outputRotations, &status );
            MCHECKERROR(status, "Error in block.outputArrayValue for outputRotations");
 
            MArrayDataBuilder rotationsBuilder = rotationsHandle.builder( &status );
            MCHECKERROR(status, "Error in rotationsBuilder = rotationsHandle.builder.\n");
 
            for(unsigned int i = 0; i< 68; ++i )
            {
                float3& translate = translationsBuilder.addElement(i, &status).asFloat3();
                MCHECKERROR(status, "ERROR in translate = translationsBuilder.addElement.\n");
                translate[0] = curData->points[i].x;
                translate[1] = -curData->points[i].y;
                //translate[2] = curData->data[i][2];
 
                /*
                float3& rotate = rotationsBuilder.addElement(i, &status).asFloat3();
                MCHECKERROR(status, "ERROR in translate = translationsBuilder.addElement.\n");
                rotate[0] = curData->points[i].x;
                rotate[1] = curData->points[i].y;
                ///rotate[2] = curData->data[i][5];
                 */
            }
            status = translationsHandle.set(translationsBuilder);
            MCHECKERROR(status, "set translationsBuilder failed\n");
 
            status = rotationsHandle.set(rotationsBuilder);
            MCHECKERROR(status, "set rotationsBuilder failed\n");
 
            block.setClean( plug );
 
            releaseDataStorage(buffer);
            return ( MS::kSuccess );
        }
        else
        {
            return MS::kFailure;
        }
    }
    return ( MS::kUnknownParameter );
}