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
//

/**
 @file
*/

#include "nm_common.h"
#include "nm_interfaces.h"
#include <elements/nm_transport.h>
#include <elements/nm_address_internal.h>
#include "nm_signals.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessIntC, "ElemNodeMessIntC");
#endif

using namespace Messages;
using namespace Meta;

//
//TClientType
namespace Mem4NullType
{
static const TUint8 mem[sizeof(TClientType)] = {0};
}

EXPORT_C const TClientType& TClientType::NullType() {return *reinterpret_cast<const TClientType*>(Mem4NullType::mem);}

/******************************************************************************************************
*
*
* RClientInterface
*
*
*******************************************************************************************************/
EXPORT_C RClientInterface::RClientInterface()
:	iTransportSender(NULL)
	{
	Mem::FillZ(iRecipientAddress, sizeof(iRecipientAddress));
	}

EXPORT_C RClientInterface::~RClientInterface()
	{
	}

EXPORT_C void RClientInterface::Open(const TRuntimeCtxId& aPostTo, MTransportSender* aSender)
	{
	iTransportSender = aSender? aSender : &TlsGlobals::Get().TransportSender();
	RecipientRef() = aPostTo; //This a deep copy
	}

EXPORT_C void RClientInterface::Close()
	{
	iTransportSender = NULL;
	Mem::FillZ(iRecipientAddress, sizeof(iRecipientAddress));
	}

EXPORT_C TBool RClientInterface::IsOpen() const
	{
	return iTransportSender != NULL;
	}


EXPORT_C /*static*/ void RClientInterface::OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage)
	{
	RClientInterface client;
	client.Open(aPostTo);
	client.PostMessage(aPostFrom, aMessage);
	client.Close();
	}

EXPORT_C void RClientInterface::PostMessage(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const
	{
	PostMessage(aPostFrom, RecipientId(), aMessage);
	}

EXPORT_C TBool RClientInterface::operator==(const TRuntimeCtxId& aRHS) const
	{
	return RecipientRef() == aRHS;
	}

EXPORT_C TBool RClientInterface::operator==(const RClientInterface& aRHS) const
	{
	return RecipientRef() == aRHS.RecipientRef();
	}

void RClientInterface::PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage) const
	{
	__ASSERT_DEBUG(iTransportSender, User::Panic(KMessagesPanic, ETransportNotOpened));	// Interface not properly initialised; needs transport for this sending thread
	__ASSERT_DEBUG(aPostTo.Size() > 0, User::Panic(KMessagesPanic, EAddressNotValidPanic));

	iTransportSender->PostMessage(aPostFrom, aPostTo, aMessage);
	}

/******************************************************************************************************
*
*
* RNodeInterface
*
*
*******************************************************************************************************/
EXPORT_C void RNodeInterface::Open(TNodeId aPostTo, const TClientType& aClientType, MTransportSender* aSender)
/*
Opens 'this'.
aPostTo by value on purpose as clients may be calling ::Open with subclasses of TNodeId, we must trunkate.
*/
	{
	RClientInterface::Open(aPostTo, aSender);
	iClientType = aClientType;
	}

EXPORT_C void RNodeInterface::Close()
	{
	RClientInterface::Close();
	iClientType = TClientType::NullType();
	}


EXPORT_C void RNodeInterface::PostMessage(const TRuntimeCtxId& aPostFrom, const TNodeId::TRemainder& aPostTo, const TSignalBase& aMessage) const
	{
	TUint8 postTo[__Align8(TRuntimeCtxId::KMaxInlineAddressSize)];
	Mem::Copy(postTo, &RecipientId(), sizeof(TNodeId));
	Mem::Copy(postTo + sizeof(TNodeId), aPostTo.iRemainder, aPostTo.Size());
	postTo[0] = aPostTo.Size() + sizeof(TNodeId);
	postTo[0] |= TRuntimeCtxId::KIsNullMask8;

	RClientInterface::PostMessage(aPostFrom, *reinterpret_cast<TRuntimeCtxId*>(postTo), aMessage);
	}


EXPORT_C TBool RNodeInterface::operator==(const RNodeInterface& aRHS) const
	{
	return RecipientId() == aRHS.RecipientId();
	}

/******************************************************************************************************
*
*
* TClientIterBase
*
*
*******************************************************************************************************/
EXPORT_C RNodeInterface* TClientIterBase::operator[](TInt aInd)
    {
    // assuming at present that array indices start from beginning of array
    // rather than current location of iterator, and array access doesn't
    // change location of iterator.
	TInt prevIndex = iIndex;
	TInt start = 0;
    RNodeInterface* cli = Find(start, +1, aInd + 1);
	iIndex = prevIndex;
	return cli;
    }

EXPORT_C RNodeInterface* TClientIterBase::Find(TInt& aInd, TInt aDir, TInt aCount)
    {
    while (aInd >= 0 && aInd < iClients.Count())
        {
        RNodeInterface* ctl = iClients[aInd];
        if (TypeMatch(*ctl) && --aCount <= 0)
            {
            iIndex = aInd;
            return ctl;
            }
        aInd += aDir;
        }
    return NULL;
    }


/******************************************************************************************************
*
*
* RRequestOriginator
*
*
*******************************************************************************************************/
EXPORT_C TInt RRequestOriginator::Open(Messages::RNodeInterface& aNode, const Messages::TRuntimeCtxId& aRequestOriginator)
	{
	if (aNode == aRequestOriginator)
		{
		iNode = &aNode;
		iRemainder = aRequestOriginator;
		return KErrNone;
		}
	return KErrArgument;
	}

EXPORT_C void RRequestOriginator::Open(RRequestOriginator& aOriginalRequest)
	{
	__ASSERT_DEBUG(aOriginalRequest.IsOpen(), User::Panic(KSpecAssert_ElemNodeMessIntC, 1));
	iNode = aOriginalRequest.iNode;
	iRemainder = aOriginalRequest.iRemainder;
	aOriginalRequest.Close();
	}

EXPORT_C TBool RRequestOriginator::IsOpen() const
	{
	return iNode != NULL;
	}

EXPORT_C TBool RRequestOriginator::operator==(const RRequestOriginator& aRHS) const
	{
	return (Node().RecipientId() == aRHS.Node().RecipientId()) && (iRemainder == aRHS.iRemainder);
	}

EXPORT_C TBool RRequestOriginator::operator==(const TRuntimeCtxId& aRHS) const
	{
	return (iNode->RecipientId() == aRHS) && (iRemainder == aRHS);
	}

EXPORT_C void RRequestOriginator::Close()
	{
	iNode = NULL;
	}

EXPORT_C void RRequestOriginator::PostMessage(const Messages::TRuntimeCtxId& aPostFrom, const Messages::TSignalBase& aMessage) const
	{
	__ASSERT_DEBUG(IsOpen(), User::Panic(KMessagesPanic, EClientNotValidPanic));
	iNode->PostMessage(aPostFrom, iRemainder, aMessage);
	}

EXPORT_C void RRequestOriginator::ReplyTo(const Messages::TRuntimeCtxId& aReplyFrom, const Messages::TSignalBase& aMessage)
	{
	PostMessage(aReplyFrom, aMessage);
	Close();
	}
