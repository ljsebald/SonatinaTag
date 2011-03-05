/*
    SonatinaTag
    Copyright (C) 2011 Lawrence Sebald

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License version 2.1 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef SonatinaTag__NSErrorExt_h
#define SonatinaTag__NSErrorExt_h

#import <Foundation/Foundation.h>

#define SonatinaTagErrorDomain  @"SonatinaTag"

enum SonatinaTagErrorCodes {
    STError_OpeningFile = -1,
    STError_IOError     = -2,
    STError_CharConvert = -3
};

@interface NSError (SonatinaTagExtensions)

+ (NSError *)errorWithDomain:(NSString *)domain
                        code:(NSInteger)code
                      reason:(NSString *)reason;

@end /* @interface NSError (SonatinaTagExtensions) */

#endif /* !SonatinaTag__NSErrorExt_h */