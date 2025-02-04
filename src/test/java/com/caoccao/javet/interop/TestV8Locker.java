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

package com.caoccao.javet.interop;

import com.caoccao.javet.BaseTestJavetRuntime;
import com.caoccao.javet.exceptions.JavetException;
import com.caoccao.javet.exceptions.JavetV8LockConflictException;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class TestV8Locker extends BaseTestJavetRuntime {
    @Test
    public void testExceptionInAcquire() throws JavetException {
        try (V8Locker v8Locker = v8Runtime.getV8Locker()) {
            assertEquals(2, v8Runtime.getExecutor("1 + 1").executeInteger());
            assertThrows(JavetV8LockConflictException.class, () -> v8Runtime.getV8Locker());
        }
    }

    @Test
    public void testExceptionInClose() throws JavetException {
        V8Locker v8Locker = v8Runtime.getV8Locker();
        assertEquals(2, v8Runtime.getExecutor("1 + 1").executeInteger());
        v8Locker.close();
        assertThrows(JavetV8LockConflictException.class, () -> v8Locker.close());
    }
}
