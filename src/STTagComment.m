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

#include "STTagComment.h"

@implementation STTagComment

- (id)initWithString:(NSString *)c
{
    if((self = [super init])) {
        _comment = [c retain];
    }
    
    return self;
}

+ (id)commentWithString:(NSString *)c
{
    STTagComment *rv = [[STTagComment alloc] initWithString:c];
    return [rv autorelease];
}

- (NSString *)comment
{
    return [NSString stringWithString:_comment];
}

- (NSString *)description
{
    return @"<No Description>";
}

- (NSString *)language
{
    return @"None";
}

@end /* @implementation STTagComment */
