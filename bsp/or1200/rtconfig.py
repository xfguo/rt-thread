# for module compiling
import os

# toolchains options
ARCH = 'openrisc'
CPU = 'or1200'
CROSS_TOOL 	= 'gcc'

if os.getenv('RTT_CC'):
	CROSS_TOOL = os.getenv('RTT_CC')

# cross_tool provides the cross compiler
# ONLY GCC SUPPORTED
if  CROSS_TOOL == 'gcc':
	PLATFORM 	= 'gcc'
	EXEC_PATH 	= '/opt/orx/orxsys/staging_dir/toolchain-or1k/bin'
elif CROSS_TOOL == 'keil':
    print '================ERROR============================'
    print 'Not support keil yet!'
    print '================================================='
    exit(0)
elif CROSS_TOOL == 'iar':
    print '================ERROR============================'
    print 'Not support iar yet!'
    print '================================================='
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
	EXEC_PATH = os.getenv('RTT_EXEC_PATH')

# BUILD = 'debug'
BUILD = 'release'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'or1k-elf-'
    CC = PREFIX + 'gcc'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    LINK = PREFIX + 'ld'
    TARGET_EXT = 'elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'

    DEVICE = ' -mhard-mul -mhard-div -msoft-float'
    # DEVICE = ' -msoft-mul -msoft-div -msoft-float'
    
    CFLAGS = DEVICE + ' -Wall -fno-builtin' #+ '-Wunknown-pragmas'
    # AFLAGS = ' -c -x assembler-with-cpp' + DEVICE
    AFLAGS = ' -c ' + DEVICE
    #LFLAGS = DEVICE + ' -Bstatic -nostdlib -u _reset --stats -T link.lds'
    #LFLAGS = ' -Bstatic -nostdlib --stats -T link.lds -L /opt/or32-new/lib/gcc/or32-elf/4.5.1-or32-1.0rc4/libgcc.a'
    LFLAGS = ' -Bstatic -nostdlib --stats -T link.lds -L /opt/orx/orxsys/staging_dir/toolchain-or1k/lib/gcc/or1k-elf/4.9.0/compat-delay/soft-float/libgcc.a'
    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -g3 -Wall'
        AFLAGS += ' -g3'
    else:
        CFLAGS += ' -O2 -Wall'

    POST_ACTION = OBJDUMP + ' -x -d -S $TARGET >> system.map\n' \
        + OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
        
