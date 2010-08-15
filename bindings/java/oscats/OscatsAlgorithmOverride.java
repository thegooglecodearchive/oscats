/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Algorithm Class Java Overrides
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

final class OscatsAlgorithmOverride extends Plumbing
{
  private OscatsAlgorithmOverride() {}
  
  static final void register(Algorithm alg_data, Test test)
  {
    reg(pointerOf(alg_data), pointerOf(test));
  }
  
  private static native final void reg(long alg_data, long test);
}
