//
//  AppDelegate.h
//  ___PROJECTNAME___
//
//  Created: ___DATE___
//
//  Copyright ___YEAR___, ___ORGANIZATIONNAME___.
//

#import <UIKit/UIKit.h>

@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *ibWindow;
}

@property (nonatomic, readwrite, retain) IBOutlet UIWindow *ibWindow;

@end

