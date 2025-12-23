#ifndef VERSION_H
#define VERSION_H

// Versio ja tekijätiedot
#define VERSION_MAJOR 1
#define VERSION_MINOR 3
#define VERSION_PATCH 1
#define VERSION_SUFFIX "Xmas-Edition"
#define VERSION_AUTHOR "Tuomas Lahteenmaki / GeneSim Team"
#define VERSION_GITHUB "https://github.com/lahtis/GeneSim"
#define VERSION_LICENSE "GPL v3.0"

// Numeroarvo (esim. 10300)
#define VERSION (VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH)

// Makrot merkkijonoiksi
#define STR(x) #x
#define XSTR(x) STR(x)

#define VERSION_STRING XSTR(VERSION_MAJOR) "." XSTR(VERSION_MINOR) "." XSTR(VERSION_PATCH) " (" VERSION_SUFFIX ")"

#endif

