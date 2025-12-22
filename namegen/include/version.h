#ifndef VERSION_H
#define VERSION_H

// Versio: MAJOR.MINOR.PATCH
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 0

// Numeroarvo (esim. 00418)
#define VERSION (VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH)

// Makro merkkijonoksi
#define STR(x) #x
#define XSTR(x) STR(x)
#define VERSION_STRING XSTR(VERSION_MAJOR) "." XSTR(VERSION_MINOR) "." XSTR(VERSION_PATCH)

#endif
