/*
 *   Copyright (c) 2021. caoccao.com Sam Cao
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package com.caoccao.javet.interop.engine;

import com.caoccao.javet.exceptions.JavetException;
import com.caoccao.javet.interfaces.IJavetLogger;
import com.caoccao.javet.interop.V8Runtime;
import com.caoccao.javet.utils.JavetDateTimeUtils;

import java.time.Duration;
import java.time.ZonedDateTime;
import java.util.Objects;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

public class JavetEngineGuard implements IJavetEngineGuard {
    protected long timeoutMillis;
    protected IJavetEngine iJavetEngine;
    protected boolean quitting;
    protected V8Runtime v8Runtime;
    protected Future future;

    public JavetEngineGuard(IJavetEngine iJavetEngine, V8Runtime v8Runtime, long timeoutMills) {
        Objects.requireNonNull(iJavetEngine);
        this.iJavetEngine = iJavetEngine;
        this.timeoutMillis = timeoutMills;
        quitting = false;
        this.v8Runtime = v8Runtime;
        future = this.iJavetEngine.getConfig().getExecutorService().submit(this);
    }

    @Override
    public void cancel() {
        quitting = true;
    }

    @Override
    public void close() throws JavetException {
        cancel();
        if (!future.isDone() && !future.isCancelled()) {
            future.cancel(true);
        }
    }

    @Override
    public long getTimeoutMillis() {
        return timeoutMillis;
    }

    @Override
    public void setTimeoutMillis(long timeoutSeconds) {
        this.timeoutMillis = timeoutSeconds;
    }

    public boolean isQuitting() {
        return quitting;
    }

    protected ZonedDateTime getUTCNow() {
        return JavetDateTimeUtils.getUTCNow();
    }

    @Override
    public void run() {
        JavetEngineConfig config = iJavetEngine.getConfig();
        IJavetLogger logger = config.getJavetLogger();
        ZonedDateTime startZonedDateTime = getUTCNow();
        while (!isQuitting() && iJavetEngine.isActive()) {
            ZonedDateTime currentZonedDateTime = getUTCNow();
            if (startZonedDateTime.plusNanos(TimeUnit.MILLISECONDS.toNanos(timeoutMillis))
                    .isBefore(currentZonedDateTime)) {
                try {
                    if (v8Runtime.isInUse()) {
                        // Javet only terminates the execution when V8 runtime is in use.
                        v8Runtime.terminateExecution();
                        Duration duration = Duration.between(startZonedDateTime, currentZonedDateTime);
                        logger.logWarn("Execution was terminated after {0}ms.", duration.toMillis());
                    }
                } catch (Exception e) {
                    logger.error(e.getMessage(), e);
                } finally {
                    break;
                }
            } else {
                try {
                    Thread.sleep(config.getEngineGuardCheckIntervalMillis());
                } catch (InterruptedException e) {
                    // It's closed.
                }
            }
        }
        quitting = true;
    }
}
