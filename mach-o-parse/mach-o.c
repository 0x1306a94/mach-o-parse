//
//  mach-o.c
//  mach-o-parse
//
//  Created by king on 2019/7/23.
//  Copyright © 2019 K&K. All rights reserved.
//

#include "mach-o.h"

#include <fcntl.h>
#include <malloc/_malloc.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const struct _cpu_type_names cpu_type_names[] = {
    {CPU_TYPE_I386, "i386"},
    {CPU_TYPE_X86_64, "x86_64"},
    {CPU_TYPE_ARM, "ARM"},
    {CPU_TYPE_ARM64, "ARM64"},
};

const struct _cpu_sub_type_names cpu_sub_type_names[] = {
    {CPU_SUBTYPE_ARM_ALL, "ALL"},
    {CPU_SUBTYPE_ARM_V4T, "V4T"},
    {CPU_SUBTYPE_ARM_V6, "V6"},
    {CPU_SUBTYPE_ARM_V5TEJ, "V5TEJ"},
    {CPU_SUBTYPE_ARM_XSCALE, "XSCALE"},
    {CPU_SUBTYPE_ARM_V7, "V7"},
    {CPU_SUBTYPE_ARM_V7F, "V7F"},
    {CPU_SUBTYPE_ARM_V7K, "V7K"},
    {CPU_SUBTYPE_ARM_V7S, "V7S"},
    {CPU_SUBTYPE_ARM_V8, "V8"},
    {CPU_SUBTYPE_ARM64_ALL, "ALL"},
    {CPU_SUBTYPE_ARM64_V8, "V8"},
    {CPU_SUBTYPE_ARM64E, "E"},
};

const char *cpu_type_name(cpu_type_t cpu_type) {
	static int cpu_type_names_size = sizeof(cpu_type_names) / sizeof(struct _cpu_type_names);
	for (int i = 0; i < cpu_type_names_size; i++) {
		if (cpu_type == cpu_type_names[i].cputype) return cpu_type_names[i].cpu_name;
	}
	return "???";
}

const char *cpu_sub_type_name(cpu_subtype_t cpu_sub_type) {
	static int cpu_type_names_size = sizeof(cpu_sub_type_names) / sizeof(struct _cpu_sub_type_names);
	for (int i = 0; i < cpu_type_names_size; i++) {
		if (cpu_sub_type == cpu_sub_type_names[i].cpusubtype) return cpu_sub_type_names[i].cpu_name;
	}
	return "???";
}

uint32_t read_magic(void *macho, int offset) {
	uint32_t magic = *(uint32_t *)((uint8_t *)macho + offset);
	return magic;
}

int is_magic_64(uint32_t magic) {
	return magic == MH_MAGIC_64 || magic == MH_CIGAM_64 || magic == FAT_CIGAM_64;
}

int should_swap_bytes(uint32_t magic) {
	return magic == MH_CIGAM || magic == MH_CIGAM_64 || magic == FAT_CIGAM || magic == FAT_CIGAM_64;
}

