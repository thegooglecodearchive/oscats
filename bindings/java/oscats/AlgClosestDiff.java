/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Closest Diff Algorithm Java Wrapper Class
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

public final class AlgClosestDiff extends Algorithm
{
    protected AlgClosestDiff(long pointer) { super(pointer); }

    public static AlgClosestDiff register(Test test, int num)
    {
      return register(test, "num", num);
    }

    public static AlgClosestDiff register(Test test, java.lang.Object... params)
    {
      AlgClosestDiff alg_data = new AlgClosestDiff(OscatsAlgClosestDiff.new_with_params(params));
      OscatsAlgorithmOverride.register(alg_data, test);
      return alg_data;
    }
}

