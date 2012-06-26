//
//  ViewController.m
//  AnimateProp
//
//  Created by Rob Napier on 6/25/12.
//  Copyright (c) 2012 Rob Napier. All rights reserved.
//

#import "ViewController.h"
#import "CircleLayer.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
  CircleLayer *circleLayer = [CircleLayer new];
  circleLayer.radius = 50;
  circleLayer.frame = self.view.bounds;
  [self.view.layer addSublayer:circleLayer];

  UIGestureRecognizer *g = [[UITapGestureRecognizer alloc]
                            initWithTarget:self
                            action:@selector(tap:)];
  [self.view addGestureRecognizer:g];
}

- (void)tap:(UIGestureRecognizer *)recognizer {
  [CATransaction setAnimationDuration:2];
  CircleLayer *circleLayer =
  (CircleLayer*)[self.view.layer.sublayers objectAtIndex:0];
  [circleLayer setRadius:100.0];
  [circleLayer setColor:[UIColor greenColor]];
  
  double delayInSeconds = 1.0;
  dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, delayInSeconds * NSEC_PER_SEC);
  dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
    [CATransaction setAnimationDuration:1];
    [circleLayer setPosition:CGPointZero];
    [circleLayer setRadius:25.0];
  });
}

@end
