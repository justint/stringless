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
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNObjectData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MMatrix.h>
#include <maya/MSimple.h>

#include "../../ext/api_macros.h"
#include "StringlessMayaCmd.h"
#include "FrameData.h"

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

MObject      StringlessMayaDevice::anchorLocally;
MObject      StringlessMayaDevice::amplifyAmount;

MObject      StringlessMayaDevice::neutralFace;


MDoubleArray StringlessMayaDevice::frameDataToDoubleArray(Stringless::FrameData* fd) {
    double melArray [StringlessMayaDevice::DATA_POINTS * 2]; // 68 * 2
    if (!fd) return MDoubleArray(melArray, StringlessMayaDevice::DATA_POINTS * 2);
    int i = 0;
    for (Stringless::PointPair p : fd->points) {
        melArray[i] = p.x;
        i++;
        melArray[i + 1] = p.y;
        i++;
    }
    return MDoubleArray(melArray, StringlessMayaDevice::DATA_POINTS * 2);
}

Stringless::FrameData StringlessMayaDevice::doubleArrayToFrameData(MDoubleArray array) {
    Stringless::FrameData fd;
    fd.frame_number = fd.fps = 0;
    
    if (array.length() <= StringlessMayaDevice::DATA_POINTS * 2) return fd;
    
    for (int i = 0; i < StringlessMayaDevice::DATA_POINTS - 1; i+=2) {
        fd.points[i].x = array[i];
        fd.points[i].y = array[i+1];
    }
    return fd;
}

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
            for (int i = 0; i < StringlessMayaDevice::DATA_POINTS; i++) {
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
    //MFnTypedAttribute   tAttr;
        
    inputRecord = numAttr.create("record", "ird", MFnNumericData::kBoolean, false, &status);
    MCHECKERROR(status, "create input Record");
    numAttr.setWritable(true);
    numAttr.setConnectable(false);
    ADD_ATTRIBUTE(inputRecord);
    
    anchorLocally = numAttr.create("anchorLocally", "al", MFnNumericData::kBoolean, true, &status);
    MCHECKERROR(status, "create anchorLocally");
    numAttr.setWritable(true);
    numAttr.setConnectable(false);
    numAttr.setStorable(true);
    ADD_ATTRIBUTE(anchorLocally);
    
    amplifyAmount = numAttr.create("amplifyAmount", "aa", MFnNumericData::kFloat, 1.0, &status);
    MCHECKERROR(status, "create amplifyAmount");
    numAttr.setWritable(true);
    numAttr.setConnectable(false);
    numAttr.setStorable(true);
    ADD_ATTRIBUTE(amplifyAmount);
    

    neutralFace = numAttr.create("neutralFace", "nf", MFnNumericData::k3Double, 0.0, &status);
    MCHECKERROR(status, "create neutralFace");
    numAttr.setWritable(true);
    numAttr.setHidden(true);
    numAttr.setArray(true);
    numAttr.setUsesArrayDataBuilder(true);
    ADD_ATTRIBUTE(neutralFace);
    
    outputTranslate = numAttr.create("outputTranslate", "ot", MFnNumericData::k3Double, 0.0, &status);
    MCHECKERROR(status, "create outputTranslate");
    numAttr.setHidden(true);
    ADD_ATTRIBUTE(outputTranslate);
    
    outputTranslations = numAttr.create("outputTranslations", "ots", MFnNumericData::k3Double, 0.0, &status);
    MCHECKERROR(status, "create outputTranslations");
    numAttr.setArray(true);
    numAttr.setUsesArrayDataBuilder(true);
    ADD_ATTRIBUTE(outputTranslations);
    
    outputRotations = numAttr.create("outputRotations", "ors", MFnNumericData::k3Double, 0.0, &status);
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
    CHECK_MSTATUS(status);
    
    /*
    MPlug test = MFnDependencyNode(thisMObject()).findPlug("currentFace")[0];
    std::string testS = "Test double: " + std::to_string(test.asDouble());
    MGlobal::displayInfo(testS.c_str());
    */
    
    bool anchorLocallyBool = MFnDependencyNode(thisMObject()).findPlug("anchorLocally").asBool();
    float amplifyLevelFloat = MFnDependencyNode(thisMObject()).findPlug("amplifyAmount").asFloat();
    
    
    /*
    MDataHandle neutralDataHandle = MFnDependencyNode(thisMObject()).findPlug("neutralFace").asMDataHandle();
    std::string test = (std::string("neutralDataHandle.data().apiType(): ") + neutralDataHandle.data().apiTypeStr()).c_str();
    MGlobal::displayInfo(test.c_str());
    MFnDoubleArrayData neutralArray(neutralDataHandle.data(), &status);
    Stringless::FrameData neutralFaceData = doubleArrayToFrameData(neutralArray.array());
    
    MDataHandle currentDataHandle = MFnDependencyNode(thisMObject()).findPlug("currentFace").asMDataHandle();
    MFnDoubleArrayData currentArray(currentDataHandle.data(), &status);
    currentArray.set(frameDataToDoubleArray(curFrameData));
    */
    
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
            
            MArrayDataHandle neutralHandle = block.outputArrayValue( neutralFace, &status );
            MCHECKERROR(status, "Error in block.outputArrayValue for neutralFace");

            MArrayDataBuilder neutralBuilder = neutralHandle.builder( &status );
            MCHECKERROR(status, "Error in neutralBuilder = neutralHandle.builder.\n");
            
            MArrayDataHandle translationsHandle = block.outputArrayValue( outputTranslations, &status );
            MCHECKERROR(status, "Error in block.outputArrayValue for outputTranslations");
 
            MArrayDataBuilder translationsBuilder = translationsHandle.builder( &status );
            MCHECKERROR(status, "Error in translationsBuilder = translationsHandle.builder.\n");
 
            MArrayDataHandle rotationsHandle = block.outputArrayValue( outputRotations, &status );
            MCHECKERROR(status, "Error in block.outputArrayValue for outputRotations");
 
            MArrayDataBuilder rotationsBuilder = rotationsHandle.builder( &status );
            MCHECKERROR(status, "Error in rotationsBuilder = rotationsHandle.builder.\n");
 
            for(unsigned int i = 0; i< StringlessMayaDevice::DATA_POINTS; ++i )
            {
                double3& neutral = neutralBuilder.addElement(i, &status).asDouble3();
                MCHECKERROR(status, "ERROR in translate = neutralBuilder.addElement.\n");

                double3& translate = translationsBuilder.addElement(i, &status).asDouble3();
                MCHECKERROR(status, "ERROR in translate = translationsBuilder.addElement.\n");
                if (anchorLocallyBool) {
                    /*
                    double avgX = (curData->points[0].x + curData->points[1].x
                        + curData->points[2].x + curData->points[14].x 
                        + curData->points[15].x + curData->points[16].x
                        + curData->points[27].x + curData->points[28].x
                        + curData->points[29].x + curData->points[30].x) / 10;
                    double avgY = (curData->points[0].y + curData->points[1].y
                        + curData->points[2].y + curData->points[14].y 
                        + curData->points[15].y + curData->points[16].y
                        + curData->points[27].y + curData->points[28].y
                        + curData->points[29].y + curData->points[30].y) / 10;  
                    */
                    translate[0] = amplifyLevelFloat*(neutral[0] - curData->points[i].x);
                    translate[1] = amplifyLevelFloat*(neutral[1] - curData->points[i].y);
                } else {
                    translate[0] = amplifyLevelFloat*curData->points[i].x;
                    translate[1] = amplifyLevelFloat*(curData->points[i].y);
                }
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