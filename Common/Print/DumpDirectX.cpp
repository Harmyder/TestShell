#include "stdafx.h"
#include "DumpDirectX.h"

#include <sstream>
#include <iomanip> 

using namespace DirectX;
using namespace std;

namespace Common
{
    static const char* SelectStyle(DumpStyle style, const char* vs, const char* matlab) {
        if (style == DumpStyle::kVisualStudio) {
            return vs;
        }
        else if (style == DumpStyle::kMatlab) {
            return matlab;
        }
        throw "Unknown style";
    }

    static const char* GlobalOpener(DumpStyle style) { return SelectStyle(style, "{", "["); }
    static const char* GlobalCloser(DumpStyle style) { return SelectStyle(style, "};", "];"); }
    static const char* LocalOpener(DumpStyle style) { return SelectStyle(style, "{", ""); }
    static const char* LocalCloser(DumpStyle style) { return SelectStyle(style, "}", ""); }
    static const char* GlobalSeparator(DumpStyle style) { return SelectStyle(style, ", ", "; "); }
    static const char* LocalSeparator(DumpStyle style) { return SelectStyle(style, ", ", " "); }

    static void FillStrings(DumpStyle style, string& go, string& gc, string& lo, string& lc, string& gs, string& ls) {
        go = GlobalOpener(style);
        gc = GlobalCloser(style);
        lo = LocalOpener(style);
        lc = LocalCloser(style);
        gs = GlobalSeparator(style);
        ls = LocalSeparator(style);
    }

    static void FormatStream(stringstream& ss) {
        ss << setw(10) << setprecision(6) << endl;
    }
    
#define PREPARE_XMFLOAT_DUMP                    \
    string go, gc, lo, lc, gs, ls;              \
    FillStrings(style, go, gc, lo, lc, gs, ls); \
    string endline = isEndLine ? "\n" : "";     \
                                                \
    stringstream res;                           \
    FormatStream(res)

    string Dump(const XMFLOAT3* points, uint32 pointsCount, DumpStyle style, bool isEndLine) {
        PREPARE_XMFLOAT_DUMP;

        res << go;
        for (uint32 i = 0; i < pointsCount; ++i) {
            res << lo;
            res << points[i].x << ls << points[i].y << ls << points[i].z;
            res << lc;
            if (i != pointsCount - 1) res << gs << endline;
        }
        res << gc << endline;

        return res.str();
    }

    static string DumpMatrix(const float* matrix, uint32 dimI, uint32 dimJ, DumpStyle style, bool isEndLine) {
        PREPARE_XMFLOAT_DUMP;

        res << go;
        for (uint32 i = 0; i < dimI; ++i) {
            res << lo;
            for (uint32 j = 0; j < dimJ; ++j) {
                res << matrix[i * dimJ + j];
                if (j != dimJ - 1) res << ls;
            }
            res << lc;
            if (i != dimI - 1) res << gs << endline;
        }
        res << gc << endline;
        return res.str();
    }

    string Dump(const XMFLOAT3X3& matrix, DumpStyle style, bool isEndLine) {
        return DumpMatrix((const float*)&matrix, 3, 3, style, isEndLine);
    }

    string Dump(const XMFLOAT4X4& matrix, DumpStyle style, bool isEndLine) {
        return DumpMatrix((const float*)&matrix, 4, 4, style, isEndLine);
    }
}
