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

package com.caoccao.javet.entities;

import java.util.HashMap;
import java.util.Map;

public class JavetEntityMap extends HashMap<String, Object> {
    public JavetEntityMap(int initialCapacity, float loadFactor) {
        super(initialCapacity, loadFactor);
    }

    public JavetEntityMap(int initialCapacity) {
        super(initialCapacity);
    }

    public JavetEntityMap() {
    }

    public JavetEntityMap(Map<? extends String, ?> m) {
        super(m);
    }

    @Override
    public Object clone() {
        return new JavetEntityMap(this);
    }
}
