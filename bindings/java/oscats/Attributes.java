/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Attributes Java Wrapper Class
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

package oscats;

import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;
import oscats.glib.Object;

public final class Attributes extends Object
{
    protected Attributes(long pointer) { super(pointer); }

    static public Attributes fromString(String str) {
      return OscatsAttributes.fromString(str);
    }
    
    public void set(byte index, boolean value) {
      OscatsAttributes.set(this, index, value);
    }
    
    public boolean get(byte index) {
      return OscatsAttributes.get(this, index);
    }
    
    public int asInt() { return OscatsAttributes.asInt(this); }
    
    public String asString() { return OscatsAttributes.asString(this); }
    
    public void copy(Attributes rhs) { OscatsAttributes.copy(this, rhs); }
    
    public int compare(Attributes rhs) { return OscatsAttributes.compare(this, rhs); }

}