void parse_macho(const char *path) {

	void *macho = NULL;
	int fd      = open(path, O_RDONLY);
	struct stat sb;
	fstat(fd, &sb);
	macho = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (macho == MAP_FAILED) {
		printf("mmap failed\n");
		return;
	}

	uint32_t magic = read_magic(macho, 0);
	int is_64      = is_magic_64(magic);
	int is_swap    = should_swap_bytes(magic);
	switch (magic) {
		case MH_CIGAM_64:
		case MH_MAGIC_64: {
			struct mach_header_64 mh = {0};
			memcpy(&mh, macho, sizeof(struct mach_header_64));
			if (is_swap) {
				swap_mach_header_64(&mh, NX_LittleEndian);
			}
			if (mh.cputype == CPU_TYPE_ARM || mh.cputype == CPU_TYPE_ARM64) {
				printf("CPU: %s_%s\n", cpu_type_name(mh.cputype), cpu_sub_type_name(mh.cpusubtype));
			} else {
				printf("CPU: %s\n", cpu_type_name(mh.cputype));
			}
			struct Context ctx = {
			    .header_offset       = 0,
			    .load_command_offset = sizeof(struct mach_header_64),
			    .ncmds               = mh.ncmds,
			    .is_64               = is_magic_64(magic),
			    .is_swap             = is_swap,
			};
			process_mach_header_64(macho, &ctx);
			break;
		}
		case MH_CIGAM:
		case MH_MAGIC: {
			struct mach_header mh = {0};
			memcpy(&mh, macho, sizeof(struct mach_header));
			if (is_swap) {
				swap_mach_header(&mh, NX_LittleEndian);
			}
			if (mh.cputype == CPU_TYPE_ARM || mh.cputype == CPU_TYPE_ARM64) {
				printf("CPU: %s_%s\n", cpu_type_name(mh.cputype), cpu_sub_type_name(mh.cpusubtype));
			} else {
				printf("CPU: %s\n", cpu_type_name(mh.cputype));
			}
			struct Context ctx = {
			    .header_offset       = 0,
			    .load_command_offset = sizeof(struct mach_header_64),
			    .ncmds               = mh.ncmds,
			    .is_64               = is_magic_64(magic),
			    .is_swap             = is_swap,
			};
			process_mach_header(macho, &ctx);
			break;
		}
		case FAT_CIGAM:
		case FAT_MAGIC: {

			struct fat_header mh = {0};
			memcpy(&mh, macho, sizeof(struct fat_header));
			is_64   = is_magic_64(magic);
			is_swap = should_swap_bytes(magic);
			if (is_swap) {
				swap_fat_header(&mh, NX_LittleEndian);
			}
			for (uint32_t i = 0; i < mh.nfat_arch; i++) {
				// reload fat_header
				struct fat_arch arch = {0};
				memcpy(&arch, macho + sizeof(struct fat_header) + i * sizeof(struct fat_arch), sizeof(struct fat_arch));
				swap_fat_arch(&arch, 1, NX_LittleEndian);
				magic   = read_magic(macho, arch.offset);
				is_64   = is_magic_64(magic);
				is_swap = should_swap_bytes(magic);

				if (arch.cputype == CPU_TYPE_ARM || arch.cputype == CPU_TYPE_ARM64) {
					printf("CPU: %s_%s\n", cpu_type_name(arch.cputype), cpu_sub_type_name(arch.cpusubtype));
				} else {
					printf("CPU: %s\n", cpu_type_name(arch.cputype));
				}
				if (is_64) {
					struct mach_header_64 amh = {0};
					int _is_swap              = should_swap_bytes(amh.magic);
					memcpy(&amh, macho + arch.offset, sizeof(struct mach_header_64));
					if (_is_swap) {
						swap_mach_header_64(&amh, NX_LittleEndian);
					}
					struct Context ctx = {
					    .header_offset       = arch.offset,
					    .load_command_offset = arch.offset + sizeof(struct mach_header_64),
					    .ncmds               = amh.ncmds,
					    .is_64               = is_magic_64(amh.magic),
					    .is_swap             = _is_swap,
					};
					process_fat_arch(macho, &ctx);
				} else {
					struct mach_header amh = {0};
					int _is_swap           = should_swap_bytes(amh.magic);
					memcpy(&amh, macho + arch.offset, sizeof(struct mach_header));
					if (_is_swap) {
						swap_mach_header(&amh, NX_LittleEndian);
					}
					struct Context ctx = {
					    .header_offset       = arch.offset,
					    .load_command_offset = arch.offset + sizeof(struct mach_header),
					    .ncmds               = amh.ncmds,
					    .is_64               = is_magic_64(amh.magic),
					    .is_swap             = _is_swap,
					};
					process_fat_arch(macho, &ctx);
				}
			}
			break;
		}
		case FAT_CIGAM_64:
		case FAT_MAGIC_64: {

			break;
		}
		default:
			break;
	}

	munmap(macho, sb.st_size);
	close(fd);
}

void process_mach_header(void *macho, struct Context *ctx) {
	struct load_command lc = {0};
	int i                  = 0;
	printf("Load Commadns: \n");
	while (i < ctx->ncmds) {
		memcpy(&lc, macho + ctx->load_command_offset, sizeof(struct load_command));
		if (ctx->is_swap) {
			swap_load_command(&lc, NX_LittleEndian);
		}
		int parse_load_command_result = parse_load_command(macho, &lc, ctx);
		if (parse_load_command_result == -1) {
			printf("parse load command failed\n");
			break;
		}
		i++;
	}
}

void process_mach_header_64(void *macho, struct Context *ctx) {
	struct load_command lc = {0};
	int i                  = 0;
	printf("Load Commadns: \n");
	while (i < ctx->ncmds) {
		memcpy(&lc, macho + ctx->load_command_offset, sizeof(struct load_command));
		if (ctx->is_swap) {
			swap_load_command(&lc, NX_LittleEndian);
		}
		int parse_load_command_result = parse_load_command(macho, &lc, ctx);
		if (parse_load_command_result == -1) {
			printf("parse load command failed\n");
			break;
		}
		i++;
	}
}

