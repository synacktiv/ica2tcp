/*************************************************************************
*
*  win32\platid.h
*
*  Platform specific types related to client identification
*
*  Copyright 2015 Citrix Systems, Inc. 
*
*************************************************************************/

#ifndef _WIN32_PLATID_H_
#define _WIN32_PLATID_H_

#define CLIENT_PRODUCT_ID       CLIENTID_CITRIX_DOS
#define CLIENT_BASE_MODEL       CLIENT_MODEL_CTX_WIN32

/*
 *  DLL's used to identify the SecureICA model
 */
#define SECUREICA_40_BIT_DLL    "pdc40n.dll"
#define SECUREICA_56_BIT_DLL    "pdc56n.dll"
#define SECUREICA_128_BIT_DLL   "pdc128n.dll"

#endif /* _WIN32_PLATID_H_ */

