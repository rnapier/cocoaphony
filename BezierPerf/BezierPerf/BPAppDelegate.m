//
//  BPAppDelegate.m
//  BezierPerf
//
//  Created by Rob Napier on 02/22/12.
//  Copyright (c) 2012 Rob Napier. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#import "BPAppDelegate.h"
#import "Bezier.h"

@implementation BPAppDelegate

@synthesize window = _window;

#define BEZIER_FUNC copyBezierTable

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  const unsigned kFPS = 50;
  const unsigned kNumCurves = 100;

  CGPoint P0 = {50, 500};
  CGPoint P1 = {300, 300};
  CGPoint P2 = {400, 700};
  CGPoint P3 = {650, 500};

  CFAbsoluteTime start = CFAbsoluteTimeGetCurrent();
  CGPoint lastPoint = {0};
  for (unsigned i = 0; i < kFPS * kNumCurves; ++i)
  {
    P0.y++;
    CGPoint *points;
    unsigned count = BEZIER_FUNC(P0, P1, P2, P3, &points);
    lastPoint = points[count-1];
    free(points);
  }
  printf("Time: %f\n", CFAbsoluteTimeGetCurrent() - start);
  printf("{%f, %f}\n", lastPoint.x, lastPoint.y );
  exit(0);

  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  self.window.backgroundColor = [UIColor whiteColor];
  [self.window makeKeyAndVisible];

  return YES;
}


@end