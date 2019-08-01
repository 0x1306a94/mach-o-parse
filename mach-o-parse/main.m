//
//  main.m
//  mac-o-parse
//
//  Created by king on 2019/7/22.
//  Copyright © 2019 K&K. All rights reserved.
//

#include "mach-o.h"
#import <Foundation/Foundation.h>

int main(int argc, const char *argv[]) {
	@autoreleasepool {
		// insert code here...
		// 解析 dysm 文件
		const char *path[] = {
		    "/Users/king/Desktop/test-iphonesimulator",
		    "/Users/king/Desktop/test-iphoneos",
		    //            "/Users/king/Desktop/ShowStart",
		};
		for (int i = 0; i < sizeof(path) / sizeof(path[0]); i++) {
			const char *p = path[i];
			printf("parse dysm begin: %s\n\n", p);
			parse_macho(p);
			printf("\nparse dysm end: %s\n", p);
			printf("\n");
		}
	}
	return 0;
}
