//=============================================================================
//
//   File : KviTalApplication.cpp
//   Creation date : Sun Aug 12 2001 04:34:21 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KviTalApplication.h"

#if defined(COMPILE_KDE4_SUPPORT)
	KviTalApplication::KviTalApplication(int &, char **)
	: KApplication()
	{
	}
#else //!defined(COMPILE_KDE4_SUPPORT)
	KviTalApplication::KviTalApplication(int & iArgc, char ** ppcArgv)
	: QApplication(iArgc,ppcArgv)
	{
		#if (QT_VERSION >= 0x050000)
			// Session management has been broken by source incompatible changes.
			QObject::connect(this,SIGNAL(commitDataRequest(QSessionManager &)),this,SLOT(commitData(QSessionManager &)));
		#endif
	}
#endif //!defined(COMPILE_KDE4_SUPPORT)

KviTalApplication::~KviTalApplication()
{
}

void KviTalApplication::commitData(QSessionManager & manager)
{
	saveConfiguration();
#if defined(COMPILE_KDE4_SUPPORT)
	KApplication::commitData(manager);
#endif //defined(COMPILE_KDE4_SUPPORT)
}


