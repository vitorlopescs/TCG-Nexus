#ifndef TCGCORE_GLOBAL_H
#define TCGCORE_GLOBAL_H

#include <QtCore/qglobal.h>

/**
 * @file tcgcore_global.h
 * @brief Macros de exportação/importação de símbolos para a biblioteca TCGCore.
 *
 * Quando o próprio módulo TCGCore é compilado, o qmake define TCGCORE_LIBRARY
 * (ver TCGCore.pro -> DEFINES += TCGCORE_LIBRARY) e as classes marcadas com
 * TCGCORE_EXPORT são exportadas (Q_DECL_EXPORT) da DLL. Quando o TCGGUI apenas
 * consome a DLL, a macro não está definida e as mesmas classes são importadas
 * (Q_DECL_IMPORT). Sem isso, o link do TCGGUI contra -lTCGCore pode falhar no
 * Windows (MinGW/MSVC).
 */
#if defined(TCGCORE_LIBRARY)
#  define TCGCORE_EXPORT Q_DECL_EXPORT
#else
#  define TCGCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // TCGCORE_GLOBAL_H
