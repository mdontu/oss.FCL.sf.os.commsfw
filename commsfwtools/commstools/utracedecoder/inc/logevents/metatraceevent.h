// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ClientServerEvent.h
//

#ifndef METATRACEEVENT_H
#define METATRACEEVENT_H

#include "traceeventbase.h"

class CMetaTraceEvent : public CTraceEventBase
    {
    friend class CTraceEventFactory;

    protected:
        explicit CMetaTraceEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler);
        explicit CMetaTraceEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler);

    public:
        //virtual void ProcessEvent() const;
        virtual void Describe(std::ostream& aOutput) const;
	};

#endif // METATRACEEVENT_H
