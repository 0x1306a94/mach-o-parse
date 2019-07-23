//
//  main.m
//  mac-o-parse
//
//  Created by king on 2019/7/22.
//  Copyright © 2019 K&K. All rights reserved.
//

#include "mach-o.h"
#import <Foundation/Foundation.h>

//#define MATCH_STRUCT(obj, location) \
//    struct obj const *obj = (struct obj *)load_bytes(obj_file, location, sizeof(struct obj));
//
//#define NSSTRING(C_STR) [NSString stringWithCString:(char *)(C_STR) encoding:[NSString defaultCStringEncoding]]
//#define CSTRING(NS_STR) [(NS_STR) cStringUsingEncoding:[NSString defaultCStringEncoding]]

//struct _cpu_type_names {
//    cpu_type_t cputype;
//    const char *cpu_name;
//};
//
//static struct _cpu_type_names cpu_type_names[] = {
//    {CPU_TYPE_I386, "i386"},
//    {CPU_TYPE_X86_64, "x86_64"},
//    {CPU_TYPE_ARM, "arm"},
//    {CPU_TYPE_ARM64, "arm64"},
//};
//
//static const char *cpu_type_name(cpu_type_t cpu_type) {
//    static int cpu_type_names_size = sizeof(cpu_type_names) / sizeof(struct _cpu_type_names);
//    for (int i = 0; i < cpu_type_names_size; i++) {
//        if (cpu_type == cpu_type_names[i].cputype) return cpu_type_names[i].cpu_name;
//    }
//    return "unknown";
//}

//uint32_t read_magic(FILE *obj_file, int offset);
//
//int is_magic_64(uint32_t magic);
//
//int should_swap_bytes(uint32_t magic);
//
//void *load_bytes(FILE *obj_file, int offset, int size);
//
//void dump_segments(FILE *obj_file);
//
//void dump_mach_header(FILE *obj_file, int offset, int is_64, int is_swap);
//
//void dump_segment_commands(FILE *obj_file, int offset, int is_64, int is_swap, uint32_t ncmds);
//
//void dump_section(FILE *obj_file, int is_swap, uint32_t offset, struct segment_command *segment);
//
//void dump_section_64(FILE *obj_file, int is_swap, uint64_t offset, struct segment_command_64 *segment);
//
//void dump_symbol_table_commands(FILE *obj_file, int offset, int is_swap, struct symtab_command *symtab);
//
//void dump_symbol_table_commands_64(FILE *obj_file, int offset, int is_swap, struct symtab_command *symtab);
//
//void dump_symbol_table_debug_symbol(char *strtab, struct symtab_command *symtab, struct nlist *list);
//
//void dump_symbol_table_common_symbol(char *strtab, struct symtab_command *symtab, struct nlist *list);
//
//void dump_symbol_table_debug_symbol_64(char *strtab, struct symtab_command *symtab, struct nlist_64 *list);
//
//void dump_symbol_table_common_symbol_64(char *strtab, struct symtab_command *symtab, struct nlist_64 *list);
//
//void dump_dwarf(FILE *obj_file, struct section *setion);
//
//void dump_dwarf_64(FILE *obj_file, struct section_64 *setion);

//void cxx_demangle(const char *mangleName, char **demangleName);

int main(int argc, const char *argv[]) {
	@autoreleasepool {
		// insert code here...
		// 解析 dysm 文件
		const char *path[3] = {
		    "/Users/king/Desktop/test-iphonesimulator",
		    "/Users/king/Desktop/test-iphoneos",
		    "/Users/king/Desktop/ShowStart",
		};
		for (int i = 0; i < 3; i++) {
			const char *p = path[i];
			printf("parse dysm begin: %s\n\n", p);
			parse_macho(p);
			printf("\nparse dysm end: %s\n", p);
			printf("\n");
		}
	}
	return 0;
}

