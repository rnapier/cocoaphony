//
//  Bezier.h
//  BezierPerf
//
//  Created by Rob Napier on 02/22/12.
//  Copyright (c) 2012 Rob Napier. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>

#ifndef _Bezier_h
#define _Bezier_h

unsigned int copyBezierSimple(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
unsigned int copyBezierAccelerate(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
unsigned int copyBezierNoPow(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
unsigned int copyBezierAccelerateNoPow(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
unsigned int copyBezierXY(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
unsigned int copyBezierTable(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results);
#endif
