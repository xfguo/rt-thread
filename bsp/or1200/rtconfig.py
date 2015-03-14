# for module compiling
import os

# toolchains options
ARCH = 'openrisc'
CPU = 'or1200'
CROSS_TOOL = 'gcc'

if os.getenv('RTT_CC'):
	CROSS_TOOL = os.getenv('RTT_CC')

# cross_tool provides the cross compiler
# ONLY GCC SUPPORTED
if CROSS_TOOL == 'gcc':
	PLATFORM = 'gcc'
	EXEC_PATH = ''
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

if EXEC_PATH == "":
	print '================ERROR============================'
	print 'cannot find EXEC_PATh, set RTT_EXEC_PATH to the toolchain execute path.'
	print '================================================='
	exit(0)

if os.path.exists(EXEC_PATH) is False:
	print '================ERROR============================'
	print 'path "%s" does not exist, set RTT_EXEC_PATH please.' % EXEC_PATH
	print '================================================='
	exit(0)

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

	DEVICE = ['-mhard-mul', '-mhard-div', '-msoft-float']
	
	CFLAGS = DEVICE + ['-Wall']
	AFLAGS = ['-c'] + DEVICE
	LFLAGS = ['-Bstatic', '-nostdlib', '--stats', '-T', 'link.lds']
	CPATH = ''
	LPATH = ''

	if BUILD == 'debug':
		CFLAGS += ['-O0', '-g3', '-Wall']
		AFLAGS += ['-g3']
	else:
		CFLAGS += ['-O2', '-Wall']

	POST_ACTION = "\n".join([
		OBJDUMP + ' -x -d -S $TARGET >> system.map',
		OBJCPY + ' -O binary $TARGET rtthread-or1200.bin',
		SIZE + ' $TARGET'
	])
