/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QTimer>
#include <QEventLoop>
#include <QObject>

#include "catch/catch.hpp"

/**
 * @file Useful test harnesses and utilities, this file should include all others.
 */

namespace testUtils {

/**
 * Spins until either `msec` milliseconds pass or the `callback` signal fires, if it times out, the test will fail.
 */
template <class T, typename F>
void spinUntil(T * obj, F callback, const int msec = 400)
{
    //Spin until we get the reply or timeout
    QEventLoop eLoop;

    QTimer timer;
    bool timeOutFail = false;
    QObject::connect(&timer, &QTimer::timeout, [&](){ timeOutFail = true; eLoop.quit(); });

    timer.start(msec);
    QObject::connect(obj, callback, &eLoop, &QEventLoop::quit);
    eLoop.exec();

    // if the timer is still active, it timed out, thus the test failed.
    REQUIRE(!timeOutFail);
}

} // end namespace testUtils

#endif // TESTUTILS_H
