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

#include "NSErrorExt.h"

@implementation NSError (SonatinaTagExtensions)

+ (NSError *)errorWithDomain:(NSString *)domain
                        code:(NSInteger)code
                      reason:(NSString *)reason
{
    NSDictionary *d;

    d = [NSDictionary dictionaryWithObject:reason
                                    forKey:NSLocalizedDescriptionKey];
    return [NSError errorWithDomain:domain code:code userInfo:d];
}

@end /* @implementation NSError (SonatinaTagExtensions) */
