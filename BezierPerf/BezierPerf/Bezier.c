//
//  Bezier.c
//  BezierPerf
//
//  Created by Rob Napier on 02/22/12.
//  Copyright (c) 2012 Rob Napier. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#import <math.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>

const unsigned kSteps = 1000;

static CGFloat Bezier(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
                     CGFloat P3) {
  return
    powf(1-t, 3) * P0
    + 3 * powf(1-t, 2) * t * P1
    + 3 * (1-t) * powf(t, 2) * P2
    + powf(t, 3) * P3;
}

static CGFloat BezierNoPow(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
                      CGFloat P3) {
  return
  (1-t)*(1-t)*(1-t) * P0
  + 3 * (1-t)*(1-t) * t * P1
  + 3 * (1-t) * t*t * P2
  + t*t*t * P3;
}

static CGFloat BezierAccelerate(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
                                CGFloat P3) {
  const CGFloat P[1][4] = {P0, P1, P2, P3};

  static const CGFloat B[4][4] = 
  { {-1,  3, -3, 1},
    { 3, -6,  3, 0},
    {-3,  3,  0, 0},
    { 1,  0,  0, 0}};
  
  const CGFloat T[4][1] = { powf(t, 3), powf(t, 2), t, 1 };
  
  CGFloat PB[1][4]; // Result of P*B (1 row, 4 columns)
  vDSP_mmul((CGFloat*)P, 1, (CGFloat*)B, 1, (CGFloat*)PB, 1, 1, 4, 4);
  
  CGFloat result[1][1]; // Result of PB*T (1 row, 1 column; a scalar)
  vDSP_mmul((CGFloat*)PB, 1, (CGFloat *)T, 1, (CGFloat*)result, 1, 1, 1, 4);
  
  return result[0][0];
}

static CGFloat BezierAccelerateNoPow(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
                                CGFloat P3) {
  
  const CGFloat P[1][4] = {P0, P1, P2, P3};
  
  static const CGFloat B[4][4] = 
  { {-1,  3, -3, 1},
    { 3, -6,  3, 0},
    {-3,  3,  0, 0},
    { 1,  0,  0, 0}};
  
  const CGFloat T[4][1] = { t*t*t, t*t, t, 1 };
  
  CGFloat PB[1][4]; // Result of P*B (1 row, 4 columns)
  vDSP_mmul((CGFloat*)P, 1, (CGFloat*)B, 1, (CGFloat*)PB, 1, 1, 4, 4);
  
  CGFloat result[1][1]; // Result of PB*T (1 row, 1 column; a scalar)
  vDSP_mmul((CGFloat*)PB, 1, (CGFloat *)T, 1, (CGFloat*)result, 1, 1, 1, 4);
  
  return result[0][0];
}


unsigned int copyBezierSimple(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = calloc(kSteps + 1, sizeof(struct CGPoint));

  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGFloat x = Bezier((CGFloat)step/(CGFloat)kSteps, P0.x, P1.x, P2.x, P3.x);
    CGFloat y = Bezier((CGFloat)step/(CGFloat)kSteps, P0.y, P1.y, P2.y, P3.y);
    (*results)[step] = CGPointMake(x, y);
  }
  return kSteps + 1;
}

unsigned int copyBezierAccelerate(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = calloc(kSteps + 1, sizeof(struct CGPoint));
  
  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGFloat x = BezierAccelerate((CGFloat)step/(CGFloat)kSteps, P0.x, P1.x, P2.x, P3.x);
    CGFloat y = BezierAccelerate((CGFloat)step/(CGFloat)kSteps, P0.y, P1.y, P2.y, P3.y);
    (*results)[step] = CGPointMake(x, y);
  }
  return kSteps + 1;
}

unsigned int copyBezierNoPow(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = calloc(kSteps + 1, sizeof(struct CGPoint));
  
  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGFloat x = BezierNoPow((CGFloat)step/(CGFloat)kSteps, P0.x, P1.x, P2.x, P3.x);
    CGFloat y = BezierNoPow((CGFloat)step/(CGFloat)kSteps, P0.y, P1.y, P2.y, P3.y);
    (*results)[step] = CGPointMake(x, y);
  }
  return kSteps + 1;
}

unsigned int copyBezierAccelerateNoPow(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = calloc(kSteps + 1, sizeof(struct CGPoint));
  
  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGFloat x = BezierAccelerateNoPow((CGFloat)step/(CGFloat)kSteps, P0.x, P1.x, P2.x, P3.x);
    CGFloat y = BezierAccelerateNoPow((CGFloat)step/(CGFloat)kSteps, P0.y, P1.y, P2.y, P3.y);
    (*results)[step] = CGPointMake(x, y);
  }
  return kSteps + 1;
}

unsigned int copyBezierXY(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));

  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGFloat t = (CGFloat)step/(CGFloat)kSteps;
    
    CGFloat C0 = (1-t)*(1-t)*(1-t); // * P0
    CGFloat C1 = 3 * (1-t)*(1-t) * t; // * P1
    CGFloat C2 = 3 * (1-t) * t*t; // * P2
    CGFloat C3 = t*t*t; // * P3;

    CGPoint point = {C0*P0.x + C1*P1.x + C2*P2.x + C3*P3.x,
      C0*P0.y + C1*P1.y + C2*P2.y + C3*P3.y};

    (*results)[step] = point;
  }
  return kSteps + 1;
}

unsigned int copyBezierTable(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static CGFloat C0[kSteps] = {0};
  static CGFloat C1[kSteps] = {0};
  static CGFloat C2[kSteps] = {0};
  static CGFloat C3[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C0[step] = (1-t)*(1-t)*(1-t); // * P0
      C1[step] = 3 * (1-t)*(1-t) * t; // * P1
      C2[step] = 3 * (1-t) * t*t; // * P2
      C3[step] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  for (unsigned step = 0; step <= kSteps; ++step)
  {
    CGPoint point = {
      C0[step]*P0.x + C1[step]*P1.x + C2[step]*P2.x + C3[step]*P3.x,
      C0[step]*P0.y + C1[step]*P1.y + C2[step]*P2.y + C3[step]*P3.y
    };
    (*results)[step] = point;
  }
  return kSteps + 1;
}