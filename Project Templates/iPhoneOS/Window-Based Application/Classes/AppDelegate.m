//
//  AppDelegate.m
//  ___PROJECTNAME___
//
//  Created: ___DATE___
//
//  Copyright ___YEAR___, ___ORGANIZATIONNAME___.
//

#import "AppDelegate.h"

//
// Private methods
//
@interface AppDelegate ()

@end

//
// Implementation
//
@implementation AppDelegate
@synthesize ibWindow;

- (void)applicationDidFinishLaunching:(UIApplication *)application {    

    // Override point for customization after application launch
    [self.ibWindow makeKeyAndVisible];
}

- (void)dealloc {
    [ibWindow release];
	ibWindow = nil;
	
    [super dealloc];
}

@end
