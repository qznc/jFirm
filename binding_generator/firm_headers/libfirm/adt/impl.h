/*
 * Copyright (C) 1995-2008 University of Karlsruhe.  All right reserved.
 *
 * This file is part of libFirm.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * Licensees holding valid libFirm Professional Edition licenses may use
 * this file in accordance with the libFirm Commercial License.
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

/**
 * @file
 * @brief  Some macros for wrapper function implementation. (WARNING deprecated)
 * @author Sebastian Hack
 * @date   9.12.2004
 *
 * Please don't use these macros, they make the code unnecessarily unreadable!
 */
#ifndef FIRM_ADT_IMPL_H
#define FIRM_ADT_IMPL_H

#define _MANGLE(prefix,name)		prefix ## name

#define INTERNAL(name)					_MANGLE(_, name)


#define _HEAD1(name,res,t1) \
	res (name)(t1 p1)

#define _HEAD2(name,res,t1,t2) \
	res (name)(t1 p1, t2 p2)

#define _HEAD3(name,res,t1,t2,t3) \
	res (name)(t1 p1, t2 p2, t3 p3)

#define _HEAD4(name,res,t1,t2,t3,t4) \
	res (name)(t1 p1, t2 p2, t3 p3, t4 p4)


#define _IMPL1(name,prefix,ret,res,t1) \
_HEAD1(name, res, t1) { \
	ret prefix ## name (p1); \
}

#define _IMPL2(name,prefix,ret,res,t1,t2) \
_HEAD2(name, res, t1, t2) { \
	ret prefix ## name (p1, p2); \
}

#define _IMPL3(name,prefix,ret,res,t1,t2,t3) \
_HEAD3(name, res, t1, t2, t3) { \
	ret prefix ## name (p1, p2, p3); \
}

#define _IMPL4(name,prefix,ret,res,t1,t2,t3,t4) \
_HEAD4(name, res, t1, t2, t3, t4) { \
	ret prefix ## name (p1, p2, p3, p4); \
}

#define IMPL1_VOID(name,prefix,t1) \
  _IMPL1(name, prefix, (void), void, t1)

#define IMPL2_VOID(name,prefix,t1,t2) \
  _IMPL2(name, prefix, (void), void, t1, t2)

#define IMPL3_VOID(name,prefix,t1,t2,t3) \
  _IMPL3(name, prefix, (void), void, t1, t2, t3)

#define IMPL4_VOID(name,prefix,t1,t2,t3,t4) \
  _IMPL4(name, prefix, (void), void, t1, t2, t3, t4)


#define IMPL1(name,type,prefix,t1) \
  _IMPL1(name, prefix, return, type, t1)

#define IMPL2(name,type,prefix,t1,t2) \
  _IMPL2(name, prefix, return, type, t1, t2)

#define IMPL3(name,type,prefix,t1,t2,t3) \
  _IMPL3(name, prefix, return, type, t1, t2, t3)

#define IMPL4(name,type,prefix,t1,t2,t3,t4) \
  _IMPL4(name, prefix, return, type, t1, t2, t3, t4)


#define FIRM_IMPL1(name,type,t1) \
  _IMPL1(name, _, return, type, t1)

#define FIRM_IMPL2(name,type,t1,t2) \
  _IMPL2(name, _, return, type, t1, t2)

#define FIRM_IMPL3(name,type,t1,t2,t3) \
  _IMPL3(name, _, return, type, t1, t2, t3)

#define FIRM_IMPL4(name,type,t1,t2,t3,t4) \
  _IMPL4(name, _, return, type, t1, t2, t3, t4)


#define FIRM_IMPL1_VOID(name,t1) \
  _IMPL1(name, _, (void), void, t1)

#define FIRM_IMPL2_VOID(name,t1,t2) \
  _IMPL2(name, _, (void), void, t1, t2)

#define FIRM_IMPL3_VOID(name,t1,t2,t3) \
  _IMPL3(name, _, (void), void, t1, t2, t3)

#define FIRM_IMPL4_VOID(name,t1,t2,t3,t4) \
  _IMPL4(name, _, (void), void, t1, t2, t3, t4)

#define FIRM_PROTO1(name,type,t1) 						_HEAD1(name, type, t1)
#define FIRM_PROTO2(name,type,t1,t2) 					_HEAD2(name, type, t1, t2)
#define FIRM_PROTO3(name,type,t1,t2,t3) 			_HEAD3(name, type, t1, t2, t3)
#define FIRM_PROTO4(name,type,t1,t2,t3,t4) 		_HEAD4(name, type, t1, t2, t3, t4)

#endif
