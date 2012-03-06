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
#import <stdio.h>

const unsigned kSteps = 999;  // FIXME (must be odd currently for asm2)

static inline CGFloat Bezier(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
                     CGFloat P3) {
  return
    powf(1-t, 3) * P0
    + 3 * powf(1-t, 2) * t * P1
    + 3 * (1-t) * powf(t, 2) * P2
    + powf(t, 3) * P3;
}

static inline CGFloat BezierNoPow(CGFloat t, CGFloat P0, CGFloat P1, CGFloat P2,
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
    
    CGPoint point = {
      C0*P0.x + C1*P1.x + C2*P2.x + C3*P3.x,
      C0*P0.y + C1*P1.y + C2*P2.y + C3*P3.y
    };
    
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

unsigned int copyBezierIntrinsic(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
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
  
  float32x2_t vP0 = {P0.x, P0.y};
  float32x2_t vP1 = {P1.x, P1.y};
  float32x2_t vP2 = {P2.x, P2.y};
  float32x2_t vP3 = {P3.x, P3.y};
  for (unsigned step = 0; step <= kSteps; ++step)
  {
    float32x2_t point;    
    point = vmul_n_f32(vP0, C0[step]);
    point = vmla_n_f32(point, vP1, C1[step]);    
    point = vmla_n_f32(point, vP2, C2[step]);    
    point = vmla_n_f32(point, vP3, C3[step]);    
    (*results)[step].x = vget_lane_f32(point, 0);
    (*results)[step].y = vget_lane_f32(point, 1);    
  }
  return kSteps + 1;
}

unsigned int copyBezierVector(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static float32x4_t C[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C[step][0] = (1-t)*(1-t)*(1-t); // * P0
      C[step][1] = 3 * (1-t)*(1-t) * t; // * P1
      C[step][2] = 3 * (1-t) * t*t; // * P2
      C[step][3] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  float32x4_t x = {P0.x, P1.x, P2.x, P3.x};
  float32x4_t y = {P0.y, P1.y, P2.y, P3.y};
  float32x4_t *c = &C[0];
  float32x2_t *result = (float32x2_t*)*results;
  for (int step = kSteps; step >= 0; step--)
  {
    float32x4_t xResult = vmulq_f32(x, *c);    
    float32x4_t yResult = vmulq_f32(y, *c);

    float32x2_t xResult1 = vpadd_f32(vget_high_f32(xResult), vget_low_f32(xResult));
    float32x2_t yResult1 = vpadd_f32(vget_high_f32(yResult), vget_low_f32(yResult));
    
    *result = vpadd_f32(xResult1, yResult1);
    c++;
    result++;
  }
  return kSteps + 1;
}

void bezier_asm(unsigned kSteps, CGPoint *results, float32x4_t *C, CGPoint* points);

unsigned int copyBezierAsm(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static float32x4_t C[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C[step][0] = (1-t)*(1-t)*(1-t); // * P0
      C[step][1] = 3 * (1-t)*(1-t) * t; // * P1
      C[step][2] = 3 * (1-t) * t*t; // * P2
      C[step][3] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  CGPoint points[4] = {P0, P1, P2, P3};
  bezier_asm(kSteps, *results, C, points);
  return kSteps + 1;
}

unsigned int copyBezierInline(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static float32x4_t C[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C[step][0] = (1-t)*(1-t)*(1-t); // * P0
      C[step][1] = 3 * (1-t)*(1-t) * t; // * P1
      C[step][2] = 3 * (1-t) * t*t; // * P2
      C[step][3] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  CGPoint points[4] = {P0, P1, P2, P3};
  
  //bezier_asm(
  //        unsigned kSteps, // r0
  //        CGPoint *results, // r1
  //        float32x4_t *C, // r2
  //        CGPoint* points //r3
  //  );

  CGPoint *resultsPtr = *results;
  int steps = kSteps + 1;
  float32x4_t *Cptr = &C[0];
  
  asm volatile (
                ".align 2\n\t"
                ".code 16\n\t"
      "vld2.32 {q0,q1}, [%[points]]\n\t" // x->q0, y->q1
      
      "vld1.f32 {q3}, [%[C]]!\n\t"      // c->q3 (First load)
      
      "Loop:\n\t"
      "vmul.f32 q8, q3, q0\n\t"       // c*x->q8
      "vmul.f32 q9, q3, q1\n\t"       // c*y->q9
      
      "vld1.f32 {q3}, [%[C]]!\n\t"      // c->q3 (Load next cycle while we're waiting for the multiply)
      
      "vpadd.f32 d16, d16, d17\n\t"     // Half-add X -> d16
      "vpadd.f32 d18, d18, d19\n\t"     // Half-add Y -> d18
      
      "vpadd.f32 d16, d16, d18\n\t"     // Full add X and Y -> d16
      
      "vst1.f32 {d16}, [%[results]]!\n\t"  // Store to results and increment pointer
      
      "subs %[steps], #1\n\t"              // (May want to move this sooner while we're stalled)
      "bne Loop\n\t"
                :  // output
                : [results] "+r" (resultsPtr),
                  [steps] "+r" (steps), // input
                  [C] "r" (Cptr),
                  [points] "r" (points)
                : "q0", "q1", "q3", "q8", "q9", "memory" // clobber
      );

  
  return kSteps + 1;
}

void bezier_asm2(unsigned kSteps, CGPoint *results, float32x4_t *C, CGPoint* points);

unsigned int copyBezierAsm2(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static float32x4_t C[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C[step][0] = (1-t)*(1-t)*(1-t); // * P0
      C[step][1] = 3 * (1-t)*(1-t) * t; // * P1
      C[step][2] = 3 * (1-t) * t*t; // * P2
      C[step][3] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  CGPoint points[4] = {P0, P1, P2, P3};
  bezier_asm2(kSteps, *results, C, points);
  return kSteps + 1;
}

void bezier_asm4(unsigned kSteps, CGPoint *results, float32x4_t *C, CGPoint* points);

unsigned int copyBezierAsm4(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));
  
  static float32x4_t C[kSteps] = {0};
  static int sInitialized = 0;
  if (!sInitialized)
  {
    for (unsigned step = 0; step <= kSteps; ++step)
    {
      CGFloat t = (CGFloat)step/(CGFloat)kSteps;
      C[step][0] = (1-t)*(1-t)*(1-t); // * P0
      C[step][1] = 3 * (1-t)*(1-t) * t; // * P1
      C[step][2] = 3 * (1-t) * t*t; // * P2
      C[step][3] = t*t*t; // * P3;
    }
    sInitialized = 1;
  }
  
  CGPoint points[4] __attribute__ ((align (16))) = {P0, P1, P2, P3};
  bezier_asm4(kSteps, *results, C, points);
  return kSteps + 1;
}

// Based on Hannu Kankaanpää's forward differencing code:
// http://www.niksula.hut.fi/~hkankaan/Homepages/bezierfast.html
unsigned int copyBezierDiff(CGPoint P0, CGPoint P1, CGPoint P2, CGPoint P3, CGPoint **results)
{
  *results = malloc((kSteps + 1) * sizeof(struct CGPoint));

  struct {
    float f;
    float fd;
    float fdd;
    float fddd;
    float fdd_2;
    float fddd_2;
    float fddd_6;
  } x, y;
  
  float t = 1.0 / (float)kSteps;
  float t2 = t * t;

  x.f = P0.x;
  x.fd = 3 * (P1.x - P0.x) * t;
  x.fdd_2 = 3 * (P0.x - 2 * P1.x + P2.x) * t2;
  x.fddd_2 = 3 * (3 * (P1.x - P2.x) + P3.x - P0.x) * t2 * t;
  
  x.fddd = x.fddd_2 + x.fddd_2;
  x.fdd = x.fdd_2 + x.fdd_2;
  x.fddd_6 = x.fddd_2 * (1.0 / 3);

  y.f = P0.y;
  y.fd = 3 * (P1.y - P0.y) * t;
  y.fdd_2 = 3 * (P0.y - 2 * P1.y + P2.y) * t2;
  y.fddd_2 = 3 * (3 * (P1.y - P2.y) + P3.y - P0.y) * t2 * t;
  
  y.fddd = y.fddd_2 + y.fddd_2;
  y.fdd = y.fdd_2 + y.fdd_2;
  y.fddd_6 = y.fddd_2 * (1.0 / 3);

  
  for (unsigned int loop=0; loop < kSteps; loop++) {
    (*results)[loop] = CGPointMake(x.f, y.f);
    
    x.f = x.f + x.fd + x.fdd_2 + x.fddd_6;
    x.fd = x.fd + x.fdd + x.fddd_2;
    x.fdd = x.fdd + x.fddd;
    x.fdd_2 = x.fdd_2 + x.fddd_2;
    
    y.f = y.f + y.fd + y.fdd_2 + y.fddd_6;
    y.fd = y.fd + y.fdd + y.fddd_2;
    y.fdd = y.fdd + y.fddd;
    y.fdd_2 = y.fdd_2 + y.fddd_2;
  }
  
  (*results)[kSteps] = P3;
  
  return kSteps + 1;
}