void process_fat(void *macho, uint32_t location) {
	uint32_t magic = *(uint32_t *)((uint8_t *)macho + location);
	int is_swap    = should_swap_bytes(magic);
	switch (magic) {
		case FAT_CIGAM:
		case FAT_MAGIC: {
			struct fat_header mh = {0};
			memcpy(&mh, macho + location, sizeof(struct fat_header));
			if (is_swap) {
				swap_fat_header(&mh, NX_LittleEndian);
			}
			process_fat_header(macho, &mh);
			break;
		}
		default:
			break;
	}
}

void process_fat_header(void *macho, struct fat_header *mh) {
	for (uint32_t i = 0; i < mh->nfat_arch; i++) {
		struct fat_arch fat_arch = {0};
		memcpy(&fat_arch, macho + sizeof(struct fat_header) + i * sizeof(struct fat_arch), sizeof(struct fat_arch));
		swap_fat_arch(&fat_arch, 1, NX_LittleEndian);
		if (*(uint64_t *)((uint8_t *)macho + fat_arch.offset) == *(uint64_t *)"!<arch>\n") {
			printf("CPU: %s\n", cpu_type_name(fat_arch.cputype));
			struct mach_header amh = {0};
			memcpy(&amh, macho + fat_arch.offset, sizeof(struct mach_header));
			int _is_swap = should_swap_bytes(amh.magic);
			if (_is_swap) {
				swap_mach_header(&amh, NX_LittleEndian);
			}
			struct Context ctx = {
			    .header_offset       = fat_arch.offset,
			    .load_command_offset = fat_arch.offset + sizeof(struct mach_header) + sizeof(struct fat_header),
			    .ncmds               = amh.ncmds,
			    .is_64               = is_magic_64(amh.magic),
			    .is_swap             = _is_swap,
			};
			process_fat_arch(macho, &ctx);
		} else {
			process_fat(macho, fat_arch.offset);
		}
	}
}

void process_fat_arch(void *macho, struct Context *ctx) {

	struct load_command lc = {0};
	int i                  = 0;
	printf("Load Commadns: \n");
	while (i < ctx->ncmds) {
		memcpy(&lc, macho + ctx->load_command_offset, sizeof(struct load_command));
		if (ctx->is_swap) {
			swap_load_command(&lc, NX_LittleEndian);
		}
		int parse_load_command_result = parse_load_command(macho, &lc, ctx);
		if (parse_load_command_result == -1) {
			printf("parse load command failed\n");
			break;
		}
		i++;
	}
}

void process_fat_arch_64(void *macho, struct Context *ctx) {
}

int parse_load_command(void *macho, struct load_command *lc, struct Context *ctx) {
	int load_command_result = 1;
	switch (lc->cmd) {
		case LC_UUID: {
			load_command_result = process_lc_uuid(macho, &ctx->load_command_offset, ctx->is_swap);
			break;
		}
		case LC_SEGMENT: {
			load_command_result = process_lc_segment(macho, &ctx->load_command_offset, ctx->is_swap);
			break;
		}
		case LC_SEGMENT_64: {
			load_command_result = process_lc_segment_64(macho, &ctx->load_command_offset, ctx->is_swap);
			break;
		}
		case LC_SYMTAB: {
			load_command_result = process_lc_symtab(macho, ctx);
			break;
		}
		case LC_DYSYMTAB: {
			break;
		}
		default:
			break;
	}
	return load_command_result;
}

int process_lc_uuid(void *macho, long *offset, int is_swap) {
	struct uuid_command command = {0};
	memcpy(&command, macho + *offset, sizeof(struct uuid_command));
	if (is_swap) {
		swap_uuid_command(&command, NX_LittleEndian);
	}
	*offset += command.cmdsize;
	printf("LC_UUID: ");
	for (int i = 0; i < 16; i++) {
		if (i > 0 && (i) % 4 == 0) printf("-");
		printf("%02X", command.uuid[i]);
	}
	printf("\n");
	return 1;
}

int process_lc_segment(void *macho, long *offset, int is_swap) {
	struct segment_command command = {0};
	memcpy(&command, macho + *offset, sizeof(struct segment_command));
	*offset += sizeof(struct segment_command);
	printf("LC_SEGMENT (%s)\n", command.segname);
	if (strcmp(command.segname, "__DWARF") == 0) {
#warning process DWARF
	}
	*offset += command.cmdsize - sizeof(struct segment_command);
	return 1;
}

