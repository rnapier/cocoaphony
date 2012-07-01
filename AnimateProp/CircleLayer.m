//
//  CircleLayer.m
//  AnimateProp
//
//  Created by Rob Napier on 6/25/12.
//  Copyright (c) 2012 Rob Napier. All rights reserved.
//

#import "CircleLayer.h"

@interface CircleLayer ()
@property CGColorRef colorRef;
@end

@implementation CircleLayer
@dynamic radius;
@dynamic color;

+ (BOOL)needsDisplayForKey:(NSString *)key {
  if ([key isEqualToString:@"radius"] || [key isEqualToString:@"color"]) {
    return YES;
  }
  return [super needsDisplayForKey:key];
}

- (CircleLayer *)init
{
  self = [super init];
  if (self) {
    self.color = [UIColor redColor];
    self.contentsScale = [[UIScreen mainScreen] scale];
    [self setNeedsDisplay];
  }
  return self;
}

- (void)drawInContext:(CGContextRef)ctx {
  CGContextSetFillColorWithColor(ctx, [[self color] CGColor]);
  CGFloat radius = self.radius;
  printf("%f\n", radius);
  CGRect rect;
  rect.size = CGSizeMake(radius, radius);
  rect.origin.x = (self.bounds.size.width - radius) / 2;
  rect.origin.y = (self.bounds.size.height - radius) / 2;
  CGContextAddEllipseInRect(ctx, rect);
  CGContextFillPath(ctx);
}

- (void)setColorRef:(CGColorRef)colorRef {
  self.color = [UIColor colorWithCGColor:colorRef];
}

- (CGColorRef)colorRef {
  return self.color.CGColor;
}

- (id < CAAction >)actionForKey:(NSString *)key {
  if ([self presentationLayer] != nil) {
    if ([key isEqualToString:@"radius"]) {
      CABasicAnimation *anim = [CABasicAnimation
                                animationWithKeyPath:@"radius"];
      anim.fromValue = [[self presentationLayer] 
                        valueForKey:@"radius"];
      return anim;
    }
    
    if ([key isEqualToString:@"color"]) {
      CABasicAnimation *anim = [CABasicAnimation
                                animationWithKeyPath:@"colorRef"];
      anim.fromValue = [[self presentationLayer] 
                        valueForKey:@"colorRef"];
      return anim;
    }

  }
  
  return [super actionForKey:key];
}

@end
