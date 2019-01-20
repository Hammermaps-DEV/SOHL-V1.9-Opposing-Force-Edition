//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VMINMAX_H
#define VMINMAX_H

#if defined ( _MSC_VER )
#pragma once
#endif

#define V_max(a, b)  (((a) > (b)) ? (a) : (b))
#define V_min(a, b)  (((a) < (b)) ? (a) : (b))

#endif // VMINMAX_H