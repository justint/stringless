/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   StringlessWindow.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on March 6, 2017, 12:42 AM
 */

#ifndef STRINGLESS_H
#define STRINGLESS_H

#include <QtCore/QPointer>
#include <QtGui/QPushButton>
#include <QtGui/QDialog>
#include <QtGui/QLabel>

#include <maya/MSpinLock.h>
#include <maya/MPxCommand.h>
#include <maya/MPxThreadedDeviceNode.h>

#include "../src/FrameData.h"
#include "../src/MemoryManager.h"
#include "../src/Reader.h"

class StringlessMayaDevice : public MPxThreadedDeviceNode {
    public:
        StringlessMayaDevice();
        virtual         ~StringlessMayaDevice() override;
        virtual void    postConstructor() override;
        virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
        virtual void    threadHandler() override;
        virtual void    threadShutdownHandler() override;
        
        void            calibrateNeutralFace();         
        
        static void*    creator();
        static MStatus  initialize();
        
        
        static MObject      inputRecord;
        static MObject      outputTranslate;
        static MObject      outputTranslations;
        static MObject      outputRotations;
        static MSpinLock    spinLock;
        static MTypeId      id;
        
        static MObject      anchorLocally; 
        static MObject      amplifyAmount;
        
        static MObject      neutralFace;
        
        static Stringless::MemoryManager    memoryManager;
        static Stringless::Reader           reader;
        
    private:
        const static int                    DATA_POINTS = 68;
        
        static Stringless::FrameData*       curFrameData;
        static bool                         bRecord;
        static bool                         bLive;
        
        static MDoubleArray                 frameDataToDoubleArray(Stringless::FrameData* fd);
        static Stringless::FrameData        doubleArrayToFrameData(MDoubleArray array);
    
};

#endif /* STRINGLESS_H */

