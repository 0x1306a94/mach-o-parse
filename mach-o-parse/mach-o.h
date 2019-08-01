//
//  mach-o.h
//  mach-o-parse
//
//  Created by king on 2019/7/23.
//  Copyright © 2019 K&K. All rights reserved.
//

#ifndef mach_o_h
#define mach_o_h

#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/swap.h>
#include <stdio.h>

struct _cpu_type_names {
	cpu_type_t cputype;
	const char *cpu_name;
};

struct _cpu_sub_type_names {
	cpu_type_t cpusubtype;
	const char *cpu_name;
};

struct Context {
	long header_offset;
	long load_command_offset;
	uint32_t ncmds;
	int is_64;
	int is_swap;
};

extern const struct _cpu_type_names cpu_type_names[];
extern const struct _cpu_sub_type_names cpu_sub_type_names[];

extern const char *cpu_type_name(cpu_type_t cpu_type);
extern const char *cpu_sub_type_name(cpu_subtype_t cpu_sub_type);

void parse_macho(const char *path);

void process_mach_header(void *macho, struct Context *ctx);

void process_mach_header_64(void *macho, struct Context *ctx);

void process_fat(void *macho, uint32_t location);

void process_fat_header(void *macho, struct fat_header *mh);

void process_fat_arch(void *macho, struct Context *ctx);

void process_fat_arch_64(void *macho, struct Context *ctx);

int parse_load_command(void *macho, struct load_command *lc, struct Context *ctx);

int process_lc_uuid(void *macho, long *offset, int is_swap);

int process_lc_segment(void *macho, long *offset, int is_swap);

int process_lc_segment_64(void *macho, long *offset, int is_swap);

int process_lc_symtab(void *macho, struct Context *ctx);

void parse_lc_symtab(void *macho, struct symtab_command *command, struct Context *ctx);
void parse_lc_symtab_64(void *macho, struct symtab_command *command, struct Context *ctx);

char *cplus_demangle(const char *mangledSymbol);
#endif /* mach_o_h */