//uint32_t read_magic(FILE *obj_file, int offset) {
//    uint32_t magic;
//    fseek(obj_file, offset, SEEK_SET);
//    fread(&magic, sizeof(uint32_t), 1, obj_file);
//    return magic;
//}
//
//int is_magic_64(uint32_t magic) {
//    return magic == MH_MAGIC_64 || magic == MH_CIGAM_64;
//}
//
//int should_swap_bytes(uint32_t magic) {
//    return magic == MH_CIGAM || magic == MH_CIGAM_64;
//}
//
//void *load_bytes(FILE *obj_file, int offset, int size) {
//    void *buf = calloc(1, size);
//    fseek(obj_file, offset, SEEK_SET);
//    fread(buf, size, 1, obj_file);
//    return buf;
//}
//
//
//
//void dump_segments(FILE *obj_file) {
//    uint32_t magic = read_magic(obj_file, 0);
//    int is_64      = is_magic_64(magic);
//    int is_swap    = should_swap_bytes(magic);
//    dump_mach_header(obj_file, 0, is_64, is_swap);
//}
//
//void dump_mach_header(FILE *obj_file, int offset, int is_64, int is_swap) {
//    uint32_t ncmds;
//    int load_commands_offset = offset;
//
//    if (is_64) {
//        int header_size               = sizeof(struct mach_header_64);
//        struct mach_header_64 *header = load_bytes(obj_file, offset, header_size);
//        if (is_swap) {
//            swap_mach_header_64(header, NX_LittleEndian);
//        }
//        ncmds = header->ncmds;
//        load_commands_offset += header_size;
//        printf("CPU: %s\n", cpu_type_name(header->cputype));
//        free(header);
//        header = NULL;
//    } else {
//        int header_size            = sizeof(struct mach_header);
//        struct mach_header *header = load_bytes(obj_file, offset, header_size);
//        if (is_swap) {
//            swap_mach_header(header, NX_LittleEndian);
//        }
//        ncmds = header->ncmds;
//        load_commands_offset += header_size;
//        printf("CPU: %s\n", cpu_type_name(header->cputype));
//        free(header);
//        header = NULL;
//    }
//    dump_segment_commands(obj_file, load_commands_offset, is_64, is_swap, ncmds);
//}
//
//void dump_segment_commands(FILE *obj_file, int offset, int is_64, int is_swap, uint32_t ncmds) {
//    int actual_offset = offset;
//    for (int i = 0; i < ncmds; i++) {
//        struct load_command *cmd = load_bytes(obj_file, actual_offset, sizeof(struct load_command));
//        if (is_swap) {
//            swap_load_command(cmd, NX_LittleEndian);
//        }
//
//        switch (cmd->cmd) {
//            case LC_SEGMENT_64: {
//                struct segment_command_64 *segment = load_bytes(obj_file, actual_offset, sizeof(struct segment_command_64));
//                if (is_swap) {
//                    swap_segment_command_64(segment, NX_LittleEndian);
//                }
//                printf("LC_SEGMENT_64: %s\n", segment->segname);
//                uint64_t offset = 0;
//                if (segment->fileoff == 0 && segment->filesize == 0) {
//                    offset = actual_offset + sizeof(struct segment_command_64);
//                } else {
//                    offset = segment->fileoff;
//                }
//                dump_section_64(obj_file, is_swap, offset, segment);
//                free(segment);
//                break;
//            }
//            case LC_SEGMENT: {
//                struct segment_command *segment = load_bytes(obj_file, actual_offset, sizeof(struct segment_command));
//                if (is_swap) {
//                    swap_segment_command(segment, NX_LittleEndian);
//                }
//                printf("LC_SEGMENT: %s\n", segment->segname);
//                uint32_t offset = 0;
//                if (segment->fileoff == 0 && segment->filesize == 0) {
//                    offset = actual_offset + sizeof(struct segment_command);
//                } else {
//                    offset = segment->fileoff;
//                }
//                dump_section(obj_file, is_swap, offset, segment);
//                free(segment);
//                break;
//            }
//            case LC_UUID: {
//                struct uuid_command *uuidcmd = load_bytes(obj_file, actual_offset, sizeof(struct uuid_command));
//                if (is_swap) {
//                    swap_uuid_command(uuidcmd, NX_LittleEndian);
//                }
//                printf("LC_UUID: ");
//                for (int i = 0; i < 16; i++) {
//                    if (i > 0 && (i) % 4 == 0) {
//                        printf("-");
//                    }
//                    printf("%02X", uuidcmd->uuid[i]);
//                }
//                printf("\n");
//                free(uuidcmd);
//                break;
//            }
//            case LC_SYMTAB: {
//                struct symtab_command *symtab = load_bytes(obj_file, actual_offset, sizeof(struct symtab_command));
//                if (is_swap) {
//                    swap_symtab_command(symtab, NX_LittleEndian);
//                }
//                printf("LC_SYMTAB:\n");
//                printf("\tSymbol Table Offset: %d\n", symtab->symoff);
//                printf("\tNumber of Symbols: %d\n", symtab->nsyms);
//                printf("\tString Table Offset: %d\n", symtab->stroff);
//                printf("\tString Table Size: %d\n", symtab->strsize);
//
//                if (is_64) {
//                    dump_symbol_table_commands_64(obj_file, symtab->symoff, is_swap, symtab);
//                } else {
//                    dump_symbol_table_commands(obj_file, symtab->symoff, is_swap, symtab);
//                }
//                break;
//            }
//            default:
//                break;
//        }
//
//        actual_offset += cmd->cmdsize;
//        free(cmd);
//    }
//}
//
//void dump_section(FILE *obj_file, int is_swap, uint32_t offset, struct segment_command *segment) {
//    if (segment->nsects == 0) return;
//    void *buffer = calloc(1, segment->nsects * sizeof(struct section));
//    fseek(obj_file, offset, SEEK_SET);
//    fread(buffer, segment->nsects * sizeof(struct section), 1, obj_file);
//    struct section *list = (struct section *)buffer;
//    if (is_swap) {
//        swap_section(list, segment->nsects, NX_LittleEndian);
//    }
//
//    if (strcmp(segment->segname, "__DWARF") == 0) {
//        printf("\tSection Header (%s %s)\n", NSSTRING(list->sectname).UTF8String, NSSTRING(list->segname).UTF8String);
//        for (uint32_t num = 0; num < segment->nsects; num++) {
//            dump_dwarf(obj_file, list);
//            list++;
//        }
//    } else {
//        for (uint32_t num = 0; num < segment->nsects; num++) {
//            printf("\tSection Header (%s %s)\n", NSSTRING(list->sectname).UTF8String, NSSTRING(list->segname).UTF8String);
//            list++;
//        }
//    }
//    free(buffer);
//    buffer = NULL;
//}
//
//void dump_section_64(FILE *obj_file, int is_swap, uint64_t offset, struct segment_command_64 *segment) {
//    if (segment->nsects == 0) return;
//    void *buffer = calloc(1, segment->nsects * sizeof(struct section_64));
//    fseek(obj_file, offset + sizeof(struct segment_command_64), SEEK_SET);
//    fread(buffer, segment->nsects * sizeof(struct section_64), 1, obj_file);
//    struct section_64 *list = (struct section_64 *)buffer;
//    if (is_swap) {
//        swap_section_64(list, segment->nsects, NX_LittleEndian);
//    }
//    if (strcmp(segment->segname, "__DWARF") == 0) {
//        for (uint32_t num = 0; num < segment->nsects; num++) {
//            printf("\tSection64 Header (%s %s)\n", NSSTRING(list->sectname).UTF8String, NSSTRING(list->segname).UTF8String);
//            uint64_t size = htonll(list->size);
//            dump_dwarf_64(obj_file, list);
//            list++;
//        }
//    } else {
//        for (uint32_t num = 0; num < segment->nsects; num++) {
//            printf("\tSection64 Header (%s %s)\n", NSSTRING(list->sectname).UTF8String, NSSTRING(list->segname).UTF8String);
//            list++;
//        }
//    }
//    free(buffer);
//    buffer = NULL;
//}
//
///*
// * The n_type field really contains four fields:
// *    unsigned char N_STAB:3,
// *              N_PEXT:1,
// *              N_TYPE:3,
// *              N_EXT:1;
// * which are used via the following masks.
// */
//#define N_STAB 0xe0
//#define N_PEXT 0x10
//#define N_TYPE 0x0e
//#define N_EXT 0x01
///*
// N_STAB 指明了这个是调试符号。这时整个n_type可能是包括源文件，目标文件，包含文件或者方法。那么nl_data.n_un.n_strx处则代表着符号名称（如果有的话）的偏移。在N_FUN时某个项是代表这个Function所占的字节大小，此时没有名称。
//
// 目标文件的最后还有一个日期结构，下面的代码展示了如何显示成人能看得懂的日期，这个数字是一个秒数，从某个固定时刻到现在为止的秒数。转成tm结构后，它也有自己的规定，显示从1900年到现在的年数，月份则是从0-11,也是醉了，搞错一个，都不能显示正确的时间。
//
// 如果是N_FUN或者N_STSYM，那么nl_data.n_un.n_strx处则代表着符号名称的偏移。
// */
//
//void dump_symbol_table_commands(FILE *obj_file, int offset, int is_swap, struct symtab_command *symtab) {
//    void *buffer = calloc(1, symtab->nsyms);
//    fseek(obj_file, symtab->symoff, SEEK_SET);
//    fread(buffer, symtab->nsyms, 1, obj_file);
//    struct nlist *list = (struct nlist *)buffer;
//    if (is_swap) {
//        swap_nlist(list, symtab->nsyms, NX_LittleEndian);
//    }
//    printf("Symbols: \n");
//    char *strtab = (char *)load_bytes(obj_file, symtab->stroff, symtab->strsize);
//    for (uint32_t i = 0; i < symtab->nsyms / sizeof(struct nlist); i++) {
//        bool bDebuggingSymbol = (list->n_type & N_STAB) != 0;
//        if (bDebuggingSymbol) {
//            dump_symbol_table_debug_symbol(strtab, symtab, list);
//        } else {
//            dump_symbol_table_common_symbol(strtab, symtab, list);
//        }
//        list++;
//    }
//    free(strtab);
//    free(buffer);
//    strtab = NULL;
//    buffer = NULL;
//}
//
//void dump_symbol_table_commands_64(FILE *obj_file, int offset, int is_swap, struct symtab_command *symtab) {
//    void *buffer = calloc(1, symtab->nsyms);
//    fseek(obj_file, symtab->symoff, SEEK_SET);
//    fread(buffer, symtab->nsyms, 1, obj_file);
//    struct nlist_64 *list = (struct nlist_64 *)buffer;
//    if (is_swap) {
//        swap_nlist_64(list, symtab->nsyms, NX_LittleEndian);
//    }
//    printf("Symbols: \n");
//    char *strtab = (char *)load_bytes(obj_file, symtab->stroff, symtab->strsize);
//    for (uint32_t i = 0; i < symtab->nsyms / sizeof(struct nlist_64); i++) {
//        bool bDebuggingSymbol = (list->n_type & N_STAB) != 0;
//        if (bDebuggingSymbol) {
//            dump_symbol_table_debug_symbol_64(strtab, symtab, list);
//        } else {
//            dump_symbol_table_common_symbol_64(strtab, symtab, list);
//        }
//        list++;
//    }
//    free(strtab);
//    free(buffer);
//    strtab = NULL;
//    buffer = NULL;
//}
//
//void dump_symbol_table_debug_symbol(char *strtab, struct symtab_command *symtab, struct nlist *list) {
//    NSString *symbolName = NSSTRING(strtab + list->n_un.n_strx);
//    printf("\t%s\n", symbolName.UTF8String);
//}
//
//void dump_symbol_table_common_symbol(char *strtab, struct symtab_command *symtab, struct nlist *list) {
//    NSString *symbolName = NSSTRING(strtab + list->n_un.n_strx);
//    printf("\t%s\n", symbolName.UTF8String);
//}
//
//void dump_symbol_table_debug_symbol_64(char *strtab, struct symtab_command *symtab, struct nlist_64 *list) {
//    NSString *symbolName = NSSTRING(strtab + list->n_un.n_strx);
//    printf("\t%s\n", symbolName.UTF8String);
//}
//
//void dump_symbol_table_common_symbol_64(char *strtab, struct symtab_command *symtab, struct nlist_64 *list) {
//    NSString *symbolName = NSSTRING(strtab + list->n_un.n_strx);
//    printf("\t%s\n", symbolName.UTF8String);
//    //    printf("\t\t");
//    //    bool bPrivateSymbol = (list->n_type & N_PEXT) != 0;
//    //    if (bPrivateSymbol) {
//    //        list->n_type &= ~N_PEXT;
//    //    }
//    //    bool bExternalSymbol = (list->n_type & N_EXT) != 0;
//    //    uint8_t type         = (list->n_type & N_TYPE);
//    //
//    //    switch (type) {
//    //        case N_SECT: {
//    //            if (bPrivateSymbol) {
//    //                printf("Private External Symbol");
//    //            } else if (bExternalSymbol) {
//    //                printf("External Symbol");
//    //            } else {
//    //                printf("Non-external Symbol");
//    //            }
//    //            break;
//    //        }
//    //        default:
//    //            break;
//    //    }
//    //    printf("\n");
//}
//
//void dump_dwarf(FILE *obj_file, struct section *setion) {
//}
//
//void dump_dwarf_64(FILE *obj_file, struct section_64 *setion) {
//    char *temp = load_bytes(obj_file, setion->offset, (uint32_t)setion->size);
//    printf("%s", temp);
//    free(temp);
//}
//
//void cxx_demangle(const char *mangleName, char **demangleName) {
//    char *cmd           = "c++filt -n ";
//    size_t len          = strlen(mangleName) + strlen(cmd);
//    char *commandString = (char *)calloc(len, 1);
//    memcpy(commandString, cmd, strlen(cmd));
//    memcpy(commandString + strlen(cmd), mangleName, strlen(mangleName));
//    FILE *fp = popen(commandString, "r");
//    if (fp) {
//        char *result = fgetln(fp, &len);
//        if (len > 0) {
//            *demangleName = (char *)calloc(len, 1);
//            strcpy(*demangleName, result);
//        }
//        pclose(fp);
//        fp = NULL;
//    }
//    free(commandString);
//    commandString = NULL;
//}
