// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// mectestpanic1step.h
//

/**
@file
*/


#ifndef MECTESTPANIC1STEP_H
#define MECTESTPANIC1STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic1Step, "MecTestPanic1");


class CMecTestPanic1Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic1Step();

public:
    explicit CMecTestPanic1Step()
        : CMecUnitTestStepBase(KMecTestPanic1Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC1STEP_H