int process_lc_segment_64(void *macho, long *offset, int is_swap) {
	struct segment_command_64 command = {0};
	memcpy(&command, macho + *offset, sizeof(struct segment_command_64));
	*offset += sizeof(struct segment_command_64);
	printf("LC_SEGMENT_64 (%s)\n", command.segname);
	if (strcmp(command.segname, "__DWARF") == 0) {
#warning process DWARF
	}
	*offset += command.cmdsize - sizeof(struct segment_command_64);
	return 1;
}

int process_lc_symtab(void *macho, struct Context *ctx) {
	struct symtab_command command = {0};
	memcpy(&command, macho + ctx->load_command_offset, sizeof(struct symtab_command));
	if (ctx->is_swap) {
		swap_symtab_command(&command, NX_LittleEndian);
	}
	ctx->load_command_offset += command.cmdsize;
	printf("LC_SYMTAB:\n");
	printf("\tSymbol Table Offset: %d\n", command.symoff);
	printf("\tNumber of Symbols: %d\n", command.nsyms);
	printf("\tString Table Offset: %d\n", command.stroff);
	printf("\tString Table Size: %d\n", command.strsize);
	printf("\tSymbols\n");
	if (ctx->is_64) {
		parse_lc_symtab_64(macho, &command, ctx);
	} else {
		parse_lc_symtab(macho, &command, ctx);
	}

	return 1;
}

void parse_lc_symtab(void *macho, struct symtab_command *command, struct Context *ctx) {
	char *strtab       = (char *)(macho + ctx->header_offset + command->stroff);
	struct nlist *list = calloc(1, command->nsyms);
	memcpy(list, macho + ctx->header_offset + command->symoff, command->nsyms);
	if (ctx->is_swap) {
		swap_nlist(list, command->nsyms, NX_LittleEndian);
	}

	for (uint32_t i = 0; i < command->nsyms / sizeof(struct nlist); i++) {
		struct nlist l = list[i];
		char *symbol   = strtab + l.n_un.n_strx;
		// C++  mangle symbol
		if (strncmp(strtab + l.n_un.n_strx, "_Z", 2) == 0) {
			symbol = cplus_demangle(symbol);
		} else if (strncmp(strtab + l.n_un.n_strx, "__Z", 3) == 0) {
			symbol = cplus_demangle(symbol + 1);
		}
		printf("\t 0x%.8x %s\n", l.n_value, symbol);
	}
	free(list);
	list = NULL;
}

void parse_lc_symtab_64(void *macho, struct symtab_command *command, struct Context *ctx) {

	char *strtab          = (char *)(macho + ctx->header_offset + command->stroff);
	struct nlist_64 *list = calloc(1, command->nsyms);
	memcpy(list, macho + ctx->header_offset + command->symoff, command->nsyms);
	if (ctx->is_swap) {
		swap_nlist_64(list, command->nsyms, NX_LittleEndian);
	}

	for (uint32_t i = 0; i < command->nsyms / sizeof(struct nlist_64); i++) {
		struct nlist_64 l = list[i];
		char *symbol      = strtab + l.n_un.n_strx;
		// C++  mangle symbol
		if (strncmp(strtab + l.n_un.n_strx, "_Z", 2) == 0) {
			symbol = cplus_demangle(symbol);
		} else if (strncmp(strtab + l.n_un.n_strx, "__Z", 3) == 0) {
			symbol = cplus_demangle(symbol + 1);
		}
		printf("\t 0x%qx %s\n", l.n_value, symbol);
	}

	free(list);
	list = NULL;
}

char *cplus_demangle(const char *mangleName) {
	char *cmd           = "c++filt -n ";
	size_t len          = strlen(mangleName) + strlen(cmd);
	char *commandString = (char *)calloc(len, 1);
	memcpy(commandString, cmd, strlen(cmd));
	memcpy(commandString + strlen(cmd), mangleName, strlen(mangleName));
	FILE *fp     = popen(commandString, "r");
	char *result = NULL;
	if (fp) {
		result = fgetln(fp, &len);
		if (len > 0 && result[len - 1] == '\n') {
            result[len - 1] = '\0';
		}
		pclose(fp);
		fp = NULL;
	}
	free(commandString);
	commandString = NULL;
	return result;
}
