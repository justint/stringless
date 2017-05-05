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

#include <stringless/FrameData.h>
#include <stringless/MemoryManager.h>
#include <stringless/Reader.h>

class StringlessMayaDevice : public MPxThreadedDeviceNode {
    public:
        StringlessMayaDevice();
        virtual         ~StringlessMayaDevice() override;
        virtual void    postConstructor() override;
        virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
        virtual void    threadHandler() override;
        virtual void    threadShutdownHandler() override;
        
        static void*    creator();
        static MStatus  initialize();
        
        
        
        static MObject      inputRecord;
        static MObject      outputTranslate;
        static MObject      outputTranslations;
        static MObject      outputRotations;
        static MSpinLock    spinLock;
        static MTypeId      id;
        
        
    private:
        static Stringless::MemoryManager    memoryManager;
        static Stringless::Reader           reader;
        static Stringless::FrameData*       curFrameData;
        static bool                         bRecord;
        static bool                         bLive;
    
};

#endif /* STRINGLESS_H */

