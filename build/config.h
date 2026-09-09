#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define BUILD_GIT_VERSION "6aba64f"
#ifdef BUILD_GIT_VERSION
#define BUILD_DATE __DATE__ " (git " BUILD_GIT_VERSION ")"
#else
#define BUILD_DATE __DATE__
#endif

#endif // CONFIG_H_INCLUDED

