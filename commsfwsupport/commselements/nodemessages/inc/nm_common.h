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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NM_COMMON_H
#define SYMBIAN_NM_COMMON_H

#include <elements/metavirtctor.h>
#include <ecom/implementationproxy.h>
#include <elements/nm_common_internal.h>
#include <elements/nm_log.h>

namespace Messages
{
class MTransportSender;
class MTransportReceiver;
class TNodeId;

static const TInt KNullMessageId = 0;


//Messages panic category
_LIT(KMessagesPanic, "Messages");

//Panics generated by the Messages module
enum TMessagesPanics
	{
	/** RxxxInterface not opened with recipient peer details
	*/
	ETransportNotOpened = 1,
	EAddressCastPanic,
	EMessageCastPanic,
	EConstructedObjectTooBig,
	EAddressNotValidPanic,
	EClientNotValidPanic
	};


class CGlobals : public CGlobalsInternal
	{
	friend class TlsGlobals;

public:
	TBool IsInterfaceRegistered(TUid aInterfaceId) const
		{
		return iVirtCtor->IsInterfaceRegistered(aInterfaceId);
		}

	void RegisterInterfaceL(TUid aInterfaceId, TInt aNumCtors, const TImplementationProxy* aCtorTable)
		{
		NM_LOG((KNodeMessagesSubTag, _L8("RegisterInterfaceL InterfaceId=%08x"),aInterfaceId));
		iVirtCtor->RegisterInterfaceL(aInterfaceId, aNumCtors, aCtorTable);
		}

	void DeregisterInterface(TUid aInterfaceId)
	    {
		NM_LOG((KNodeMessagesSubTag, _L8("DeregisterInterface InterfaceId=%08x"),aInterfaceId));
	    iVirtCtor->DeregisterInterface(aInterfaceId);
	    }


private:
	IMPORT_C explicit CGlobals();
	IMPORT_C ~CGlobals();
	IMPORT_C void ConstructL();
	};

struct TlsGlobals
	{
public:
	IMPORT_C static CGlobals& InstallGlobalsL();
	IMPORT_C static void UnInstallGlobals();
	IMPORT_C static CGlobals& Get();
	};

} //namespace Messages


#endif
//SYMBIAN_NM_COMMON_H

