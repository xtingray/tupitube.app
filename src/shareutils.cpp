//=============================================================================
// Copyright (c) 2014 Nicolas Froment

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//=============================================================================

#include "shareutils.h"

ShareUtils::ShareUtils(QQuickItem *parent) : QQuickItem(parent)
{
#if defined(Q_OS_ANDROID)
    pShareUtils = new AndroidShareUtils(this);
#else
    pShareUtils = new PlatformShareUtils(this);
#endif
}

void ShareUtils::setLog(const QString &log)
{
    pShareUtils->setLog(log);
}

void ShareUtils::shareUrl(const QString &text, const QUrl &url)
{
    pShareUtils->shareUrl(text, url);
}

void ShareUtils::shareMedia(const QString &target)
{
    pShareUtils->shareMedia(target);
}
