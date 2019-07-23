//
//  ViewController.m
//  test
//
//  Created by king on 2019/7/23.
//  Copyright © 2019 K&K. All rights reserved.
//

#import "ViewController.h"

#import "XXOO.hpp"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
	[super viewDidLoad];
	// Do any additional setup after loading the view.

    XX::XXOO *xxoo = new XX::XXOO();
	xxoo->test("xxooo");
	delete xxoo;
	xxoo = NULL;
}

@end
