//
//  STFile.h
//  SonatinaTag
//
//  Created by Lawrence Sebald on 6/5/10.
//  Copyright 2010 Lawrence Sebald. All rights reserved.
//

#ifndef STFile_h
#define STFile_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>

#define STFileID3v1Type     @"ID3v1"
#define STFileID3v2Type     @"ID3v2"

@interface STFile : NSObject {
@private
    NSString *_filename;
    NSMutableDictionary *_tags;
}

- (id)initWithFile:(NSString *)filename;
- (void)dealloc;

- (id<STTag>)getTagForType:(NSString *)type;
- (id<STTag>)tag;

@end /* @interface STFile */

#endif /* !STFile_h */
