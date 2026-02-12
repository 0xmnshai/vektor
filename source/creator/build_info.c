


typedef unsigned long ulong;

#define BUILD_DATE "2026-02-13"
#define BUILD_TIME "03:51:00"
#define BUILD_HASH "0000000000000000000000000000000000000000"
#define BUILD_COMMIT_TIMESTAMP 1770922260
#define BUILD_COMMIT_DATE "2026-02-13"
#define BUILD_COMMIT_TIME "03:51:00"
#define BUILD_BRANCH "main"
#define BUILD_PLATFORM "Mac"
#define BUILD_TYPE "Debug"
#define BUILD_CFLAGS "-g -O0 -D_DEBUG"
#define BUILD_CXXFLAGS "-g -O0 -D_DEBUG"
#define BUILD_LINKFLAGS "-g -O0 -D_DEBUG"
#define BUILD_SYSTEM "CMake"

#ifdef BUILD_DATE

extern char build_date[];
extern char build_time[];
extern char build_hash[];
extern ulong build_commit_timestamp;
extern char build_commit_date[];
extern char build_commit_time[];
extern char build_branch[];
extern char build_platform[];
extern char build_type[];
extern char build_cflags[];
extern char build_cxxflags[];
extern char build_linkflags[];
extern char build_system[];


char build_date[] = BUILD_DATE;
char build_time[] = BUILD_TIME;
char build_hash[] = BUILD_HASH;
ulong build_commit_timestamp = BUILD_COMMIT_TIMESTAMP;
char build_commit_date[16] = "\0";
char build_commit_time[16] = "\0";
char build_branch[] = BUILD_BRANCH;

char build_platform[] = BUILD_PLATFORM;
char build_type[] = BUILD_TYPE;

#ifdef BUILD_CFLAGS
char build_cflags[] = BUILD_CFLAGS;
char build_cxxflags[] = BUILD_CXXFLAGS;
char build_linkflags[] = BUILD_LINKFLAGS;
char build_system[] = BUILD_SYSTEM;
#else
char build_cflags[] = "unmaintained buildsystem alert!";
char build_cxxflags[] = "unmaintained buildsystem alert!";
char build_linkflags[] = "unmaintained buildsystem alert!";
char build_system[] = "unmaintained buildsystem alert!";
#endif

#endif
