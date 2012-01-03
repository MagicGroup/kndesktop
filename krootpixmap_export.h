#ifndef KROOTPIXMAP_EXPORT_H
#define KROOTPIXMAP_EXPORT_H

#include <kdemacros.h>

#if defined _WIN32 || defined _WIN64
# ifndef KROOTPIXMAP_EXPORT
#  ifdef MAKE_KROOTPIXMAP_LIB
#   define KROOTPIXMAP_EXPORT KDE_EXPORT
#  else
#   define KROOTPIXMAP_EXPORT KDE_IMPORT
#  endif
# endif
#else // UNIX
# define KROOTPIXMAP_EXPORT KDE_EXPORT
#endif

#endif // KROOTPIXMAP_EXPORT_H
